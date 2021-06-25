#ifndef CHARTOOL_PAGE_PAGE_BASE_H
#define CHARTOOL_PAGE_PAGE_BASE_H

#include <nlohmann/json.hpp>

namespace page {
  class CharToolPage {
  public:
    virtual void reset() = 0;
    virtual void setFromJSON(const nlohmann::ordered_json &j_obj) = 0;
    virtual nlohmann::ordered_json getJSON() const = 0;

    bool isSaved() const { return saved; }
    void setSaved(bool saved = true) { this->saved = saved; }

  protected:
    bool saved = true;
  };
} // namespace page

#endif // CHARTOOL_PAGE_PAGE_BASE_H
