#pragma once
#include "confcompglobal.hpp"
#include <confstdsstream.hpp>
#include <filesystem>
#include <concepts>

namespace confcomp {
  class CompilerDiagnostic {
  public:
    static constexpr char_t CONF_COMP_ERR_FORMAT_CHAR = '%';
    static constexpr char_t CONF_COMP_ERR_FORMAT_ANY = '?';

    static constexpr const char_t* CONF_COMP_ERR_INTERNAL_CODE = "C1000";

    enum class DiagnosticType {
      DEBUG,
      ERROR,
      WARNING,
      INFO
    };

    template<typename... _Args>CompilerDiagnostic(uint32_t line, std::filesystem::path file,
      string_t code, DiagnosticType type, _Args... format) : m_Line{line}, m_File{std::move(file)},
      m_Code{std::move(code)}, m_Type{type} {
      if(!m_IsInitialized) RegisterStdMessages();
      m_Msg = GetMessageByCode(m_Code);
      (FormatMessageV(m_Msg, format), ...);
    }
      
    static void RegisterMessage(string_t code, string_t message);

    const string_t& GetMessage() const noexcept {
      return m_Msg;
    }
    
    uint32_t GetLine() const noexcept {
      return m_Line;
    }

    const string_t& GetCode() const noexcept {
      return m_Code;
    }

    const std::filesystem::path& GetFile() const noexcept {
      return m_File;
    }

    DiagnosticType GetType() const noexcept {
      return m_Type;
    }

  private:
    template<typename... _Args>static void FormatMessage(string_t& msg, _Args... format) {
      (FormatMessageV(msg, format), ...);
    }

#pragma warning(push)
#pragma warning(disable: 4244) // Conversion from '_T' to 'const _Elem', possible loss of data
    // TODO: Find a better replacement format !
    template<typename _T>static void FormatMessageV(string_t& msg, _T val) {
      auto it = std::find(msg.begin(), msg.end(), CONF_COMP_ERR_FORMAT_CHAR);
      if(it != msg.end()) {
        if(*++it != CONF_COMP_ERR_FORMAT_ANY) {
          confstd::osstream_t oss;
          oss << "Incorrect replacement char '" << *it << "' on message '" << msg << "' at " << std::distance(it, msg.begin());
          msg = GetMessageByCode(CONF_COMP_ERR_INTERNAL_CODE);
          FormatMessage(msg, __FILE__, __LINE__, oss.str());
          return;
        }
        const string_t token{confstd::to_string(val)};
        msg.replace(it-1, it+1, token.cbegin(), token.cend());
      }
    }
#pragma warning(pop)

    static const string_t& GetMessageByCode(const string_t& code);
    static void RegisterStdMessages();

    static bool m_IsInitialized;
    static std::vector<std::pair<string_t, string_t>> m_Messages;
    string_t m_Msg;
    const string_t m_Code;
    const std::filesystem::path m_File;
    const uint32_t m_Line;
    const DiagnosticType m_Type;
  };

  using DiagnosticType = CompilerDiagnostic::DiagnosticType;
}