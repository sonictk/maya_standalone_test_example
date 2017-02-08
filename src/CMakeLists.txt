# Add all source files
file (GLOB PROJECT_SOURCES 
    "*.cpp"
    "*.c"
)
# Export project sources to parent CMakeLists file
set (PROJECT_SOURCES ${PROJECT_SOURCES} PARENT_SCOPE)
