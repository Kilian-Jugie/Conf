#include "compilersettings.hpp"

namespace confcomp {
  void CompilerSettings::SetDebugLevel(DebugLevel lvl) noexcept {
    m_DebugLevel = lvl;
  }
  CompilerSettings::DebugLevel CompilerSettings::GetDebugLevel() const noexcept {
    return m_DebugLevel;
  }
  void CompilerSettings::DisableWarning(string_t warn) {
    m_DisabledWarnings.push_back(warn);
  }
  void CompilerSettings::EnableWarning(string_t warn) {
    const auto it = std::find(m_DisabledWarnings.cbegin(), m_DisabledWarnings.cend(), warn);
    if(it != m_DisabledWarnings.cend())
      m_DisabledWarnings.erase(it);
  }
  void CompilerSettings::SetWarningsAsErrors(bool v) noexcept {
    m_WarningsAsErrors = v;
  }
  bool CompilerSettings::IsWarningsAsErrors() const noexcept {
    return m_WarningsAsErrors;
  }
  bool CompilerSettings::IsWarningEnable(string_t warn) const noexcept {
    return std::find(m_DisabledWarnings.cbegin(), m_DisabledWarnings.cend(), warn) == m_DisabledWarnings.cend();
  }
}