# CMake 2.8.11 and above required for linking to qtmain.lib library on Windows
# CMake 2.8.12 and above required for MACOSX_RPATH property
cmake_minimum_required(VERSION 2.8.12)
set(PROJECT_NAME "maya_standalone_test")

if(APPLE)
    message(STATUS "Setting MacOSX SDK...")
    set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk")
endif(APPLE)

project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Attempt to find existing installation of Maya and define variables
find_package(Maya REQUIRED)

# Add include search paths
include_directories(
  ${MAYA_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/include
)
message(STATUS "Including Maya headers from: ${MAYA_INCLUDE_DIR}")
message(STATUS "Including project headers from: ${PROJECT_SOURCE_DIR}/include")

if(APPLE)
    set(MAYA_FRAMEWORK_DIR "${MAYA_LOCATION}/Maya.app/Contents/Frameworks")
    message(STATUS "Setting OSX Framework path to: ${MAYA_FRAMEWORK_DIR}")
    set(CMAKE_MACOSX_RPATH TRUE)
    set(CMAKE_FIND_FRAMEWORK FIRST)
    set(CMAKE_FRAMEWORK_PATH "${MAYA_FRAMEWORK_DIR}")
endif(APPLE)

# Set compiler flags for each platform
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS "Detected compiler as Clang...")
    # Using Clang
    set(CMAKE_CXX_FLAGS "-stdlib=libstdc++ \
                        -std=c++0x \
                        -fno-gnu-keywords \
                        -fpascal-strings \
                        -pthread \
                        -O3 \
                        -dynamic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -gdwarf-4 -gmodules")
    set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libstdc++ \
                                -std=c++0x \
                                -fno-gnu-keywords \
                                -fpascal-strings \
                                -O3 \
                                -Wl, \
                                -headerpad_max_install_names \
                                -dynamic")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
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

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    set(CMAKE_CXX_FLAGS "/MP /GR /GS /W3 /Gy /Zc:wchar_t /Zc:forScope /Gm /O2 /Ob1 /fp:precise /GF /WX /nologo")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /ZI /EHsc")
endif()

# Add project sources
add_subdirectory(src)

# Don't skip the full RPATH for the build/install tree
set(CMAKE_SKIP_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

if(APPLE)
    message (STATUS "Setting OSX-specific settings...")
    set(CMAKE_INSTALL_NAME_DIR "@rpath")
    set(CMAKE_INSTALL_RPATH "${MAYA_LIBRARY_DIR};${MAYA_FRAMEWORK_DIR}")
endif(APPLE)

# Disable RPATH stripping during installation so that the binary will link correctly
# to the Maya libs
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable (${EXECUTABLE_NAME} ${PROJECT_SOURCES})

# Set preprocessor definitions
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS}")

# Link the executable to the libraries
message(STATUS "Linking to Maya libraries at: ${MAYA_LIBRARY_DIR}")
target_link_libraries(${EXECUTABLE_NAME} ${MAYA_LIBRARIES})

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

# If on Windows, copy over all the DLLs necessary for the standalone app to dynamically link against
if(WIN32)
    file(GLOB MAYA_DLLS "${MAYA_LOCATION}/bin/*.dll")
    foreach(DLL_FILE ${MAYA_DLLS})
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_FILE}" "${CMAKE_INSTALL_PREFIX}"
            COMMENT "Copying ${DLL_FILE} to installation directory...")
    endforeach(DLL_FILE)

# If on OSX, since @rpath linking/framework linking is horrendously overcomplicated, copy over the .dylibs 
# and framework files to the application directory as well
elseif(APPLE)
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${MAYA_LIBRARY_DIR}" "${CMAKE_INSTALL_PREFIX}"
        COMMENT "Copying ${MAYA_LIBRARY_DIR} to installation directory...")

    get_filename_component(INSTALL_PARENT_DIR ${CMAKE_INSTALL_PREFIX} DIRECTORY)
    set(FRAMEWORKS_INSTALL_DIR "${INSTALL_PARENT_DIR}/Frameworks")
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${MAYA_FRAMEWORK_DIR}" "${FRAMEWORKS_INSTALL_DIR}"
        COMMENT "Copying ${MAYA_FRAMEWORK_DIR} to ${FRAMEWORKS_INSTALL_DIR}...")
endif()
