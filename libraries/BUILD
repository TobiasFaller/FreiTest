load("@rules_cc//cc:defs.bzl", "cc_library")

# -----------------------------------------------------------------------------
# Other
# -----------------------------------------------------------------------------
# https://github.com/greg7mdp/sparsepp
cc_library(
    name = "sparsepp",
    srcs = [],
    hdrs = glob(["other/sparsepp/sparsepp/**/*.h"]),
    strip_include_prefix = "other/sparsepp/",
    visibility = ["//visibility:public"]
)
# https://github.com/amrayn/easyloggingpp
cc_library(
    name = "easyloggingpp",
    srcs = glob(["other/easyloggingpp/src/*.cc"]),
    hdrs = glob(["other/easyloggingpp/src/*.h"]),
    strip_include_prefix = "other/easyloggingpp/src",
    visibility = ["//visibility:public"],
    copts = [
        "-Wno-range-loop-construct"
    ]
)
# https://github.com/Tessil/hat-trie
cc_library(
    name = "hat-trie",
    hdrs = glob(["other/hat-trie/include/**/*.h"]),
    strip_include_prefix = "other/hat-trie/include",
    visibility = ["//visibility:public"]
)
#https://github.com/andportnoy/riscv-disassembler
cc_library(
    name = "capstone",
    hdrs = glob(["other/capstone/include/capstone/*.h"]),
    strip_include_prefix = "other/capstone/include/capstone/",
    include_prefix = "capstone",
    visibility = ["//visibility:public"],
    deps = [":capstone-implementation"]
)
cc_library(
    name = "capstone-implementation",
    srcs = glob(["other/capstone/arch/**/*.c", "other/capstone/*.c"]),
    hdrs = glob(["other/capstone/arch/**/*.h", "other/capstone/arch/**/*.inc", "other/capstone/*.h", "other/capstone/include/**/*.h"]),
    copts = [
        "-isystem", "libraries/other/capstone/include/",
    ]
)
# https://github.com/lua/lua
cc_library(
    name = "lua",
    hdrs = glob(["other/lua/*.h"]),
    strip_include_prefix = "other/lua/",
    include_prefix = "lua",
    visibility = ["//visibility:public"],
    deps = [":lua-implementation"]
)
cc_library(
    name = "lua-implementation",
    srcs = glob(["other/lua/*.c"], exclude=["**/onelua.c", "**/lua.c", "**/luac.c"]),
    hdrs = glob(["other/lua/*.h"]),
    copts = [
        "-iquote", "libraries/other/lua/",
    ]
)

# -----------------------------------------------------------------------------
# BMC-Solver NCIP (Next Craig Interpolant Prover)
# -----------------------------------------------------------------------------

cc_library(
    name = "ncip",
    defines = ["HAS_BMC_SOLVER_NCIP"],
    visibility = ["//visibility:public"],
    deps = [":ncip-source"]
)
cc_library(
    name = "ncip-source",
    hdrs = glob(["solver/bmc-ncip/src/**/*.hpp"]),
    strip_include_prefix = "solver/bmc-ncip/src",
    include_prefix = "ncip",
    defines = ["NCIP_BACKEND_MINICRAIG", "NCIP_BACKEND_CADICRAIG"],
    deps = [":ncip-implementation"],
)
cc_library(
    name = "ncip-implementation",
    srcs = glob(["solver/bmc-ncip/src/**/*.cpp"], exclude=["solver/bmc-ncip/src/**/bmc-prover.cpp"]),
    hdrs = glob(["solver/bmc-ncip/src/**/*.hpp"]),
    defines = ["INCLUDE_PREFIXED", "NCIP_BACKEND_MINICRAIG", "NCIP_BACKEND_CADICRAIG"],
    deps = [":minicraig-2.2.0", ":cadicraig"],
    copts = [
        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-misleading-indentation",

        # Implementation-local defines (don't put in "defines")
        "-DNDEBUG",

        "-iquote", "libraries/solver/bmc-ncip/src/"
    ],
)

# -----------------------------------------------------------------------------
# Max SAT Solver Pacose
# -----------------------------------------------------------------------------

# https://gitlab.tf.uni-freiburg.de/solver/maxsat-solver/pacose-maxsat-solver
cc_library(
    name = "pacose",
    defines = ["HAS_MAX_SAT_SOLVER_PACOSE"],
    visibility = ["//visibility:public"],
    deps = [":pacose-source"]
)
cc_library(
    name = "pacose-source",
    hdrs = glob([
        "solver/max-sat-pacose/maxSAT/Pacose.h",
        "solver/max-sat-pacose/maxSAT/Encodings.h",
        "solver/max-sat-pacose/maxSAT/Settings.h",
        "solver/max-sat-pacose/maxSAT/Softclause.h",
        "solver/max-sat-pacose/maxSAT/Greedyprepro.h",
        "solver/max-sat-pacose/maxSAT/DGPW/deallocator.h",
        "solver/max-sat-pacose/Helper/ClauseDB.h",
        "solver/max-sat-pacose/solver-proxy/SATSolverProxy.h"
    ]),
    strip_include_prefix = "solver/max-sat-pacose/",
    include_prefix = "pacose",
    deps = [":pacose-implementation"]
)
cc_library(
    name = "pacose-implementation",
    srcs = glob([
        "solver/max-sat-pacose/maxSAT/**/*.cpp",
        "solver/max-sat-pacose/Helper/**/*.cpp",
        "solver/max-sat-pacose/solver-proxy/SATSolverProxy.cpp",
        "solver/max-sat-pacose/solver-proxy/CadicalSolverProxy.cpp",
        "solver/max-sat-pacose/solver-proxy/Glucose421SolverProxy.cpp"
    ]),
    hdrs = glob([
        "solver/max-sat-pacose/maxSAT/**/*.h",
        "solver/max-sat-pacose/Helper/**/*.h",
        "solver/max-sat-pacose/Helper/**/*.hpp",
        "solver/max-sat-pacose/solver-proxy/**/*.h",
        "solver/max-sat-pacose/solver-proxy/**/*.hpp",
    ]),
    copts = [
        "-iquote", "libraries/solver/max-sat-pacose/maxSAT",
        "-iquote", "libraries/solver/max-sat-pacose/maxSAT/DGPW",
        "-iquote", "libraries/solver/max-sat-pacose/solver-proxy",

        "-Wno-sign-compare",
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
        "-Wno-reorder",
        "-Wno-misleading-indentation",
        "-Wno-parentheses",
        "-Wno-class-memaccess",
        "-Wno-deprecated-declarations",
        "-Wno-unknown-pragmas",
        "-Wno-Wmaybe-uninitialized",
    ]
)

# -----------------------------------------------------------------------------
# SAT Solver Glucose
# -----------------------------------------------------------------------------

# https://www.labri.fr/perso/lsimon/glucose/
cc_library(
    name = "glucose-4.2.1",
    defines = [
        "HAS_SAT_SOLVER_GLUCOSE_421_SINGLE",
        "HAS_SAT_SOLVER_GLUCOSE_421_PARALLEL"
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
    deps = ["glucose-4.2.1-source"]
)
cc_library(
    name = "glucose-4.2.1-source",
    hdrs = glob(["solver/sat-glucose-4.2.1/**/*.h"]),
    strip_include_prefix = "solver/sat-glucose-4.2.1",
    include_prefix = "glucose-4.2.1",
    deps = [":glucose-4.2.1-implementation"]
)
cc_library(
    name = "glucose-4.2.1-implementation",
    srcs = glob(
        include=["solver/sat-glucose-4.2.1/**/*.cc"],
        exclude=["solver/sat-glucose-4.2.1/**/Main.cc"]
    ),
    hdrs = glob(["solver/sat-glucose-4.2.1/**/*.h"]),
    copts = [
        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-misleading-indentation",
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
        "-Wno-reorder",
        "-Wno-unknown-pragmas",
        "-Wno-maybe-uninitialized",
    ]
)

cc_library(
    name = "glucose-4.1",
    defines = ["HAS_SAT_SOLVER_GLUCOSE_41_SINGLE"],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
    deps = ["glucose-4.1-source"]
)
cc_library(
    name = "glucose-4.1-source",
    hdrs = glob(["solver/sat-glucose-4.1/**/*.h"]),
    strip_include_prefix = "solver/sat-glucose-4.1",
    include_prefix = "glucose-4.1",
    visibility = ["//visibility:public"],
    deps = [":glucose-4.1-implementation"]
)
cc_library(
    name = "glucose-4.1-implementation",
    srcs = glob(
        include=["solver/sat-glucose-4.1/**/*.cc"],
        exclude=["solver/sat-glucose-4.1/**/Main.cc"]
    ),
    hdrs = glob(["solver/sat-glucose-4.1/**/*.h"]),
    copts = [
        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-misleading-indentation",
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
        "-Wno-reorder",
        "-Wno-unknown-pragmas"
    ]
)

# -----------------------------------------------------------------------------
# SAT Solver CaDiCaL
# -----------------------------------------------------------------------------

# https://github.com/arminbiere/cadical
cc_library(
    name = "cadical",
    defines = ["HAS_SAT_SOLVER_CADICAL"],
    visibility = ["//visibility:public"],
    deps = ["cadical-source"]
)
cc_library(
    name = "cadical-source",
    hdrs = glob(["solver/sat-cadical/src/**/*.h*"]),
    strip_include_prefix = "solver/sat-cadical/src",
    include_prefix = "cadical",
    copts = [ "-DNBUILD" ],
    deps = [":cadical-implementation"]
)
cc_library(
    name = "cadical-implementation",
    srcs = glob(
        include=["solver/sat-cadical/src/**/*.cpp"],
        exclude=[
            "solver/sat-cadical/src/**/cadical.cpp",
            "solver/sat-cadical/src/**/mobical.cpp"
        ]
    ),
    hdrs = glob(["solver/sat-cadical/src/**/*.h*"]),
    defines = ["VERSION=\\\"2.0.0\\\""],
    copts = [
        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-misleading-indentation",

        "-DNBUILD",

        "-iquote", "libraries/solver/sat-cadical/src/"
    ],
)

# -----------------------------------------------------------------------------
# SAT Solver CaDiCaL with Craig interpolation
# -----------------------------------------------------------------------------

# https://github.com/arminbiere/cadical
cc_library(
    name = "cadicraig",
    defines = ["HAS_SAT_SOLVER_CADICRAIG"],
    visibility = ["//visibility:public"],
    deps = ["cadicraig-source"]
)
cc_library(
    name = "cadicraig-source",
    hdrs = glob(["solver/sat-cadical/contrib/**/*.hpp"]),
    strip_include_prefix = "solver/sat-cadical/contrib",
    include_prefix = "cadicraig",
    defines = ["INCLUDE_PREFIXED"],
    deps = [":cadicraig-implementation"]
)
cc_library(
    name = "cadicraig-implementation",
    srcs = glob(["solver/sat-cadical/contrib/craigtracer.cpp"]),
    hdrs = glob(["solver/sat-cadical/contrib/craigtracer.hpp"]),
    defines = [ "INCLUDE_PREFIXED" ],
    copts = [
        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-misleading-indentation",
        "-Wno-unused-value",

        "-iquote", "libraries/solver/sat-cadical/contrib/"
    ],
    deps = [":cadical"]
)

# -----------------------------------------------------------------------------
# SAT Solver MiniSAT
# -----------------------------------------------------------------------------

cc_library(
    name = "minisat-2.2.0",
    defines = ["HAS_SAT_SOLVER_MINISAT"],
    linkopts = ["-lz", "-pthread"],
    visibility = ["//visibility:public"],
    deps = ["minisat-2.2.0-source"]
)
cc_library(
    name = "minisat-2.2.0-source",
    hdrs = glob(["solver/sat-minisat-2.2.0/minisat/**/*.h"]),
    strip_include_prefix = "solver/sat-minisat-2.2.0/minisat/",
    include_prefix = "minisat-2.2.0",
    deps = [":minisat-2.2.0-implementation"]
)
cc_library(
    name = "minisat-2.2.0-implementation",
    srcs = glob(
        include=["solver/sat-minisat-2.2.0/minisat/**/*.cc"],
        exclude=["solver/sat-minisat-2.2.0/minisat/**/Main.cc"]
    ),
    hdrs = glob(["solver/sat-minisat-2.2.0/minisat/**/*.h"]),
    copts = [
        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-misleading-indentation"
    ]
)

# -----------------------------------------------------------------------------
# SAT Solver MiniCraig
# -----------------------------------------------------------------------------

cc_library(
    name = "minicraig-2.2.0",
    defines = ["HAS_SAT_SOLVER_MINICRAIG"],
    linkopts = ["-lz", "-pthread"],
    visibility = ["//visibility:public"],
    deps = ["minicraig-2.2.0-source"]
)
cc_library(
    name = "minicraig-2.2.0-source",
    hdrs = glob(["solver/sat-minicraig-2.2.0/minisat/**/*.h"]),
    strip_include_prefix = "solver/sat-minicraig-2.2.0/minisat/",
    include_prefix = "minicraig-2.2.0",
    deps = [":minicraig-2.2.0-implementation"]
)
cc_library(
    name = "minicraig-2.2.0-implementation",
    srcs = glob(
        include=["solver/sat-minicraig-2.2.0/minisat/**/*.cc"],
        exclude=["solver/sat-minicraig-2.2.0/minisat/**/Main.cc"]
    ),
    hdrs = glob([
        "solver/sat-minicraig-2.2.0/minisat/**/*.h",
        "solver/sat-minicraig-2.2.0/minisat/core/Solver.cc",
        "solver/sat-minicraig-2.2.0/minisat/simp/SimpSolver.cc"
    ]),
    copts = [
        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-misleading-indentation",

        "-iquote", "libraries/sat-minicraig-2.2.0/minisat/",
    ]
)

# -----------------------------------------------------------------------------
# SAT Solver Crypto MiniSAT
# -----------------------------------------------------------------------------

cc_library(
    name = "crypto-minisat",
    defines = ["HAS_SAT_SOLVER_CRYPTO_MINISAT"],
    visibility = ["//visibility:public"],
    deps = ["crypto-minisat-source"],
    linkopts = ["-lgmp"]
)
cc_library(
    name = "crypto-minisat-source",
    hdrs = glob(["solver/sat-crypto-minisat/src/**/*.h"], exclude=[
        "solver/sat-crypto-minisat/src/msvc/*.h",
        "solver/sat-crypto-minisat/src/mpi/*.h",
    ]),
    strip_include_prefix = "solver/sat-crypto-minisat/src/",
    include_prefix = "crypto-minisat",
    deps = [":crypto-minisat-implementation"]
)
cc_library(
    name = "crypto-minisat-implementation",
    srcs = glob([
        "solver/sat-crypto-minisat/src/cnf.cpp",
        "solver/sat-crypto-minisat/src/drat.cpp",
        "solver/sat-crypto-minisat/src/propengine.cpp",
        "solver/sat-crypto-minisat/src/varreplacer.cpp",
        "solver/sat-crypto-minisat/src/clausecleaner.cpp",
        "solver/sat-crypto-minisat/src/occsimplifier.cpp",
        "solver/sat-crypto-minisat/src/subsumestrengthen.cpp",
        "solver/sat-crypto-minisat/src/clauseallocator.cpp",
        "solver/sat-crypto-minisat/src/sccfinder.cpp",
        "solver/sat-crypto-minisat/src/solverconf.cpp",
        "solver/sat-crypto-minisat/src/distillerlong.cpp",
        "solver/sat-crypto-minisat/src/distillerlongwithimpl.cpp",
        "solver/sat-crypto-minisat/src/str_impl_w_impl.cpp",
        "solver/sat-crypto-minisat/src/solutionextender.cpp",
        "solver/sat-crypto-minisat/src/completedetachreattacher.cpp",
        "solver/sat-crypto-minisat/src/searcher.cpp",
        "solver/sat-crypto-minisat/src/solver.cpp",
        "solver/sat-crypto-minisat/src/compfinder.cpp",
        "solver/sat-crypto-minisat/src/comphandler.cpp",
        "solver/sat-crypto-minisat/src/hyperengine.cpp",
        "solver/sat-crypto-minisat/src/subsumeimplicit.cpp",
        "solver/sat-crypto-minisat/src/datasync.cpp",
        "solver/sat-crypto-minisat/src/reducedb.cpp",
        "solver/sat-crypto-minisat/src/clausedumper.cpp",
        "solver/sat-crypto-minisat/src/bva.cpp",
        "solver/sat-crypto-minisat/src/intree.cpp",
        "solver/sat-crypto-minisat/src/satzilla_features_calc.cpp",
        "solver/sat-crypto-minisat/src/satzilla_features_to_reconf.cpp",
        "solver/sat-crypto-minisat/src/satzilla_features.cpp",
        "solver/sat-crypto-minisat/src/searchstats.cpp",
        "solver/sat-crypto-minisat/src/xorfinder.cpp",
        "solver/sat-crypto-minisat/src/cardfinder.cpp",
        "solver/sat-crypto-minisat/src/cryptominisat.cpp",
        "solver/sat-crypto-minisat/src/cryptominisat_c.cpp",
        "solver/sat-crypto-minisat/src/walksat.cpp",
        "solver/sat-crypto-minisat/src/sls.cpp",
        "solver/sat-crypto-minisat/src/vardistgen.cpp",
        "solver/sat-crypto-minisat/src/ccnr.cpp",
        "solver/sat-crypto-minisat/src/ccnr_cms.cpp",
        "solver/sat-crypto-minisat/src/lucky.cpp",
        "solver/sat-crypto-minisat/src/GitSHA1.cpp",
    ]),
    hdrs = glob(["solver/sat-crypto-minisat/src/**/*.h"], exclude=[
        "solver/sat-crypto-minisat/src/msvc/*.h",
        "solver/sat-crypto-minisat/src/mpi/*.h",
    ]),
    copts = [
        "-Wno-deprecated-declarations",
        "-Wno-mismatched-tags",
        "-Wno-class-memaccess",
        "-Wno-sign-compare",
        "-Wno-unused-label",
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
        "-Wno-range-loop-construct",
        "-Wno-maybe-uninitialized",
    ]
)

# -----------------------------------------------------------------------------
# Max-SAT Solver Incremental OpenWBO (Complete)
# -----------------------------------------------------------------------------

cc_library(
    name = "inc-bmo-complete",
    defines = ["HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE"],
    visibility = ["//visibility:public"],
    deps = ["inc-bmo-complete-source"]
)
cc_library(
    name = "inc-bmo-complete-source",
    hdrs = glob(["solver/max-sat-inc-bmo-complete/**/*.h"]),
    strip_include_prefix = "solver/max-sat-inc-bmo-complete/",
    include_prefix = "inc-bmo-complete",
    defines = ["BMO_SOLVER_NSPACE=Glucose41"],
    deps = [":inc-bmo-complete-implementation", ":glucose-4.1"]
)
cc_library(
    name = "inc-bmo-complete-implementation",
    srcs = glob(
        include=["solver/max-sat-inc-bmo-complete/**/*.cc"],
        exclude=["solver/max-sat-inc-bmo-complete/**/Main.cc"]
    ),
    hdrs = glob(["solver/max-sat-inc-bmo-complete/**/*.h"]),
    copts = [
        "-iquote", "libraries/solver/max-sat-inc-bmo-complete/",

        "-Wno-class-memaccess",
        "-Wno-parentheses",
        "-Wno-sign-compare",
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
        "-Wno-misleading-indentation",
        "-Wno-deprecated-declarations",
        "-Wno-unused-label",
        "-Wno-unknown-pragmas"
    ],
    defines = ["BMO_SOLVER_NSPACE=Glucose41"],
    deps = [":glucose-4.1"]
)

# -----------------------------------------------------------------------------
# Static / Dynamic IPASIR Solver Loader
# -----------------------------------------------------------------------------

cc_library(
    name = "ipasir-dynamic",
    defines = ["HAS_SAT_SOLVER_IPASIR_DYNAMIC"],
    srcs = [],
    linkopts = ["-ldl"],
    visibility = ["//visibility:public"]
)

cc_library(
    name = "ipasir-static",
    defines = ["HAS_SAT_SOLVER_IPASIR_STATIC"],
    hdrs = glob(["solver/sat-ipasir/*.h"]),
    strip_include_prefix = "solver/sat-ipasir/",
    include_prefix = "ipasir",
    visibility = ["//visibility:public"],
    deps = [":ipasir-static-implementation"]
)
cc_library(
    name = "ipasir-static-implementation",
    # Adjust the following line to link against the desired static IPASIR solver
    srcs = glob(["solver/sat-ipasir-solvers/libipasirminisat220.a"])
)

