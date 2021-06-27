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

#include "voices.h"

#include "../../parser/chars/voices.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/wizard.h>

#include <sstream>

page::Voices::Voices(wxNotebook *parent)
    : CharToolPage(parent, CharToolFrame::CHARTOOL_INDEX_VOICES),
      addRemove(new wxAddRemoveCtrl(this)),
      dataView(new wxDataViewTreeCtrl(addRemove, wxID_ANY, wxDefaultPosition,
                                      wxDefaultSize,
                                      wxDV_NO_HEADER | wxDV_ROW_LINES)) {
  addRemove->SetAdaptor(new DataViewAddRemoveAdaptor(dataView, this));
  addRemove->Layout();

  reset();

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(addRemove, wxSizerFlags(1).Expand());
  SetSizer(sizer);

  Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &Voices::PreventEditing, this,
       wxID_ANY);
}

void page::Voices::reset() { dataView->DeleteAllItems(); }

void page::Voices::setFromJSON(const nlohmann::ordered_json &j_obj) {
  parser::VoiceGroupsStruct vgs =
      parser::parseJSON<parser::VoiceGroupsStruct>(j_obj);
  wxFileName voiceDir = wxFileName::DirName(path.GetPath() + "/voices");
  std::vector<std::string> missingSounds;
  for (const auto &vg : vgs) {
    wxDataViewItem vg_name = dataView->AppendContainer(
        wxDataViewItem(), dataToDisplay(vg.name, vg.percent));
    for (const auto &sound : vg.sounds) {
      wxFileName soundPath =
          wxFileName::FileName(voiceDir.GetPath() + '/' + sound + ".wav");
      if (!soundPath.FileExists()) {
        missingSounds.emplace_back(sound);
      }
      dataView->AppendItem(vg_name, sound);
    }
    dataView->Expand(vg_name);
  }

  if (!missingSounds.empty()) {
    std::stringstream strstream;
    strstream << voiceDir.GetPath() << " is missing sounds for:";
    for (const auto &sound : missingSounds) {
      strstream << '\n' << sound;
    }
    wxMessageDialog msgDialog(this, strstream.str(), "Missing Voices",
                              wxOK | wxCENTRE | wxICON_ERROR);
    msgDialog.ShowModal();
  }
}

nlohmann::ordered_json page::Voices::getJSON() const {
  parser::VoiceGroupsStruct vgs;

  const wxDataViewItem &rootItem = dataView->GetStore()->GetRoot()->GetItem();
  int voiceGroupCount = dataView->GetChildCount(rootItem);
  vgs.resize(voiceGroupCount);
  for (int i = 0; i < voiceGroupCount; i++) {
    const wxDataViewItem &voiceGroup = dataView->GetNthChild(rootItem, i);
    parser::VoiceGroup vg;
    std::string display = dataView->GetItemText(voiceGroup);
    displayToData(display, vg.name, vg.percent);
    int soundCount = dataView->GetChildCount(voiceGroup);
    vg.sounds.resize(soundCount);
    for (int j = 0; j < soundCount; j++) {
      const wxDataViewItem &sound = dataView->GetNthChild(voiceGroup, j);
      vg.sounds[j] = dataView->GetItemText(sound);
    }
    vgs[i] = vg;
  }

  return parser::parseObject<parser::VoiceGroupsStruct>(vgs);
}

void page::Voices::PreventEditing(wxDataViewEvent &event) { event.Veto(); }

std::string page::Voices::dataToDisplay(const std::string &name, int percent) {
  std::stringstream ss;
  ss << name << " [" << percent << "%]";
  return ss.str();
}
bool page::Voices::displayToData(const std::string &display, std::string &name,
                                 int &percent) {
  size_t delimitBegin = display.rfind(" [");
  size_t delimitEnd = display.rfind("%]");
  if (delimitBegin == std::string::npos || delimitEnd == std::string::npos ||
      delimitBegin >= delimitEnd) {
    return false;
  }

  name = display.substr(0, delimitBegin);
  percent = static_cast<int>(std::strtol(
      display.substr(delimitBegin + 2, delimitEnd - delimitBegin - 2).c_str(),
      nullptr, 10));
}

page::Voices::DataViewAddRemoveAdaptor::DataViewAddRemoveAdaptor(
    wxDataViewTreeCtrl *dataView, Voices *voices_instance)
    : dataView(dataView), voices_instance(voices_instance) {}

wxWindow *page::Voices::DataViewAddRemoveAdaptor::GetItemsCtrl() const {
  return dataView;
}

bool page::Voices::DataViewAddRemoveAdaptor::CanAdd() const {
  if (!dataView->HasSelection()) {
    // Add new voicegroup
    return true;
  }
  if (dataView->IsContainer(dataView->GetSelection())) {
    // Add new sound in voicegroup
    return true;
  }
  return false;
}

bool page::Voices::DataViewAddRemoveAdaptor::CanRemove() const {
  return dataView->HasSelection();
}

void page::Voices::DataViewAddRemoveAdaptor::OnAdd() {
  if (!dataView->HasSelection()) {
    // Add new voicegroup
    wxWizard wizard(voices_instance, wxID_ANY, "Add Voice Group");
    wxWizardPageSimple wizardPage(&wizard);
    wxStaticText nameText(&wizardPage, wxID_ANY, "Name");
    wxTextCtrl nameCtrl(&wizardPage, wxID_ANY);
    wxStaticText percentText(&wizardPage, wxID_ANY, "Percent");
    wxSpinCtrl percentCtrl(&wizardPage, wxID_ANY, wxEmptyString,
                           wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0,
                           100, 100);
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(&nameText, wxSizerFlags(0).Expand());
    sizer->Add(&nameCtrl, wxSizerFlags(0).Expand());
    sizer->Add(&percentText, wxSizerFlags(0).Expand());
    sizer->Add(&percentCtrl, wxSizerFlags(0).Expand());
    wizardPage.SetSizer(sizer);
    if (wizard.RunWizard(&wizardPage)) {
      wxDataViewItem &container = dataView->AppendContainer(
          wxDataViewItem(),
          Voices::dataToDisplay(nameCtrl.GetValue().ToStdString(),
                                percentCtrl.GetValue()));
      dataView->Expand(container);
      voices_instance->markAsUnsaved();
    }
    return;
  }

  wxDataViewItem &voicegroup = dataView->GetSelection();
  if (dataView->IsContainer(voicegroup)) {
    // Add new sound in voicegroup
    wxFileName soundDir =
        wxFileName::DirName(voices_instance->getPath().GetPath() + "/voices");
    wxFileDialog fileDialog(
        voices_instance, "Add Sounds to " + dataView->GetItemText(voicegroup),
        soundDir.GetPath(), wxEmptyString, "WAV files (*.wav)|*.wav",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (fileDialog.ShowModal() == wxID_OK) {
      wxArrayString arr;
      fileDialog.GetPaths(arr);
      for (const auto &path : arr) {
        wxFileName soundFile = wxFileName::FileName(path);
        dataView->AppendItem(voicegroup, soundFile.GetName());
      }
      voices_instance->markAsUnsaved();
    }

    return;
  }
}

void page::Voices::DataViewAddRemoveAdaptor::OnRemove() {
  dataView->DeleteItem(dataView->GetSelection());
  voices_instance->markAsUnsaved();
}
