#include "pch.h"
#include <compiler.hpp>
#include <compilerdiagnostic.hpp>
#include <confstdfstream.hpp>

using namespace confcomp;

constexpr char_t TEST_FNAME_HLT_STD[] = "compiler_hlt_test.conf";

CompilerResults compileText(string_t text, CompilerSettings settings) {
	confstd::ofstream_t of(TEST_FNAME_HLT_STD);
	of << text;
	of.close();

	settings.SetDebugLevel(confcomp::Compiler::DebugLevel::FULL);
	settings.SetWarningsAsErrors(true);

	auto r = confcomp::Compiler::Compile({TEST_FNAME_HLT_STD}, settings);

	std::remove(TEST_FNAME_HLT_STD);

	return r;
}

CompilerSettings getTestSettings() {
	CompilerSettings settings;
	settings.SetDebugLevel(confcomp::Compiler::DebugLevel::FULL);
	settings.SetWarningsAsErrors(true);
	return settings;
}

CompilerResults compileText(string_t text) {
	return compileText(text, getTestSettings());
}

#define CONF_TEST_HLT_DONE(r) EXPECT_EQ(r.GetErrorCount(getTestSettings()), 0)
#define CONF_TEST_HLT_ERROR(r) EXPECT_GT(r.GetErrorCount(getTestSettings()), 0)

//We use include which produce an error if no file is provided even in earlier versions of conf
TEST(ConfCompComment, Inline) {
	CONF_TEST_HLT_DONE(compileText("//#include"));
	CONF_TEST_HLT_DONE(compileText("//#include\n//#include\n//#include"));
	CONF_TEST_HLT_DONE(compileText("//#include/*\n//*/#include\n"));
}

TEST(ConfCompComment, MultiLine) {
	CONF_TEST_HLT_DONE(compileText("/*#include*/"));
	CONF_TEST_HLT_DONE(compileText("/*#include\n//#include\n#include*/"));
	CONF_TEST_HLT_DONE(compileText("/*#include//*/\n"));
}

TEST(ConfCompInclude, NoArg) {
	CONF_TEST_HLT_ERROR(compileText("#include"));
	CONF_TEST_HLT_ERROR(compileText("#include "));
	CONF_TEST_HLT_ERROR(compileText("#include\n"));
}

TEST(ConfCompInclude, Process) {
	confstd::ofstream_t of("compiler_hlt_include_test.conf");
	of << "#define TEST_DEFINE test\n";
	of.close();

	CONF_TEST_HLT_DONE(compileText("#include compiler_hlt_include_test.conf"));
	std::remove("compiler_hlt_include_test.conf");

	of.open("compiler_hlt_include_test.conf");
	of << "#include";
	of.close();

	CONF_TEST_HLT_ERROR(compileText("#include compiler_hlt_include_test.conf"));
	std::remove("compiler_hlt_include_test.conf");
}