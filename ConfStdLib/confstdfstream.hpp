#pragma once
#include "confstdglobal.hpp"
#include <fstream>

namespace confstd {
  using ifstream_t = std::basic_ifstream<char_t>;
  using ofstream_t = std::basic_ofstream<char_t>;
  using fstream_t = std::basic_fstream<char_t>;
  using istream_t = std::basic_istream<char_t>;
  using ostream_t = std::basic_ostream<char_t>;
}