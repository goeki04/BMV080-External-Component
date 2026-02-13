Import("env")
import os

# Pfad zu diesem Script ermitteln
script_dir = os.path.dirname(os.path.abspath(__file__))

# Pfade zu den statischen Bibliotheken
lib_postProcessor = os.path.join(script_dir, "lib_postProcessor.a")
lib_bmv080 = os.path.join(script_dir, "lib_bmv080.a")

# Als Linkflags hinzufügen (ohne -l Präfix!)
env.Append(
    LINKFLAGS=[
        lib_postProcessor,
        lib_bmv080
    ]
)

print(f"[BMV080] Added libraries: {lib_postProcessor}, {lib_bmv080}")
