package(default_visibility = ["//visibility:public"])

cc_library(
    name = "halt_test_play",
    srcs = ["halt_test_play.cpp"],
    hdrs = ["halt_test_play.h"],
    deps = [
        "//shared/parameter:cpp_configs",
        "//software/ai/hl/stp/play",
        "//software/ai/hl/stp/tactic/stop:stop_tactic",
        "//software/util/generic_factory",
    ],
    alwayslink = True,
)

cc_library(
    name = "move_test_play",
    srcs = ["move_test_play.cpp"],
    hdrs = ["move_test_play.h"],
    deps = [
        "//shared/parameter:cpp_configs",
        "//software/ai/hl/stp/play",
        "//software/ai/hl/stp/tactic/move:move_tactic",
        "//software/util/generic_factory",
    ],
    alwayslink = True,
)
