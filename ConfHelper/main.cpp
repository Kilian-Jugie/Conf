#include "compiler.hpp"
#include <iostream>
#include <unordered_map>

int main(int argc, char** argv) {
	std::unordered_map<confcomp::DiagnosticType, confcomp::string_t> diagTyTrad;
	diagTyTrad[confcomp::DiagnosticType::DEBUG] = "debug";
	diagTyTrad[confcomp::DiagnosticType::ERROR] = "error";
	diagTyTrad[confcomp::DiagnosticType::INFO] = "info";
	diagTyTrad[confcomp::DiagnosticType::WARNING] = "warning";

	confcomp::CompilerSettings settings;
	settings.SetDebugLevel(confcomp::Compiler::DebugLevel::FULL);
	settings.SetWarningsAsErrors(true);

	auto r = confcomp::Compiler::Compile({"test.conf"}, settings);
	std::for_each(r.GetDiagnostics().begin(), r.GetDiagnostics().end(), [&diagTyTrad](const confcomp::CompilerDiagnostic& err) {
		std::cerr << err.GetFile() << "(" << err.GetLine() << "): " << diagTyTrad[err.GetType()] <<  " " << err.GetCode() << ": " << err.GetMessage() << "\n";
		});
	std::cout << "Compilation ended with " << r.GetErrorCount(settings) << " errors.\n";
	return 0;
}