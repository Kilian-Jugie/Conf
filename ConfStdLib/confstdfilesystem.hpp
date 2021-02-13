#pragma once
#include <filesystem>
#include "confstdglobal.hpp"

namespace confstd {
  template<typename _T>concept PathConstructible = requires(_T v) {
    { std::filesystem::path{v} };
  };
}