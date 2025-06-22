load("@rules_cc//cc:defs.bzl", "cc_test", "cc_library")

cc_test(
    name = "VerilogGrammarTest",
    srcs = [ "VerilogGrammarTest.cpp" ],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "VerilogPreprocessorTest",
    srcs = [ "VerilogPreprocessorTest.cpp" ],
    data = [
        "data/verilog/comments.v",
        "data/verilog/comments_expected.v",
        "data/verilog/include.v",
        "data/verilog/included.v",
        "data/verilog/include_expected.v",
    ],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "VerilogSpiritParserTest",
    srcs = [ "VerilogSpiritParserTest.cpp" ],
    data = [
        "data/verilog/basic.v",
        "data/verilog/flipflop.v"
    ],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "LogicEncoderTest",
    srcs = [ "LogicEncoderTest.cpp" ],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "SolverProxyTest",
    srcs = [ "SolverProxyTest.cpp" ],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "CircuitBuilderTest",
    srcs = [ "CircuitBuilderTest.cpp" ],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "TestPatternTest",
    srcs = [ "TestPatternTest.cpp"],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "UnaryCounterTest",
    srcs = [ "UnaryCounterTest.cpp"],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "BinaryCounterTest",
    srcs = [ "BinaryCounterTest.cpp"],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "CircuitSimulatorTest",
    srcs = [ "CircuitSimulatorTest.cpp"],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "LuaInvokerTest",
    srcs = [ "LuaInvokerTest.cpp"],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)

cc_test(
    name = "UdfmParserTest",
    srcs = [ "UdfmParserTest.cpp"],
    data = [
        "data/udfm/simple_empty_alternative.udfm",
        "data/udfm/simple_empty_cell.udfm",
        "data/udfm/simple_empty_fault.udfm",
        "data/udfm/simple_example.udfm",
        "data/udfm/tessent_example.udfm",
    ],
    copts = ["-iquote", "src", "-iquote", "test"],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)
