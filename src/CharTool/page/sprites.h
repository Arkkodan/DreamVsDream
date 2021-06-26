#ifndef CHARTOOL_PAGE_SPRITES_H
#define CHARTOOL_PAGE_SPRITES_H
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

#include "page_base.h"

#include <wx/addremovectrl.h>
#include <wx/dataview.h>

namespace page {
  class Sprites : public CharToolPage {
  public:
    Sprites(wxNotebook *parent);

    void markAsUnsaved();

  private:
    void reset() override final;
    void setFromJSON(const nlohmann::ordered_json &j_obj) override final;
    nlohmann::ordered_json getJSON() const override final;

  private:
    wxAddRemoveCtrl *addRemove;
    wxDataViewListCtrl *dataView;
  };

  class DataViewAddRemoveAdaptor : public wxAddRemoveAdaptor {
  public:
    explicit DataViewAddRemoveAdaptor(wxDataViewListCtrl *dataView,
                                      Sprites *instance);

    wxWindow *GetItemsCtrl() const override final;

    bool CanAdd() const override final;
    bool CanRemove() const override final;
    void OnAdd() override final;
    void OnRemove() override final;

  private:
    wxDataViewListCtrl *dataView;

    Sprites *sprites_instance;
  };
} // namespace page

#endif // CHARTOOL_PAGE_SPRITES_H
