#pragma once
#include "compilerresults.hpp"
#include "compilersettings.hpp"
#include <filesystem>
#include <vector>
#include <stack>

namespace confcomp {
	class Compiler {
	public:
		static constexpr char_t CONF_COMP_PREPROC_CHAR = '#';
		static constexpr char_t CONF_COMP_STRING_CHAR = '\"';
		static constexpr char_t CONF_COMP_LCOMM_SEQ[] = "//";
		static constexpr char_t CONF_COMP_MLCOMMO_SEQ[] = "/*";
		static constexpr char_t CONF_COMP_MLCOMMC_SEQ[] = "*/";
		static constexpr char_t CONF_COMP_PREPROC_TOK_INCLUDE[] = "include";
		static constexpr char_t CONF_COMP_PREPROC_TOK_DEFINE[] = "define";

		using DebugLevel = CompilerSettings::DebugLevel;

		static CompilerResults Compile(std::vector<std::filesystem::path> files, CompilerSettings settings);
	private:
		struct CompilerFile {
			std::filesystem::path path;
			uint32_t line;
		};

		class CompilerContext {
		public:
			CompilerContext(CompilerResults results, CompilerSettings settings) : 
				m_Results{std::move(results)}, m_Settings{std::move(settings)},
				m_CurrentFileIndex{0}, m_FileList{{}} {}

			template<typename... _Args>
			void AddDiag(string_t code, DiagnosticType type, _Args&&... format) {
				if(m_Settings.IsWarningEnable(code))
					m_Results.AddDiagnostic(CompilerDiagnostic(GetCurrentFile()->line, confstd::to_string(GetCurrentFile()->path.native()),
						std::move(code), type, std::forward<_Args>(format)...));
			}

			template<confstd::StringConstructible _StrTy, typename... _Args>
			void AddDiagIfLvl(CompilerSettings::DebugLevel minlvl, _StrTy&& code, DiagnosticType type, _Args&&... format) {
				if(m_Settings.GetDebugLevel() >= minlvl)
					AddDiag(std::forward<_StrTy>(code), type, std::forward<_Args>(format)...);
			}

			void SetCurrentFile(const std::filesystem::path& path);
			const CompilerFile* GetCurrentFile() const noexcept;
			const CompilerResults& GetResults() const noexcept;

			bool IsFileListed(const std::filesystem::path& path) const;
			void IncLine() noexcept;
		private:
			CompilerResults m_Results;
			CompilerSettings m_Settings;
			std::size_t m_CurrentFileIndex;

			std::vector<CompilerFile> m_FileList;
		};

		using ppcallback_t = void(*)(CompilerContext& context, string_view_t expr);

		static void CompileFile(const std::filesystem::path& file, CompilerContext& context);
		static void ThreatPreprocessorExpr(CompilerContext& context, string_view_t expr);
	
		static void PreprocessExpr(string_t& expr);
		static void CommentsPass(string_t& raw);

		static void Initialize();

		static bool m_IsInitialized;
		static std::vector<std::pair<string_t, ppcallback_t>> m_PPCallbacks;
		static std::vector<std::pair<string_t, string_t>> m_PPDefines;
	};

}