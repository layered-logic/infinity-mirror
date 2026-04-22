# Layered Logic Infinity Mirror — standard variant (Pro)
#
# Scaffold only. Real component registration lands when core/ modules
# have implementations.

idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES
        # core components — add as they come online
)
