package(default_visibility = ["//visibility:public"])

#added platformio rules to make constants library compatible with arduino board build tool
load(
    "@platformio_rules//platformio:platformio.bzl",
    "platformio_library",
)

cc_library(
    name = "constants",
    srcs = [],
    hdrs = ["constants.h"],
    deps = [],
    alwayslink = True,
)

platformio_library(
    name = "constants_platformio",
    hdr = "constants.h",
)

cc_library(
    name = "robot_constants",
    srcs = [
        "2015_robot_constants.c",
        "2021_robot_constants.c",
    ],
    hdrs = [
        "2015_robot_constants.h",
        "2021_robot_constants.h",
        "robot_constants.h",
    ],
    deps = [
        ":constants",
        "//firmware/shared/math:tbots_math",
    ],
)
