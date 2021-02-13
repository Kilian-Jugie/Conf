#include "compilerdiagnostic.hpp"

namespace confcomp {
  std::vector<std::pair<string_t, string_t>> CompilerDiagnostic::m_Messages = {};
  bool CompilerDiagnostic::m_IsInitialized = false;

  void CompilerDiagnostic::RegisterMessage(string_t code, string_t message) {
    m_Messages.push_back(std::make_pair<string_t, string_t>(std::move(code), std::move(message)));
  }

  const string_t& CompilerDiagnostic::GetMessageByCode(const string_t& code) {
    auto fit = std::find_if(m_Messages.cbegin(), m_Messages.cend(),
      [code](const std::pair<string_t, string_t>& it) {
        return it.first == code;
      });
    if(fit != m_Messages.cend())
      return fit->second;
    else
      return m_Messages[0].second;
  }

  void CompilerDiagnostic::RegisterStdMessages() {
    RegisterMessage("C1000", "Internal compiler error at %?:%?\nDetails: %?");
    RegisterMessage("C1001", "File %? not found");

    RegisterMessage("C2001", "Not enough arguments in preprocessor directive '%?': expected %? got %?");

    RegisterMessage("I1001", "File %? already included");

    m_IsInitialized = true;
  }


}