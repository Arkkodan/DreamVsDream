// MIT License
//
// Copyright (c) 2021 Arkkodan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "frame.h"

#include "app.h"

#include <wx/aboutdlg.h>
#include <wx/menu.h>

CharToolFrame::CharToolFrame()
    : wxFrame(nullptr, wxID_ANY, CharToolApp::TITLE) {
  wxMenu *menuFile = new wxMenu();
  menuFile->Append(wxID_EXIT);

  wxMenu *menuHelp = new wxMenu();
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar();
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);

  Bind(wxEVT_MENU, &CharToolFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_MENU, &CharToolFrame::OnAbout, this, wxID_ABOUT);
}

void CharToolFrame::OnExit(wxCommandEvent &event) { Close(true); }

void CharToolFrame::OnAbout(wxCommandEvent &event) {
  wxAboutDialogInfo info;
  info.SetName(CharToolApp::TITLE);
  info.SetVersion(DVD_VERSION_STRING);
  info.SetDescription(
      "This program creates characters for Dream vs. Dream. It is meant to "
      "replace the older character tools: Atlas, Sprtool, and Compiler.");
  info.AddDeveloper("Arkkodan");

  wxAboutBox(info);
}
