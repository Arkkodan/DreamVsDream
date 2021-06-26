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

#include "sprites.h"

#include "../frame.h"

#include "../../parser/chars/sprites.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

#include <sstream>

page::Sprites::Sprites(wxNotebook *parent)
    : CharToolPage(parent), addRemove(new wxAddRemoveCtrl(this)),
      dataView(new wxDataViewListCtrl(
          addRemove, wxID_ANY, wxDefaultPosition, wxDefaultSize,
          wxDV_MULTIPLE | wxDV_HORIZ_RULES | wxDV_ROW_LINES |
              wxDV_VARIABLE_LINE_HEIGHT)) {
  dataView->ClearColumns();
  dataView->AppendTextColumn("Name");
  dataView->AppendBitmapColumn("Sprite", 1);

  addRemove->SetAdaptor(new DataViewAddRemoveAdaptor(dataView, this));

  reset();

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(addRemove, wxSizerFlags(1).Expand());

  SetSizer(sizer);
}

void page::Sprites::reset() { dataView->DeleteAllItems(); }

void page::Sprites::setFromJSON(const nlohmann::ordered_json &j_obj) {
  parser::SpritesStruct ss = parser::parseJSON<parser::SpritesStruct>(j_obj);
  wxFileName imageDir = wxFileName::DirName(path.GetPath() + "/sprites");
  std::vector<std::string> missingImages;
  for (const auto &sprite : ss) {
    wxVector<wxVariant> v;
    wxFileName imagePath =
        wxFileName::FileName(imageDir.GetPath() + '/' + sprite + ".png");
    wxBitmap bitmap;
    if (imagePath.FileExists()) {
      bitmap.LoadFile(imagePath.GetFullPath(), wxBITMAP_TYPE_PNG);
    }
    else {
      missingImages.emplace_back(sprite);
    }
    v.push_back(sprite);
    v.push_back(static_cast<wxVariant>(bitmap));
    dataView->AppendItem(v);
  }

  if (!missingImages.empty()) {
    std::stringstream strstream;
    strstream << imageDir.GetPath() << " is missing images for:";
    for (const auto &image : missingImages) {
      strstream << '\n' << image;
    }
    wxMessageDialog msgDialog(this, strstream.str(), "Missing Sprites",
                              wxOK | wxCENTRE | wxICON_ERROR);
    msgDialog.ShowModal();
  }
}

nlohmann::ordered_json page::Sprites::getJSON() const {
  parser::SpritesStruct ss;
  for (int i = 0, size = dataView->GetItemCount(); i < size; i++) {
    wxVariant var;
    dataView->GetValue(var, i, 0);
    ss.emplace_back(var.GetString().ToStdString());
  }

  return parser::parseObject<parser::SpritesStruct>(ss);
}

void page::Sprites::markAsUnsaved() {
  if (saved) {
    saved = false;
    auto *parent = dynamic_cast<wxNotebook *>(GetParent());
    if (parent) {
      parent->SetPageText(CharToolFrame::CHARTOOL_INDEX_SPRITES,
                          CharToolFrame::NOTEBOOK_TAB_LABELS
                                  [CharToolFrame::CHARTOOL_INDEX_SPRITES] +
                              '*');
    }
  }
}

page::DataViewAddRemoveAdaptor::DataViewAddRemoveAdaptor(
    wxDataViewListCtrl *dataView, Sprites *instance)
    : dataView(dataView), sprites_instance(instance) {}

wxWindow *page::DataViewAddRemoveAdaptor::GetItemsCtrl() const {
  return dataView;
}

bool page::DataViewAddRemoveAdaptor::CanAdd() const { return true; }

bool page::DataViewAddRemoveAdaptor::CanRemove() const {
  return dataView->GetSelectedItemsCount() != 0;
}

void page::DataViewAddRemoveAdaptor::OnAdd() {
  wxFileName imageDir =
      wxFileName::DirName(sprites_instance->getPath().GetPath() + "/sprites");
  wxFileDialog fileDialog(sprites_instance, "Add Sprites", imageDir.GetPath(),
                          wxEmptyString, "PNG files (*.png)|*.png",
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
  if (fileDialog.ShowModal() == wxID_OK) {
    wxArrayString arr;
    fileDialog.GetPaths(arr);
    for (const auto &path : arr) {
      wxFileName imageFile = wxFileName::FileName(path);
      wxVector<wxVariant> v;
      wxBitmap bitmap;
      bitmap.LoadFile(imageFile.GetFullPath(), wxBITMAP_TYPE_PNG);
      v.push_back(imageFile.GetName());
      v.push_back(static_cast<wxVariant>(bitmap));
      dataView->AppendItem(v);
    }

    sprites_instance->markAsUnsaved();
  }
}

void page::DataViewAddRemoveAdaptor::OnRemove() {
  wxDataViewItemArray arr;
  dataView->GetSelections(arr);
  for (const auto &item : arr) {
    dataView->DeleteItem(dataView->ItemToRow(item));
  }
  sprites_instance->markAsUnsaved();
}
