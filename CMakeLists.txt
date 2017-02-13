# CMake 2.8.11 and above required for linking to qtmain.lib library on Windows
cmake_minimum_required(VERSION 2.8.11)
set(PROJECT_NAME "maya_standalone_test")
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Attempt to find existing installation of Maya and define variables
find_package(Maya REQUIRED)


# Add include search paths
include_directories(
  ${MAYA_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/include
)

# Set compiler flags for each platform
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Detected compiler as Clang on OSX...")
    # Using Clang on XCode
    set(CMAKE_FRAMEWORK_PATH "")
    set(CMAKE_CXX_FLAGS "-stdlib=libstdc++ \
                        -std=c++0x \
                        -fno-gnu-keywords \
                        -fpascal-strings \
                        -O3 \
                        -Wl,-executable_path,${MAYA_LOCATION}/Maya.app/Contents/MacOS \
                        -F${MAYA_LOCATION}/Maya.app/Contents/Frameworks \
                        -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk \
                        -headerpad_max_install_names \
                        -dynamic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb -glldb")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC on Linux
    set(CMAKE_C_FLAGS "-DBits64_ \
                      -m64 \
                      -DUNIX \
                      -D_BOOL \
                      -DLINUX \
                      -DFUNCPROTO \
                      -D_GNU_SOURCE \
                      -DLINUX_64 \
                      -fPIC \
                      -fno-strict-aliasing \
                      -DREQUIRE_IOSTREAM \
                      -O3 \
                      -Wall \
                      -Wno-multichar \
                      -Wno-comment \
                      -Wno-sign-compare \
                      -funsigned-char \
                      -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} \
                        -Wno-deprecated \
                        -Wno-reorder \
                        -ftemplate-depth-25 \
                        -fno-gnu-keywords")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    # TODO: Need to find the compiler flags for Visual Studio compiler
    set(CMAKE_CXX_FLAGS "/MP")
    set(CMAKE_CXX_FLAGS_DEBUG "/MP /ZI /EHsc")
endif()

# Add project sources
add_subdirectory(src)

# Don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)
# Disable RPATH stripping during installation so that the binary will link correctly
# to the Maya libs
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable (${EXECUTABLE_NAME} ${PROJECT_SOURCES})

# Set preprocessor definitions
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS}")

# Link the executable to the libraries
link_directories(${MAYA_LIBRARY_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${MAYA_LIBRARIES})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${PROJECT_SOURCE_DIR}/bin)
