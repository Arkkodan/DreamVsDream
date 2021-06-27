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

#include "general.h"

#include "../../parser/chars/general.h"

#include <wx/sizer.h>
#include <wx/valnum.h>

page::General::General(wxNotebook *parent)
    : CharToolPage(parent, CharToolFrame::CHARTOOL_INDEX_GENERAL),
      propGrid(
          new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                             wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER)) {
  propGrid->Append(new wxStringProperty("Display Name", "displayName"));

  wxArrayString groupEnum;
  wxArrayInt groupInt;
  for (size_t i = 0; i < parser::GROUP_MAX; i++) {
    groupEnum.Add(parser::GROUP_ENUM_STRINGS[i]);
    groupInt.Add(i);
  }

  propGrid->Append(new wxEnumProperty("Group", "group", groupEnum, groupInt));

  propGrid->Append(new wxUIntProperty("HP", "hp"));

  wxPGProperty *collisionProp = propGrid->Append(
      new wxStringProperty("Collision Box", "collisionBox", "<composed>"));
  propGrid->AppendIn(collisionProp, new wxUIntProperty("Width", "width"));
  propGrid->AppendIn(collisionProp, new wxUIntProperty("Height", "height"));

  propGrid->Append(new wxFloatProperty("Gravity", "gravity"));
  propGrid->Append(new wxUIntProperty("Palette Count", "paletteCount"));

  reset();

  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(propGrid, wxSizerFlags(1).Expand());

  SetSizer(sizer);

  Bind(wxEVT_PG_CHANGING, &General::OnPGChanging, this);
}

void page::General::OnPGChanging(wxPropertyGridEvent &event) {
  markAsUnsaved();

  const wxVariant value = event.GetValue();
  if (value.IsNull()) {
    return;
  }

  const wxPGProperty *prop = event.GetProperty();
  const wxString name = prop->GetName();

  if (name == "displayName") {
    wxString vstr = value.GetString();
    if (vstr.empty()) {
      event.Veto();
      event.SetValidationFailureMessage(name + " must not be empty");
      event.SetValidationFailureBehavior(wxPG_VFB_DEFAULT);
    }
  }
  else if (name == "hp" || name == "collisionBox.width" ||
           name == "collisionBox.height" || name == "paletteCount") {
    long vint = value.GetInteger();
    if (vint <= 0) {
      event.Veto();
      event.SetValidationFailureMessage(name + " must be greater than 0");
      event.SetValidationFailureBehavior(wxPG_VFB_DEFAULT);
    }
  }
  else if (name == "gravity") {
    double vdouble = value.GetDouble();
    if (vdouble <= 0.0) {
      event.Veto();
      event.SetValidationFailureMessage(name + " must be greater than 0.0");
      event.SetValidationFailureBehavior(wxPG_VFB_DEFAULT);
    }
  }
}

void page::General::reset() {
  propGrid->SetPropertyValue("displayName", "null");
  propGrid->SetPropertyValue("group", "Other");
  propGrid->SetPropertyValue("hp", 1000);
  propGrid->SetPropertyValue("collisionBox.width", 30);
  propGrid->SetPropertyValue("collisionBox.height", 100);
  propGrid->SetPropertyValue("gravity", 1.0);
  propGrid->SetPropertyValue("paletteCount", 1);
}

void page::General::setFromJSON(const nlohmann::ordered_json &j_obj) {
  parser::GeneralStruct gs = parser::parseJSON<parser::GeneralStruct>(j_obj);
  propGrid->SetPropertyValue("displayName", wxString(gs.displayName));

  propGrid->SetPropertyValue("group", gs.group);
  propGrid->SetPropertyValue("hp", gs.hp);
  propGrid->SetPropertyValue("collisionBox.width", gs.collisionBox.width);
  propGrid->SetPropertyValue("collisionBox.height", gs.collisionBox.height);
  propGrid->SetPropertyValue("gravity", gs.gravity);
  propGrid->SetPropertyValue("paletteCount", gs.paletteCount);
}

nlohmann::ordered_json page::General::getJSON() const {
  parser::GeneralStruct gs;
  gs.displayName = propGrid->GetPropertyValueAsString("displayName");
  gs.group = propGrid->GetPropertyValueAsInt("group");
  gs.hp = propGrid->GetPropertyValueAsInt("hp");
  gs.collisionBox.width = propGrid->GetPropertyValueAsInt("collisionBox.width");
  gs.collisionBox.height =
      propGrid->GetPropertyValueAsInt("collisionBox.height");
  gs.gravity = propGrid->GetPropertyValueAsDouble("gravity");
  gs.paletteCount = propGrid->GetPropertyValueAsInt("paletteCount");
  return parser::parseObject<parser::GeneralStruct>(gs);
}
