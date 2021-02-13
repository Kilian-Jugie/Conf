#pragma once
#include "compilerdiagnostic.hpp"
#include "confcompglobal.hpp"
#include <vector>

namespace confcomp {
	class CompilerResults {
	public:
		void AddDiagnostic(CompilerDiagnostic err) {
			m_Diagnostics.push_back(std::move(err));
		}

		const std::vector<CompilerDiagnostic>& GetDiagnostics() const noexcept {
			return m_Diagnostics;
		}

		uint32_t GetErrorCount(const CompilerSettings& settings) const noexcept;

	private:
		std::vector<CompilerDiagnostic> m_Diagnostics;
	};
}