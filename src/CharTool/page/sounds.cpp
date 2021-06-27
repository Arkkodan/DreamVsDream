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

#include "sounds.h"

#include "../../parser/chars/sounds.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/textdlg.h>

#include <sstream>

page::Sounds::Sounds(wxNotebook *parent)
    : CharToolPage(parent, CharToolFrame::CHARTOOL_INDEX_SOUNDS),
      addRemove(new wxAddRemoveCtrl(this)),
      dataView(new wxDataViewTreeCtrl(addRemove, wxID_ANY, wxDefaultPosition,
                                      wxDefaultSize,
                                      wxDV_NO_HEADER | wxDV_ROW_LINES)) {
  addRemove->SetAdaptor(new DataViewAddRemoveAdaptor(dataView, this));

  reset();

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(addRemove, wxSizerFlags(1).Expand());
  SetSizer(sizer);

  Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &Sounds::PreventEditing, this,
       wxID_ANY);
}

void page::Sounds::reset() { dataView->DeleteAllItems(); }

void page::Sounds::setFromJSON(const nlohmann::ordered_json &j_obj) {
  parser::SoundGroupsStruct sgs =
      parser::parseJSON<parser::SoundGroupsStruct>(j_obj);
  wxFileName soundDir = wxFileName::DirName(path.GetPath() + "/sounds");
  std::vector<std::string> missingSounds;
  for (const auto &sg : sgs) {
    wxDataViewItem sg_name =
        dataView->AppendContainer(wxDataViewItem(), sg.name);
    for (const auto &sound : sg.sounds) {
      wxFileName soundPath =
          wxFileName::FileName(soundDir.GetPath() + '/' + sound + ".wav");
      if (!soundPath.FileExists()) {
        missingSounds.emplace_back(sound);
      }
      dataView->AppendItem(sg_name, sound);
    }
    dataView->Expand(sg_name);
  }

  if (!missingSounds.empty()) {
    std::stringstream strstream;
    strstream << soundDir.GetPath() << " is missing sounds for:";
    for (const auto &sound : missingSounds) {
      strstream << '\n' << sound;
    }
    wxMessageDialog msgDialog(this, strstream.str(), "Missing Sounds",
                              wxOK | wxCENTRE | wxICON_ERROR);
    msgDialog.ShowModal();
  }
}

nlohmann::ordered_json page::Sounds::getJSON() const {
  parser::SoundGroupsStruct sgs;

  const wxDataViewItem &rootItem = dataView->GetStore()->GetRoot()->GetItem();
  int soundGroupCount = dataView->GetChildCount(rootItem);
  sgs.resize(soundGroupCount);
  for (int i = 0; i < soundGroupCount; i++) {
    const wxDataViewItem &soundGroup = dataView->GetNthChild(rootItem, i);
    parser::SoundGroup sg;
    sg.name = dataView->GetItemText(soundGroup);
    int soundCount = dataView->GetChildCount(soundGroup);
    sg.sounds.resize(soundCount);
    for (int j = 0; j < soundCount; j++) {
      const wxDataViewItem &sound = dataView->GetNthChild(soundGroup, j);
      sg.sounds[j] = dataView->GetItemText(sound);
    }
    sgs[i] = sg;
  }

  return parser::parseObject<parser::SoundGroupsStruct>(sgs);
}

void page::Sounds::PreventEditing(wxDataViewEvent &event) { event.Veto(); }

page::Sounds::DataViewAddRemoveAdaptor::DataViewAddRemoveAdaptor(
    wxDataViewTreeCtrl *dataView, Sounds *sounds_instance)
    : dataView(dataView), sounds_instance(sounds_instance) {}

wxWindow *page::Sounds::DataViewAddRemoveAdaptor::GetItemsCtrl() const {
  return dataView;
}

bool page::Sounds::DataViewAddRemoveAdaptor::CanAdd() const {
  if (!dataView->HasSelection()) {
    // Add new soundgroup
    return true;
  }
  if (dataView->IsContainer(dataView->GetSelection())) {
    // Add new sound in soundgroup
    return true;
  }
  return false;
}

bool page::Sounds::DataViewAddRemoveAdaptor::CanRemove() const {
  return dataView->HasSelection();
}

void page::Sounds::DataViewAddRemoveAdaptor::OnAdd() {
  if (!dataView->HasSelection()) {
    // Add new soundgroup
    wxTextEntryDialog textDialog(sounds_instance, "Name", "Add Sound Group");
    if (textDialog.ShowModal() == wxID_OK) {
      wxDataViewItem &container =
          dataView->AppendContainer(wxDataViewItem(), textDialog.GetValue());
      dataView->Expand(container);
      sounds_instance->markAsUnsaved();
    }
    return;
  }

  wxDataViewItem &soundgroup = dataView->GetSelection();
  if (dataView->IsContainer(soundgroup)) {
    // Add new sound in soundgroup
    wxFileName soundDir =
        wxFileName::DirName(sounds_instance->getPath().GetPath() + "/sounds");
    wxFileDialog fileDialog(
        sounds_instance, "Add Sounds to " + dataView->GetItemText(soundgroup),
        soundDir.GetPath(), wxEmptyString, "WAV files (*.wav)|*.wav",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (fileDialog.ShowModal() == wxID_OK) {
      wxArrayString arr;
      fileDialog.GetPaths(arr);
      for (const auto &path : arr) {
        wxFileName soundFile = wxFileName::FileName(path);
        dataView->AppendItem(soundgroup, soundFile.GetName());
      }
      sounds_instance->markAsUnsaved();
    }

    return;
  }
}

void page::Sounds::DataViewAddRemoveAdaptor::OnRemove() {
  dataView->DeleteItem(dataView->GetSelection());
  sounds_instance->markAsUnsaved();
}
