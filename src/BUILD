load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

# Solver for Solver Proxy
DEFAULT_NCIP = ["//libraries:ncip"]
DEFAULT_PACOSE = ["//libraries:pacose"]
DEFAULT_CADICAL = ["//libraries:cadical"]
DEFAULT_CADICRAIG = ["//libraries:cadicraig"]
DEFAULT_CRYPTO_MINISAT = ["//libraries:crypto-minisat"]
DEFAULT_INC_BMO_COMPLETE = ["//libraries:inc-bmo-complete"]
DEFAULT_IPASIR_DYNAMIC = ["//libraries:ipasir-dynamic"]
DEFAULT_IPASIR_STATIC = []
DEFAULT_GLUCOSE_421 = ["//libraries:glucose-4.2.1"]
DEFAULT_GLUCOSE_41 = ["//libraries:glucose-4.1"]
DEFAULT_MINISAT_220 = ["//libraries:minisat-2.2.0"]
DEFAULT_MINICRAIG_220 = ["//libraries:minicraig-2.2.0"]


config_setting(
    name = "enable_ncip",
    define_values = {"enable_cip": "yes"}
)
config_setting(
    name = "disable_ncip",
    define_values = {"enable_cip": "no"}
)

config_setting(
    name = "enable_pacose",
    define_values = {"enable_pacose": "yes"}
)
config_setting(
    name = "disable_pacose",
    define_values = {"enable_pacose": "no"}
)

config_setting(
    name = "enable_cadical",
    define_values = {"enable_cadical": "yes"}
)
config_setting(
    name = "disable_cadical",
    define_values = {"enable_cadical": "no"}
)

config_setting(
    name = "enable_cadicraig",
    define_values = {"enable_cadicraig": "yes"}
)
config_setting(
    name = "disable_cadicraig",
    define_values = {"enable_cadicraig": "no"}
)

config_setting(
    name = "enable_crypto_minisat",
    define_values = {"enable_crypto_minisat": "yes"}
)
config_setting(
    name = "disable_crypto_minisat",
    define_values = {"enable_crypto_minisat": "no"}
)

config_setting(
    name = "enable_inc_bmo_complete",
    define_values = {"enable_inc_bmo_complete": "yes"}
)
config_setting(
    name = "disable_inc_bmo_complete",
    define_values = {"enable_inc_bmo_complete": "no"}
)

config_setting(
    name = "enable_ipasir_dynamic",
    define_values = {"enable_ipasir_dynamic": "yes"}
)
config_setting(
    name = "disable_ipasir_dynamic",
    define_values = {"enable_ipasir_dynamic": "no"}
)

config_setting(
    name = "enable_ipasir_static",
    define_values = {"enable_ipasir_static": "yes"}
)
config_setting(
    name = "disable_ipasir_static",
    define_values = {"enable_ipasir_static": "no"}
)

config_setting(
    name = "enable_glucose_421",
    define_values = {"enable_glucose_421": "yes"}
)
config_setting(
    name = "disable_glucose_421",
    define_values = {"enable_glucose_421": "no"}
)

config_setting(
    name = "enable_glucose_41",
    define_values = {"enable_glucose_41": "yes"}
)
config_setting(
    name = "disable_glucose_41",
    define_values = {"enable_glucose_41": "no"}
)

config_setting(
    name = "enable_minisat_220",
    define_values = {"enable_minisat_220": "yes"}
)
config_setting(
    name = "disable_minisat_220",
    define_values = {"enable_minisat_220": "no"}
)

config_setting(
    name = "enable_minicraig_220",
    define_values = {"enable_minicraig_220": "yes"}
)
config_setting(
    name = "disable_minicraig_220",
    define_values = {"enable_minicraig_220": "no"}
)

config_setting(
    name = "enable_starvision",
    define_values = {"enable_starvision": "yes"}
)
config_setting(
    name = "disable_starvision",
    define_values = {"enable_starvision": "no"}
)

# -----------------------------------------------------------------------------
# Solver for Solver Proxy
# -----------------------------------------------------------------------------

cc_library(
    name = "ncip",
    deps = select({
        "enable_ncip": ["//libraries:ncip"],
        "disable_ncip": [],
        "//conditions:default": DEFAULT_NCIP
    })
)

cc_library(
    name = "pacose",
    deps = select({
        "enable_pacose": ["//libraries:pacose"],
        "disable_pacose": [],
        "//conditions:default": DEFAULT_PACOSE
    })
)

cc_library(
    name = "cadical",
    deps = select({
        "enable_cadical": ["//libraries:cadical"],
        "disable_cadical": [],
        "//conditions:default": DEFAULT_CADICAL
    })
)

cc_library(
    name = "cadicraig",
    deps = select({
        "enable_cadicraig": ["//libraries:cadicraig"],
        "disable_cadicraig": [],
        "//conditions:default": DEFAULT_CADICRAIG
    })
)

cc_library(
    name = "crypto-minisat",
    deps = select({
        "enable_crypto_minisat": ["//libraries:crypto-minisat"],
        "disable_crypto_minisat": [],
        "//conditions:default": DEFAULT_CRYPTO_MINISAT
    })
)

cc_library(
    name = "inc-bmo-complete",
    deps = select({
        "enable_inc_bmo_complete": ["//libraries:inc-bmo-complete"],
        "disable_inc_bmo_complete": [],
        "//conditions:default": DEFAULT_INC_BMO_COMPLETE
    })
)

cc_library(
    name = "ipasir-dynamic",
    deps = select({
        "enable_ipasir_dynamic": ["//libraries:ipasir-dynamic"],
        "disable_ipasir_dynamic": [],
        "//conditions:default": DEFAULT_IPASIR_DYNAMIC
    })
)

cc_library(
    name = "ipasir-static",
    deps = select({
        "enable_ipasir_static": ["//libraries:ipasir-static"],
        "disable_ipasir_static": [],
        "//conditions:default": DEFAULT_IPASIR_STATIC
    })
)

cc_library(
    name = "glucose-4.2.1",
    deps = select({
        "enable_glucose_421": ["//libraries:glucose-4.2.1"],
        "disable_glucose_421": [],
        "//conditions:default": DEFAULT_GLUCOSE_421
    })
)

cc_library(
    name = "glucose-4.1",
    deps = select({
        "enable_glucose_41": ["//libraries:glucose-4.1"],
        "disable_glucose_41": [],
        "//conditions:default": DEFAULT_GLUCOSE_41
    })
)

cc_library(
    name = "minisat-2.2.0",
    deps = select({
        "enable_minisat_220": ["//libraries:minisat-2.2.0"],
        "disable_minisat_220": [],
        "//conditions:default": DEFAULT_MINISAT_220
    })
)

cc_library(
    name = "minicraig-2.2.0",
    deps = select({
        "enable_minicraig_220": ["//libraries:minicraig-2.2.0"],
        "disable_minicraig_220": [],
        "//conditions:default": DEFAULT_MINICRAIG_220
    })
)


cc_library(
    name = "libfreitest",
    visibility = ["//visibility:public"],
    srcs = glob(
        include=["**/*.cpp", "**/*.hpp", "**/*.ipp"],
        exclude=["EntryPoint.cpp"]
    ),
    defines = [
        "HAS_BMC_SOLVER_EXPORT_HYS",
        "HAS_BMC_SOLVER_EXPORT_CIP",
        "HAS_BMC_SOLVER_DEBUG",
        "HAS_SAT_SOLVER_DIMACS",
        "HAS_SAT_SOLVER_DEBUG",
        # Slilence warning of global placeholders
        # (required by boost ptree implementation).
        "BOOST_BIND_GLOBAL_PLACEHOLDERS"
    ],
    copts = [
        # Enable includes relative to src
        "-iquote", "src",

        # Disable some warnings to keep sanity
        "-Wno-comment",
        "-Wno-c99-designator",
        "-Wno-vla-cxx-extension",

        # Disable warning about unsed command line arguments
        "-Wno-unknown-pragmas"
    ],
    linkopts = [
        # Boost libraries
        "-lboost_filesystem",
        "-lboost_regex",
        "-lboost_iostreams",

        # Add parallel support
        "-ltbb",
    ],
    linkstatic = True,
    alwayslink = True,
    deps = [
        # Libraries for implementation
        "//libraries:sparsepp",
        "//libraries:easyloggingpp",
        "//libraries:hat-trie",
        "//libraries:capstone",
        "//libraries:lua",

        # Solver for Solver Proxy
        ":ncip",
        ":pacose",
        ":cadical",
        ":cadicraig",
        ":crypto-minisat",
        ":inc-bmo-complete",
        ":ipasir-dynamic",
        ":ipasir-static",
        ":glucose-4.2.1",
        ":glucose-4.1",
        ":minisat-2.2.0",
        ":minicraig-2.2.0",

    ]
)

cc_binary(
    name = "freitest",
    srcs = [ "EntryPoint.cpp" ],
    copts = [
        # Enable includes relative to src
        "-iquote", "src",

        # Disable some warnings to keep sanity
        "-Wno-unused-variable",
        "-Wno-unused-but-set-variable",
        "-Wno-comment",

        # Disable warning about unsed command line arguments
        "-Wno-unused-command-line-argument",
        "-Wno-unknown-warning-option",
        "-Wno-unknown-pragmas"
    ],
    linkopts = [
        # Add automatic stack trace for segmentation faults / assertions
        # This is currently handled by easylogging++
        # "-lSegFault",
    ],
    linkstatic = True,
    deps = [ "//src:libfreitest" ]
)
