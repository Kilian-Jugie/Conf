#pragma once
#include "conffile.hpp"

namespace conflib {
  // An item is the unit manipulated by conf softwares
  // to describe any sort of objects in code (scopes,
  // variables etc)
  class ConfItem {
    ConfFile& m_File;
    uint64_t m_Line;
    string_t m_Name;

  public:
    ConfItem() = default;
    ConfItem(const ConfItem&) = default; 
    ConfItem(ConfItem&&) = default;

    const string_t& GetName() const {
      return m_Name;
    }

    void SetName(string_t name) {
      m_Name = std::move(name);
    }

    uint64_t GetLine() const {
      return m_Line;
    }

    void SetLine(uint64_t line) {
      m_Line = line;
    }

    const ConfFile& GetFile() const {
      return m_File;
    }

    void SetFile(const ConfFile& file) {
      m_File = file;
    }
  };
}