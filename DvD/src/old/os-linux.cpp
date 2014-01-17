#include "globals.h"
#include "input.h"
#include "error.h"
#include "player.h"
#include "network.h"
#include "graphics.h"

#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <GL/glx.h>

extern int menu;
extern Player madotsuki;
extern Player poniko;

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

namespace OS
{
    unsigned int gameFrame = 0;
    uint64_t epoch;

    bool ctxErrorOccurred = false;
    int ctxErrorHandler( Display *dpy, XErrorEvent *ev )
    {
        ctxErrorOccurred = true;
        return 0;
    }

    Display* display;
    Window win;
    Atom wmDeleteMessage;

    bool in_fullscreen = false;

    //Original screen configuration
    XRRScreenConfiguration* mode_original_configuration;
    short mode_original_rate;
    Rotation mode_original_rotation;
    SizeID mode_original_size;

    //New screen configuration
    int mode_index = -1;
    short mode_rate = 0;

    //Dummy blank cursor
    Cursor cursor;

    void setResolution(bool restore)
    {
        if(mode_index == -1) return;

        if(restore)
        {
            XRRSetScreenConfigAndRate(display, mode_original_configuration, RootWindow(display, 0),
                                      mode_original_size, mode_original_rotation, mode_original_rate, CurrentTime);
            XUndefineCursor(display, win);

        }
        else
        {
            XRRSetScreenConfigAndRate(display, mode_original_configuration, RootWindow(display, 0),
                                          mode_index, RR_Rotate_0, mode_rate, CurrentTime);
            XDefineCursor(display, win, cursor);
        }
    }

    void init()
    {
        //I have no idea what any of this shit does
        //Thank you, http://content.gpwiki.org/index.php/OpenGL:Tutorials:Setting_up_OpenGL_on_X11

        display = XOpenDisplay(0);

        if(!display)
			die("Failed to open X display.");

        static int visual_attribs[] =
        {
            GLX_X_RENDERABLE    , True,
            GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
            GLX_RENDER_TYPE     , GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
            GLX_RED_SIZE        , 8,
            GLX_GREEN_SIZE      , 8,
            GLX_BLUE_SIZE       , 8,
            GLX_ALPHA_SIZE      , 8,
            GLX_DEPTH_SIZE      , 24,
            GLX_STENCIL_SIZE    , 8,
            GLX_DOUBLEBUFFER    , True,
            //GLX_SAMPLE_BUFFERS  , 1,
            //GLX_SAMPLES         , 4,
            None
        };

        //Check the GLX version
        int glx_major, glx_minor;
        if(!glXQueryVersion(display, &glx_major, &glx_minor) || ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
            die("Invalid GLX version.");

        //Check framebuffer configs
        int fbcount;
        GLXFBConfig* fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);

        if(!fbc)
			die("Failed to retrieve a framebuffer config");

        //Pick the FB config/visual with the most samples per pixel
        int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

        int i;
        for (i = 0; i < fbcount; i++)
        {
            XVisualInfo* vi = glXGetVisualFromFBConfig(display, fbc[i]);
            if(vi)
            {
                int samp_buf, samples;
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
                glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES       , &samples );

                if(best_fbc < 0 || (samp_buf && samples) > best_num_samp)
                    best_fbc = i, best_num_samp = samples;
                if(worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                    worst_fbc = i, worst_num_samp = samples;
            }
            XFree(vi);
        }

        GLXFBConfig bestFbc = fbc[best_fbc];

        //Be sure to free the FBConfig list allocated by glXChooseFBConfig()
        XFree(fbc);

        //Get a visual
        XVisualInfo* vi = glXGetVisualFromFBConfig(display, bestFbc);

        //Create colormap
        XSetWindowAttributes swa;
        Colormap cmap;
        swa.colormap = cmap = XCreateColormap(display, RootWindow( display, vi->screen ), vi->visual, AllocNone);
        swa.background_pixmap = None;
        swa.border_pixel      = 0;
        swa.event_mask        = StructureNotifyMask;

        //Create window
        win = XCreateWindow(display, RootWindow(display, vi->screen),
                            0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, vi->depth, InputOutput,
                            vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);

        if(!win)
			die("Failed to create window.");

        //Done with the visual info data
        XFree(vi);

        //Set the title
        XStoreName(display, win, WINDOW_TITLE);

        //Map the window

        //NOTE: It is not necessary to create or make current to a context before
        //calling glXGetProcAddressARB
        glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
        glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

        GLXContext ctx = 0;

        //Install an X error handler so the application won't exit if GL 3.0
        //context allocation fails.
        //
        //Note this error handler is global.  All display connections in all threads
        //of a process use the same error handler, so be sure to guard against other
        //threads issuing X commands while this code is running.
        ctxErrorOccurred = false;
        int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

        //Check for the GLX_ARB_create_context extension string and the function.
        //If either is not present, use GLX 1.3 context creation method.
        if(!GLX_ARB_create_context || !glXCreateContextAttribsARB)
            ctx = glXCreateNewContext( display, bestFbc, GLX_RGBA_TYPE, 0, True );

        //If it does, try to get a GL 3.0 context!
        else
        {
            int context_attribs[] =
            {
                GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
                GLX_CONTEXT_MINOR_VERSION_ARB, 0,
                //GLX_CONTEXT_FLAGS_ARB        , GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                None
            };

            //Create context
            ctx = glXCreateContextAttribsARB( display, bestFbc, 0, True, context_attribs);

            //Sync to ensure any errors generated are processed.
            XSync(display, False);
            if(ctxErrorOccurred || !ctx)
            {
                //Couldn't create GL 3.0 context.  Fall back to old-style 2.x context.
                //When a context version below 3.0 is requested, implementations will
                //return the newest context version compatible with OpenGL versions less
                //than version 3.0.
                //GLX_CONTEXT_MAJOR_VERSION_ARB = 1
                context_attribs[1] = 1;
                //GLX_CONTEXT_MINOR_VERSION_ARB = 0
                context_attribs[3] = 0;

                ctxErrorOccurred = false;

                ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);
            }
        }

        //Sync to ensure any errors generated are processed.
        XSync(display, False);

        //Restore the original error handler
        XSetErrorHandler(oldHandler);

        if(ctxErrorOccurred || !ctx)
            die("Failed to create an OpenGL context");

        //Make it current
        glXMakeCurrent(display, win, ctx);

        //Set up input and messages
        //XAutoRepeatOff(display);
        XSelectInput(display, win, KeyPressMask | KeyReleaseMask);
        wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(display, win, &wmDeleteMessage, 1);

        //Resize hints
        XSizeHints _size_hints;
        _size_hints.flags = PSize | PMinSize | PMaxSize;
        _size_hints.min_width = WINDOW_WIDTH;
        _size_hints.max_width = WINDOW_WIDTH;
        _size_hints.min_height = WINDOW_HEIGHT;
        _size_hints.max_height = WINDOW_HEIGHT;
        XSetWMNormalHints(display, win, &_size_hints);

        //Get current fullscreen mode
        mode_original_configuration = XRRGetScreenInfo(display, RootWindow(display, 0));
        mode_original_rate = XRRConfigCurrentRate(mode_original_configuration);
        mode_original_size = XRRConfigCurrentConfiguration(mode_original_configuration, &mode_original_rotation);

        //Get the correct fullscreen mode
        int c_modes;
        XRRScreenSize* modes = XRRSizes(display, 0, &c_modes);

        for(int i = 0; i < c_modes; i++)
        {
            if(modes[i].width == WINDOW_WIDTH && modes[i].height == WINDOW_HEIGHT)
            {
                mode_index = i;

                //Get the closest framerate to 60
                int c_rates;
                short* rates = XRRRates(display, 0, i, &c_rates);

                for(int i = 0; i < c_rates; i++)
                {
                    if(abs(rates[i] - 60) < abs(mode_rate - 60))
                        mode_rate = rates[i];
                }
                break;
            }
        }

        //Create blank cursor
        Pixmap blank;
        XColor dummy;
        char data[1] = {0};

        blank = XCreateBitmapFromData(display, win, data, 1, 1);
        cursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
        XFreePixmap(display, blank);

        //Set icon
        /*
    #define icon_bitmap_width 1
    #define icon_bitmap_height 1
        static char icon_bitmap_bits[] = {
        0x60, 0x00, 0x01, 0xb0, 0x00, 0x07, 0x0c, 0x03, 0x00, 0x04, 0x04, 0x00,
        0xc2, 0x18, 0x00, 0x03, 0x30, 0x00, 0x01, 0x60, 0x00, 0xf1, 0xdf, 0x00,
        0xc1, 0xf0, 0x01, 0x82, 0x01, 0x00, 0x02, 0x03, 0x00, 0x02, 0x0c, 0x00,
        0x02, 0x38, 0x00, 0x04, 0x60, 0x00, 0x04, 0xe0, 0x00, 0x04, 0x38, 0x00,
        0x84, 0x06, 0x00, 0x14, 0x14, 0x00, 0x0c, 0x34, 0x00, 0x00, 0x00, 0x00};

        Pixmap icon_pixmap = XCreateBitmapFromData(display, win, icon_bitmap_bits, icon_bitmap_width, icon_bitmap_height);

        XWMHints wm_hints;
        wm_hints.initial_state = NormalState;
        wm_hints.input = True;
        //wm_hints.icon_pixmap = icon_pixmap;
        wm_hints.icon_window = win;
        wm_hints.flags = StateHint | IconWindowHint | InputHint;
        XSetWMHints(display, win, &wm_hints);

        Atom net_wm_icon = XInternAtom(display, "_NET_WM_ICON", False);
        Atom cardinal = XInternAtom(display, "CARDINAL", False);

        int length = 2 + 16 * 16 + 2 + 32 * 32;
        XChangeProperty(display, win, net_wm_icon, cardinal, 32, PropModeReplace, (const ubyte_t*)buffer, length);*/

        struct timeval tv;
        gettimeofday(&tv, NULL);
        epoch = (uint64_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    }

    void deinit()
    {
        setResolution(true);
        XRRFreeScreenConfigInfo(mode_original_configuration);
        XFreeCursor(display, cursor);
        XCloseDisplay(display);
    }

    unsigned long getTime()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (unsigned long)((tv.tv_sec * 1000 + tv.tv_usec / 1000) - epoch);
    }

    void setTitle(const char* title)
    {
        XStoreName(display, win, (const char*)title);
    }

    void show(ARGS_OS_SHOW_DEF)
    {
        XMapWindow(display, win);
    }

    bool getClipboardByAtom(char* b_sz_, size_t size_, const char* sz_atom_)
    {
        if(!b_sz_) return false;

        Atom clipboard = XInternAtom(display, (const char*)sz_atom_, True);
        if(clipboard == None) return true;
        XConvertSelection(display,
                          clipboard, // atom,
                          XA_STRING, // type?
                          XA_STRING, // prop,
                          win,
                          CurrentTime);

        // wait for the event
        XEvent report;
        ubyte_t *buf = 0;
        Atom type;
        int format;
        unsigned long nitems, bytes;
        for(;;)
        {
            XNextEvent(display, &report);
            if(report.type == SelectionNotify)
            {
                if (report.xselection.property == None)
                {
                    *b_sz_ = 0;
                    return true;
                }
                XGetWindowProperty(display,
                               win,
                               report.xselection.property,
                               0, // offset
                               (~0L), // length
                               False, // delete
                               AnyPropertyType, // reg_type
                               &type,// *actual_type_return,
                               &format,// *actual_format_return
                               &nitems,// *nitems_return
                               &bytes, // *bytes_after_return
                               &buf// **prop_return);
                               );
                if(!buf) return true;
                strncpy(b_sz_, (char*)buf, size_);
                return false;
            }
        }
    }

    void getClipboard(char* b_sz_, size_t size_)
    {
        if(getClipboardByAtom(b_sz_, size_, "CLIPBOARD"))
            getClipboardByAtom(b_sz_, size_, "PRIMARY");
    }

    void toggleFullscreen()
    {
        if(in_fullscreen)
        {
            setResolution(true);

            XEvent xev;
            Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
            Atom wm_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

            memset(&xev, 0, sizeof(xev));
            xev.type = ClientMessage;
            xev.xclient.window = win;
            xev.xclient.message_type = wm_state;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = 0;
            xev.xclient.data.l[1] = wm_fullscreen;
            xev.xclient.data.l[2] = 0;

            XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &xev);

            in_fullscreen = false;
        }
        else
        {
            setResolution(false);

            XEvent xev;
            Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
            Atom wm_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

            memset(&xev, 0, sizeof(xev));
            xev.type = ClientMessage;
            xev.xclient.window = win;
            xev.xclient.message_type = wm_state;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = 1;
            xev.xclient.data.l[1] = wm_fullscreen;
            xev.xclient.data.l[2] = 0;

            XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &xev);

            in_fullscreen = true;
        }
    }

    bool alt_pressed = false;

    void refresh()
    {
        glXSwapBuffers(display, win);

#ifdef GAME
        madotsuki.frameInput = 0;
        poniko.frameInput = 0;
#endif

        XEvent msg;
        XEvent next;
        XSelectInput(display, win, KeyPressMask | KeyReleaseMask);
        //while(XCheckMaskEvent(display, KeyPressMask | KeyReleaseMask, &msg))
        while(XPending(display) > 0)
        {
            XNextEvent(display, &msg);
            switch(msg.type)
            {
                case KeyPress:
                {
                    int key = XLookupKeysym(&msg.xkey, 0);

                    //Mess around with it before we send it off
                    switch(key)
                    {
                    case XK_Shift_R:
                        key = XK_Shift_L;
                        break;
                    case XK_Control_R:
                        key = XK_Control_L;
                        break;
                    case XK_Alt_L:
                    case XK_Alt_R:
                        alt_pressed = true;
                        break;
                    case XK_Return:
                        if(alt_pressed)
                            toggleFullscreen();
                        break;
                    case XK_Escape:
                        exit(0);
                    }
                    Input::keyPress(key, true);
                }
                break;

                case KeyRelease:
                {
                    //Stupid key repeat; this bypasses it.
                    //If you disable key repeat, it disables it for everything.
                    //EVERYTHING.
                    //Fuck X.
                    if(XEventsQueued(display, QueuedAfterReading))
                    {
                        XPeekEvent(display, &next);
                        if(next.type == KeyPress && next.xkey.time == msg.xkey.time && next.xkey.keycode == msg.xkey.keycode)
                        {
                            XNextEvent(display, &next);
                            break;
                        }
                    }

                    int key = XLookupKeysym(&msg.xkey, 0);

                    //Mess around with it before we send it off
                    switch(key)
                    {
                    case XK_Shift_R:
                        key = XK_Shift_L;
                        break;
                    case XK_Control_R:
                        key = XK_Control_L;
                        break;
                    case XK_Alt_L:
                    case XK_Alt_R:
                        alt_pressed = false;
                        break;
                    }
                    Input::keyPress(key, false);
                }
                break;

                case ClientMessage:
                {
                    if((unsigned long)msg.xclient.data.l[0] == wmDeleteMessage)
                        exit(0);
                }
                break;
            }
        }

#ifdef GAME
        Network::refresh();
        Input::refresh();
        Sound::refresh();
#endif
        Graphics::refresh();
        gameFrame++;
    }
}
