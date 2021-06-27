#ifndef CHARTOOL_FRAME_H
#define CHARTOOL_FRAME_H
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

#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/notebook.h>

#include <array>
#include <string>

class CharToolFrame : public wxFrame {
public:
  CharToolFrame();

public:
  enum {
    CHARTOOL_INDEX_GENERAL,
    CHARTOOL_INDEX_SPRITES,
    CHARTOOL_INDEX_SOUNDS,
    CHARTOOL_INDEX_VOICES,

    CHARTOOL_INDEX_MAX
  };

  static const std::array<std::string, CHARTOOL_INDEX_MAX> NOTEBOOK_TAB_LABELS;
  static const std::array<std::string, CHARTOOL_INDEX_MAX> PAGE_FILE_NAMES;

private:
  void OnNew(const wxCommandEvent &event);
  void OnOpen(const wxCommandEvent &event);
  void OnClose(const wxCommandEvent &event);
  void OnSaveAll(const wxCommandEvent &event);
  void OnExit(const wxCommandEvent &event);
  void OnCloseWindow(const wxCloseEvent &event);
  void OnAbout(const wxCommandEvent &event);

  void resetPages();
  bool isSaved() const;
  bool obtainClosePermission();
  void saveAll();
  void markAllAsSaved();

private:
  wxFileName path;

  std::array<bool, CHARTOOL_INDEX_MAX> loadArray;

  wxMenu *menuFile;

  wxNotebook *notebook;
};

#endif // CHARTOOL_FRAME_H
