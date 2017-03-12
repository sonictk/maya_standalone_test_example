# Add all source files
file (GLOB TEST_SOURCES
    "*.cpp"
    "*.c"
)
# Export project sources to parent CMakeLists file
set (TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
