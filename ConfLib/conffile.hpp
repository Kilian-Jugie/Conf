#pragma once
#include "conflibglobal.hpp"
#include <confstdconcepts.hpp>
#include <filesystem>

namespace conflib {
  class ConfFile {
    std::filesystem::path m_Path;
    uint64_t m_LineCount;

  public:
    template<confstd::concepts::StdStringCompatible _StrTy> ConfFile(_StrTy&& fname) : m_Path{std::forward<_StrTy>(fname)}, m_LineCount{0U} {}
    //TODO: perfect forward with path concept
    ConfFile(const std::filesystem::path& path) : m_Path{path}, m_LineCount{0U} {}

    const std::filesystem::path& GetPath() const noexcept {
      return m_Path;
    }
  };
}