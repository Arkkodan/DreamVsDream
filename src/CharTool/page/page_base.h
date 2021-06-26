#ifndef CHARTOOL_PAGE_PAGE_BASE_H
#define CHARTOOL_PAGE_PAGE_BASE_H
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

#include <nlohmann/json.hpp>
#include <wx/filename.h>
#include <wx/notebook.h>

#include <string>

namespace page {
  class CharToolPage : public wxNotebookPage {
  protected:
    CharToolPage(wxNotebook *parent) : wxNotebookPage(parent, wxID_ANY) {}

  public:
    virtual void reset() = 0;
    virtual void setFromJSON(const nlohmann::ordered_json &j_obj) = 0;
    virtual nlohmann::ordered_json getJSON() const = 0;

    bool isSaved() const { return saved; }
    void setSaved(bool saved = true) { this->saved = saved; }
    wxFileName getPath() const { return path; }
    void setPath(const wxFileName &path) { this->path = path; }

  protected:
    bool saved = true;
    wxFileName path = "";
  };
} // namespace page

#endif // CHARTOOL_PAGE_PAGE_BASE_H
