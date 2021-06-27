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
#include "page/general.h"
#include "page/sounds.h"
#include "page/sprites.h"
#include "page/voices.h"

#include "../fileIO/json.h"

#include <wx/aboutdlg.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/textfile.h>

const std::array<std::string, CharToolFrame::CHARTOOL_INDEX_MAX>
    CharToolFrame::NOTEBOOK_TAB_LABELS = {"General", "Sprites", "Sounds",
                                          "Voices"};

const std::array<std::string, CharToolFrame::CHARTOOL_INDEX_MAX>
    CharToolFrame::PAGE_FILE_NAMES = {"general", "sprites", "sounds", "voices"};

CharToolFrame::CharToolFrame()
    : wxFrame(nullptr, wxID_ANY, CharToolApp::TITLE, wxDefaultPosition,
              wxSize(640, 480)),
      path(), loadArray(), menuFile(new wxMenu),
      notebook(new wxNotebook(this, wxID_ANY)) {

  menuFile->Append(wxID_NEW);
  menuFile->Append(wxID_OPEN);
  menuFile->Append(wxID_CLOSE);
  menuFile->Enable(wxID_CLOSE, false);
  menuFile->Append(wxID_SAVE);
  menuFile->Enable(wxID_SAVE, false);
  menuFile->Append(wxID_EXIT);

  wxMenu *menuHelp = new wxMenu;
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");
  SetMenuBar(menuBar);

  CreateStatusBar();

  notebook->Hide();
  notebook->AddPage(new page::General(notebook),
                    NOTEBOOK_TAB_LABELS[CHARTOOL_INDEX_GENERAL]);
  notebook->AddPage(new page::Sprites(notebook),
                    NOTEBOOK_TAB_LABELS[CHARTOOL_INDEX_SPRITES]);
  notebook->AddPage(new page::Sounds(notebook),
                    NOTEBOOK_TAB_LABELS[CHARTOOL_INDEX_SOUNDS]);
  notebook->AddPage(new page::Voices(notebook),
                    NOTEBOOK_TAB_LABELS[CHARTOOL_INDEX_VOICES]);
  resetPages();

  Layout();

  wxImage::AddHandler(new wxPNGHandler);

  Bind(wxEVT_MENU, &CharToolFrame::OnNew, this, wxID_NEW);
  Bind(wxEVT_MENU, &CharToolFrame::OnOpen, this, wxID_OPEN);
  Bind(wxEVT_MENU, &CharToolFrame::OnClose, this, wxID_CLOSE);
  Bind(wxEVT_MENU, &CharToolFrame::OnSaveAll, this, wxID_SAVE);
  Bind(wxEVT_MENU, &CharToolFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_MENU, &CharToolFrame::OnAbout, this, wxID_ABOUT);

  Bind(wxEVT_CLOSE_WINDOW, &CharToolFrame::OnCloseWindow, this);
}

void CharToolFrame::OnNew(const wxCommandEvent &event) {
  if (!obtainClosePermission()) {
    return;
  }

  wxDirDialog dirDialog(this, "Select New Fighter Directory", wxEmptyString,
                        wxDD_DEFAULT_STYLE);

  bool success = false;

  do {
    if (dirDialog.ShowModal() == wxID_OK) {
      // Selected directory
      wxFileName newpath = wxFileName::DirName(dirDialog.GetPath());

      loadArray.fill(false);
      for (size_t i = 0; i < CHARTOOL_INDEX_MAX; i++) {
        wxFileName filepath = wxFileName::FileName(
            newpath.GetPath() + '/' + PAGE_FILE_NAMES[i] + ".json");
        if (filepath.FileExists()) {
          wxMessageDialog msgDialog(
              this,
              "Directory already contains fighter info. Initialize anyway?",
              "Fighter Directory Exists",
              wxYES_NO | wxCENTRE | wxICON_QUESTION);
          int modalResponse = msgDialog.ShowModal();
          if (modalResponse == wxID_NO) {
            return;
          }
          break;
        }
      }

      path = newpath;

      notebook->Show();
      Layout();
      menuFile->Enable(wxID_CLOSE, true);
      menuFile->Enable(wxID_SAVE, true);
      SetStatusText("Path: \"" + path.GetPath() + "\" was initialized.");
      success = true;

      // Mark all as unsaved
      for (size_t i = 0, size = notebook->GetPageCount(); i < size; i++) {
        page::CharToolPage *page =
            dynamic_cast<page::CharToolPage *>(notebook->GetPage(i));
        if (page && page->isSaved()) {
          page->setSaved(false);
          notebook->SetPageText(i, NOTEBOOK_TAB_LABELS[i] + '*');
        }
      }
    }
    else {
      // No selected directory
      break;
    }
  } while (!success);
}

void CharToolFrame::OnOpen(const wxCommandEvent &event) {
  if (!obtainClosePermission()) {
    return;
  }

  wxDirDialog dirDialog(this, "Select Existing Fighter Directory",
                        wxEmptyString,
                        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

  bool success = false;

  do {
    if (dirDialog.ShowModal() == wxID_OK) {
      // Selected directory
      path = wxFileName::DirName(dirDialog.GetPath());

      loadArray.fill(false);
      for (size_t i = 0; i < CHARTOOL_INDEX_MAX; i++) {
        wxFileName filepath = wxFileName::FileName(
            path.GetPath() + '/' + PAGE_FILE_NAMES[i] + ".json");
        if (filepath.FileExists()) {
          loadArray[i] = true;
        }
      }

      if (std::any_of(loadArray.cbegin(), loadArray.cend(),
                      [](const bool &load) { return load; })) {
        resetPages();

        for (size_t i = 0; i < CHARTOOL_INDEX_MAX; i++) {
          page::CharToolPage *currentPage =
              dynamic_cast<page::CharToolPage *>(notebook->GetPage(i));
          if (currentPage) {
            currentPage->setPath(path);
            wxFileName filepath = wxFileName::FileName(
                path.GetPath() + '/' + PAGE_FILE_NAMES[i] + ".json");
            if (filepath.FileExists()) {
              currentPage->setFromJSON(
                  fileIO::readJSON(filepath.GetFullPath().ToStdString()));
            }
          }
        }

        notebook->Show();
        Layout();
        menuFile->Enable(wxID_CLOSE, true);
        menuFile->Enable(wxID_SAVE, true);
        SetStatusText("Path: \"" + path.GetPath() + "\" was loaded.");
        success = true;
        markAllAsSaved();

        // Mark new as unsaved
        for (size_t i = 0, size = notebook->GetPageCount(); i < size; i++) {
          if (!loadArray[i]) {
            page::CharToolPage *page =
                dynamic_cast<page::CharToolPage *>(notebook->GetPage(i));
            if (page && page->isSaved()) {
              page->setSaved(false);
              notebook->SetPageText(i, NOTEBOOK_TAB_LABELS[i] + '*');
            }
          }
        }
      }
      else {
        wxMessageDialog msgDialog(
            this, "Directory does not contain any fighter info.",
            "Invalid Fighter Directory", wxOK | wxCENTRE | wxICON_ERROR);
        msgDialog.ShowModal();
      }
    }
    else {
      // No selected directory
      break;
    }
  } while (!success);
}

void CharToolFrame::OnClose(const wxCommandEvent &event) {
  if (obtainClosePermission()) {
    menuFile->Enable(wxID_CLOSE, false);
    menuFile->Enable(wxID_SAVE, false);
    notebook->Hide();
    path.Clear();
    markAllAsSaved();
  }
}

void CharToolFrame::OnSaveAll(const wxCommandEvent &event) { saveAll(); }

void CharToolFrame::OnExit(const wxCommandEvent &event) {
  if (obtainClosePermission()) {
    Close(true);
    markAllAsSaved();
  }
}
void CharToolFrame::OnCloseWindow(const wxCloseEvent &event) {
  if (!event.CanVeto() || obtainClosePermission()) {
    Destroy();
  }
}

void CharToolFrame::OnAbout(const wxCommandEvent &event) {
  wxAboutDialogInfo info;
  info.SetName(CharToolApp::TITLE);
  info.SetVersion(DVD_VERSION_STRING);
  info.SetDescription(
      "This program creates characters for Dream vs. Dream. It is meant to "
      "replace the older character tools: Atlas, Sprtool, and Compiler.\nThis "
      "tool is useful for creating a character from a starting point. It is "
      "not perfect, so you may want to manually edit files as well.");
  info.AddDeveloper("Arkkodan");

  wxAboutBox(info);
}

void CharToolFrame::resetPages() {
  SetStatusText("No fighter is loaded.");

  for (size_t i = 0, size = notebook->GetPageCount(); i < size; i++) {
    page::CharToolPage *page =
        dynamic_cast<page::CharToolPage *>(notebook->GetPage(i));
    if (page) {
      page->reset();
    }
  }
}

bool CharToolFrame::isSaved() const {
  for (size_t i = 0, size = notebook->GetPageCount(); i < size; i++) {
    page::CharToolPage *page =
        dynamic_cast<page::CharToolPage *>(notebook->GetPage(i));
    if (page && !page->isSaved()) {
      return false;
    }
  }
  return true;
}

bool CharToolFrame::obtainClosePermission() {
  if (isSaved()) {
    return true;
  }
  else {
    wxMessageDialog msgDialog(
        this, "This fighter has unsaved changes. Do you want to save changes?",
        "Unsaved Changes", wxYES_NO | wxCANCEL | wxCENTER);
    switch (msgDialog.ShowModal()) {
    case wxID_YES:
      saveAll();
      // Fallthrough
    case wxID_NO:
      return true;
    }

    return false;
  }
}

void CharToolFrame::saveAll() {
  // Save all
  for (size_t i = 0, size = notebook->GetPageCount(); i < size; i++) {
    page::CharToolPage *page =
        dynamic_cast<page::CharToolPage *>(notebook->GetPage(i));
    if (page && !page->isSaved()) {
      fileIO::writeTextToFile(path.GetPath().ToStdString() + '/' +
                                  PAGE_FILE_NAMES[i] + ".json",
                              page->getJSON().dump(4));
      page->setSaved();
      notebook->SetPageText(i, NOTEBOOK_TAB_LABELS[i]);
    }
  }

  SetStatusText("Path: \"" + path.GetPath() + "\" was saved.");

  markAllAsSaved();
}

void CharToolFrame::markAllAsSaved() {
  for (size_t i = 0, size = notebook->GetPageCount(); i < size; i++) {
    page::CharToolPage *page =
        dynamic_cast<page::CharToolPage *>(notebook->GetPage(i));
    if (page && !page->isSaved()) {
      page->setSaved();
      notebook->SetPageText(i, NOTEBOOK_TAB_LABELS[i]);
    }
  }
}
