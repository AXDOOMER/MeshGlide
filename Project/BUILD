cc_library(
    name = "network",
    srcs = ["network.cpp"],
    hdrs = ["network.h"],
)

cc_library(
    name = "viewdraw",
    srcs = ["viewdraw.cpp"],
    hdrs = ["viewdraw.h"],
    deps = [
        ":things",
        ":cache",
        ":level"
    ],
)

cc_library(
    name = "cache",
    srcs = ["cache.cpp"],
    hdrs = ["cache.h"],
    deps = [
        ":texture",
    ],
)


cc_library(
    name = "things",
    srcs = ["things.cpp"],
    hdrs = ["things.h"],
    deps = [
        ":texture",
        ":cache"
    ],
)

cc_library(
    name = "texture",
    srcs = ["texture.cpp"],
    hdrs = ["texture.h"],
    deps = [
        ":vecmath",
    ],
)

cc_library(
    name = "vecmath",
    srcs = ["vecmath.cpp"],
    hdrs = ["vecmath.h"],
)


cc_library(
    name = "level",
    srcs = ["level.cpp"],
    hdrs = ["level.h"],
    deps = [
        ":vecmath",
        ":things",
        # ":physics",
        ":random"
    ],
)

cc_library(
    name = "physics",
    srcs = ["physics.cpp"],
    hdrs = ["physics.h"],
    deps = [
        ":things",
        ":level"
    ],
)

cc_library(
    name = "random",
    srcs = ["random.cpp"],
    hdrs = ["random.h"],
)


cc_library(
    name = "command",
    srcs = ["command.cpp"],
    hdrs = ["command.h"],
)

cc_library(
    name = "events",
    srcs = ["events.cpp"],
    hdrs = ["events.h"],
    deps = [
        ":viewdraw",
    ],
)

cc_library(name="glfw", linkopts=["-lglfw"])

cc_library(name="sdl2", linkopts=["-lSDL2"])

cc_library(name="sdl2_image", linkopts=["-lSDL2_image"])

cc_library(name="zmq", linkopts=["-lzmq"])

cc_library(name="gl", linkopts=["-lGL"])

cc_library(name="glu", linkopts=["-lGLU"])

cc_binary(
    name = "MeshGlide",
    srcs = ["main.cpp"],
    deps = [
        ":network",
	":viewdraw",
        ":things",
        ":command",
        ":physics",
        ":events",
        ":glfw",
        ":sdl2",
        ":sdl2_image",
        ":zmq",
        ":gl",
        ":glu"
    ],
)
