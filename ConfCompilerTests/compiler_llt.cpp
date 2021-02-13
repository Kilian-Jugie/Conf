#include "pch.h"
#include <compilerdiagnostic.hpp>

using namespace confcomp;

TEST(ConfCompFormat, All) {
  CompilerDiagnostic::RegisterMessage("T1000", "%? %? %? %?");
  CompilerDiagnostic err{42, __FILE__, "T1000", DiagnosticType::ERROR , "TEST", -1, 1, 10000000000};

  EXPECT_EQ(err.GetFile(), __FILE__);
  EXPECT_EQ(err.GetLine(), 42);
  EXPECT_EQ(err.GetMessage(), "TEST -1 1 10000000000");
}
