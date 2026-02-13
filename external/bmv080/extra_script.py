Import("env")
import os

script_dir = os.path.dirname(os.path.abspath(__file__))

lib_postProcessor = os.path.join(script_dir, "lib_postProcessor.a")
lib_bmv080 = os.path.join(script_dir, "lib_bmv080.a")

env.Append(
    LINKFLAGS=[
        lib_postProcessor,
        lib_bmv080
    ]
)

print(f"[BMV080] Added libraries: {lib_postProcessor}, {lib_bmv080}")
