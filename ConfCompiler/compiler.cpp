#include "compiler.hpp"
#include "confstdparse.hpp"
#include "compilerdiagnostic.hpp"
#include <confstdfstream.hpp>
#include <confstdsstream.hpp>
#include <confstdfilesystem.hpp>
#include <algorithm>
#include <string>

#include <iostream>

namespace confcomp {
  using confstd::ifstream_t;
  using confstd::osstream_t;
  using confstd::string_t;

  bool Compiler::m_IsInitialized = false;
  std::vector<std::pair<string_t, Compiler::ppcallback_t>> Compiler::m_PPCallbacks = {};
  std::vector<std::pair<string_t, string_t>> Compiler::m_PPDefines = {};

  void Compiler::CompileFile(const std::filesystem::path& file, CompilerContext& context) {
    if(!m_IsInitialized) {
      Initialize();
      m_IsInitialized = true;
    }

    if(!std::filesystem::exists(file)) {
      context.AddDiag("C1001", DiagnosticType::ERROR, file.native());
      return;
    }

    // Refs/ptrs are invalidated during potential vector realloc so we use copy
    const auto tcurf = context.GetCurrentFile() ? *context.GetCurrentFile() : CompilerFile{};
    context.SetCurrentFile(file);

    ifstream_t in{file};
    osstream_t ss;
    ss << in.rdbuf();
    string_t str{ss.str()};

    CommentsPass(str);

    std::vector<string_t> splitted = confstd::split(str, ";^{^}\n\r");
    
    for(auto& expr : splitted) {
      context.IncLine();
      confstd::trim(expr);
      confstd::press(expr);
      PreprocessExpr(expr);
      if(expr[0] == CONF_COMP_PREPROC_CHAR)
        ThreatPreprocessorExpr(context, expr);
    }

    if(tcurf.path.string().size() > 0)
      context.SetCurrentFile(tcurf.path);
  }

  void Compiler::ThreatPreprocessorExpr(CompilerContext& context, string_view_t expr) {
    const auto cexpr = expr.substr(1);
    std::vector<string_t> splitted = confstd::split(cexpr, " ");
    for(const auto& it : m_PPCallbacks) {
      if(it.first == splitted[0]) {
        it.second(context, cexpr);
        return;
      }
    }
  }

  void Compiler::PreprocessExpr(string_t& expr) {
    for(const auto& it : m_PPDefines)
      confstd::strreplace(expr, it.first, it.second, [&expr, &it](std::size_t pos) {
      pos += it.first.size();
      return pos >= expr.size() || !isalnum(expr[pos]) || isspace(expr[pos]);
        });
  }

  void Compiler::CommentsPass(string_t& raw) {
    __conf_constini auto lcomlen = confstd::length(CONF_COMP_LCOMM_SEQ)-1;
    __conf_constini auto mlcomolen = confstd::length(CONF_COMP_MLCOMMO_SEQ)-1;
    bool inString = false;
    for(std::size_t i{0}; i < raw.size(); ++i) {
      if(raw[i] == CONF_COMP_STRING_CHAR) 
        inString ^= true;
      else if(inString) continue;
      else if(raw[i] == CONF_COMP_LCOMM_SEQ[0] || raw[i] == CONF_COMP_MLCOMMO_SEQ[0]) {
        if(i + lcomlen + 1 < raw.size()) {
          string_t clue{raw.begin() + i, raw.begin() + i + lcomlen};
          if(clue == CONF_COMP_LCOMM_SEQ) {
            auto endpos = raw.find('\n', i);
            if(endpos == std::decay_t<decltype(raw)>::npos) 
              endpos = raw.size()-1;
            i = std::distance(raw.begin(), raw.erase(raw.cbegin() + i, raw.cbegin() + endpos + 1));
          }
        }
        if(i + mlcomolen + 1 < raw.size()) {
          string_t clue{raw.begin() + i, raw.begin() + i + mlcomolen};
          if(clue == CONF_COMP_MLCOMMO_SEQ) {
            auto endpos = raw.find(CONF_COMP_MLCOMMC_SEQ, i);
            if(endpos == std::decay_t<decltype(raw)>::npos)
              endpos = (raw.size() - mlcomolen);
            i = std::distance(raw.begin(), raw.erase(raw.cbegin() + i,
              raw.cbegin() + endpos + mlcomolen));
          }
        }
      }
    }
  }

  void Compiler::Initialize() {
    m_PPCallbacks.push_back(std::make_pair(CONF_COMP_PREPROC_TOK_INCLUDE, [](CompilerContext& context, string_view_t expr) {
      const auto separator = expr.find(' ');
      if(separator == decltype(expr)::npos) {
        context.AddDiag("C2001", DiagnosticType::ERROR, expr, 1, 0);
        return;
      }
      auto fname = expr.substr(confstd::length(CONF_COMP_PREPROC_TOK_INCLUDE));
      if(context.IsFileListed(fname)) {
        context.AddDiagIfLvl(DebugLevel::FULL, "I1001", DiagnosticType::INFO, fname);
        return;
      }
      CompileFile(fname, context);
      }));

    m_PPCallbacks.push_back(std::make_pair(CONF_COMP_PREPROC_TOK_DEFINE, [](CompilerContext& context, string_view_t expr) {
      __conf_constini auto pptokmlen = confstd::length(CONF_COMP_PREPROC_TOK_DEFINE);
      const auto separator = expr.find(' ', pptokmlen);
      const auto tok = expr.substr(pptokmlen, separator-pptokmlen);
      string_t rep{""};
      if(separator != decltype(expr)::npos)
        rep = static_cast<string_t>(expr.substr(separator+1));
      m_PPDefines.push_back(std::make_pair(static_cast<string_t>(tok), std::move(rep)));
      }));
  }

  CompilerResults Compiler::Compile(std::vector<std::filesystem::path> files, CompilerSettings settings) {
    CompilerResults results;
    CompilerContext context{results, std::move(settings)};
    std::for_each(files.begin(), files.end(),
      [&context](const std::filesystem::path& p) { CompileFile(p, context); });
    return context.GetResults();
  }

  
  void Compiler::CompilerContext::SetCurrentFile(const std::filesystem::path& path) {
    const auto it = std::find_if(m_FileList.begin(), m_FileList.end(),
      [&path](const CompilerFile& f) { return path == f.path;	});
    if(it != m_FileList.end())
      m_CurrentFileIndex = std::distance(m_FileList.begin(), it);
    else {
      m_FileList.push_back({.path = path, .line = 0});
      m_CurrentFileIndex = m_FileList.size() - 1;
    }
  }

  const CompilerResults& Compiler::CompilerContext::GetResults() const noexcept {
    return m_Results;
  }

  uint32_t CompilerResults::GetErrorCount(const CompilerSettings& settings) const noexcept {
    uint32_t ret{0U};
    for(const auto& it : GetDiagnostics()) {
      if(it.GetType() == DiagnosticType::ERROR ||
        (settings.IsWarningsAsErrors() && it.GetType() == DiagnosticType::WARNING))
        ++ret;
    }
    return ret;
  }

  const Compiler::CompilerFile* Compiler::CompilerContext::GetCurrentFile() const noexcept {
    return &m_FileList[m_CurrentFileIndex];
  }

  bool Compiler::CompilerContext::IsFileListed(const std::filesystem::path& path) const {
    return std::find_if(m_FileList.cbegin(), m_FileList.cend(),
      [&path](const CompilerFile& f) { return path == f.path;	}) != m_FileList.cend();;
  }
  void Compiler::CompilerContext::IncLine() noexcept {
    ++m_FileList[m_CurrentFileIndex].line;
  }
}