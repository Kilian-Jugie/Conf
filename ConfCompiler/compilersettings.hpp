#pragma once
#include "confcompglobal.hpp"
#include <vector>

namespace confcomp {
	class CompilerSettings {
	public:
		enum class DebugLevel {
			NONE = 0,
			MINIMAL = 1,
			MEDIUM = 2,
			IMPROVED = 3,
			MAXIMAL = 4,
			FULL = 5
		};

		void SetDebugLevel(DebugLevel lvl) noexcept;
		DebugLevel GetDebugLevel() const noexcept;

		void DisableWarning(string_t warn);
		void EnableWarning(string_t warn);

		void SetWarningsAsErrors(bool v) noexcept;

		bool IsWarningsAsErrors() const noexcept;
		bool IsWarningEnable(string_t warn) const noexcept;
	private:
		bool m_WarningsAsErrors;
		DebugLevel m_DebugLevel;
		std::vector<string_t> m_DisabledWarnings;
	};
}