package(default_visibility = ["//visibility:public"])

cc_library(
    name = "primitive_executor",
    srcs = ["primitive_executor.cpp"],
    hdrs = ["primitive_executor.h"],
    deps = [
        "//proto:tbots_cc_proto",
        "//proto/primitive:primitive_msg_factory",
        "//shared:constants",
        "//software/math:math_functions",
        "//software/world",
    ],
)

cc_library(
    name = "thunderloop",
    srcs = ["thunderloop.cpp"],
    hdrs = ["thunderloop.h"],
    deps = [
        ":primitive_executor",
        "//proto:tbots_cc_proto",
        "//software/jetson_nano/services:motor",
    ],
)

cc_binary(
    name = "thunderloop_main",
    srcs = ["thunderloop_main.cpp"],
    deps = [
        ":thunderloop",
        "//shared:constants",
    ],
)
