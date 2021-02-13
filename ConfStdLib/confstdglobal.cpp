#include "confstdglobal.hpp"
#include <codecvt>

std::string confstd::wstr_str(const std::wstring& source) {
  return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(source);
}

std::wstring confstd::str_wtr(const std::string& source) {
  return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(source);
}
