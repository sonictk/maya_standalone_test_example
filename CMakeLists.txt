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
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /ZI /Od /EHsc")
endif()

# Add project sources
add_subdirectory(src)

# Add test sources
add_subdirectory(tests)

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
set(TEST_EXECUTABLE_NAME "unittests_${PROJECT_NAME}")
add_executable (${EXECUTABLE_NAME} ${PROJECT_SOURCES})
add_executable (${TEST_EXECUTABLE_NAME} ${TEST_SOURCES})

# Set preprocessor definitions
set_target_properties(${EXECUTABLE_NAME} ${TEST_EXECUTABLE_NAME} PROPERTIES
    COMPILE_DEFINITIONS "${MAYA_COMPILE_DEFINITIONS}")

# Link the executable to the libraries
message(STATUS "Linking to Maya libraries at: ${MAYA_LIBRARY_DIR}")
target_link_libraries(${EXECUTABLE_NAME} ${MAYA_LIBRARIES})
target_link_libraries(${TEST_EXECUTABLE_NAME} ${MAYA_LIBRARIES})

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/bin)
install(TARGETS ${EXECUTABLE_NAME} ${TEST_EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})

# For standalone linking, run post-build commands to copy necessary dylibs/OSX frameworks 
# to the executable folder
# If on Windows, because there is extra stuff in Python that relies on the Maya libs directory specifically, 
# create the .bat file for setting the environment for end-users to run the tests with
if(WIN32)
    file(TO_NATIVE_PATH ${MAYA_LOCATION} MAYA_LOCATION_WIN)
    configure_file("${PROJECT_SOURCE_DIR}/scripts/launch_maya_standalone_test.bat.in"
                   "${CMAKE_INSTALL_PREFIX}/launch_maya_standalone_test.bat" NEWLINE_STYLE CRLF)

# If on OSX, since @rpath linking/framework linking is horrendously overcomplicated, copy over the .dylibs 
# and framework files to the application directory as well
elseif(APPLE)
    file(GLOB MAYA_DYLIBS 
        "${MAYA_LIBRARY_DIR}/*.dylib"
        "${MAYA_LIBRARY_DIR}/QtCore"
        "${MAYA_LIBRARY_DIR}/QtDeclarative"
        "${MAYA_LIBRARY_DIR}/QtCore"
        "${MAYA_LIBRARY_DIR}/QtMultimedia"
        "${MAYA_LIBRARY_DIR}/QtHelp"
        "${MAYA_LIBRARY_DIR}/QtGui"
        "${MAYA_LIBRARY_DIR}/QtDesignerComponents"
        "${MAYA_LIBRARY_DIR}/QtDesigner"
        "${MAYA_LIBRARY_DIR}/QtSvg"
        "${MAYA_LIBRARY_DIR}/QtSql"
        "${MAYA_LIBRARY_DIR}/QtScriptTools"
        "${MAYA_LIBRARY_DIR}/QtScript"
        "${MAYA_LIBRARY_DIR}/QtOpenGL"
        "${MAYA_LIBRARY_DIR}/QtNetwork"
        "${MAYA_LIBRARY_DIR}/shiboken"
        "${MAYA_LIBRARY_DIR}/Render"
        "${MAYA_LIBRARY_DIR}/QtXmlPatterns"
        "${MAYA_LIBRARY_DIR}/QtXml"
        "${MAYA_LIBRARY_DIR}/QtWebKit"
        "${MAYA_LIBRARY_DIR}/phonon")
    foreach(DYLIB ${MAYA_DYLIBS})
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E 
            copy_if_different "${DYLIB}" "${CMAKE_INSTALL_PREFIX}"
            COMMENT "Copying ${DYLIB} to installation directory...")
    endforeach()

    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${MAYA_LIBRARY_DIR}/plug-ins" "${CMAKE_INSTALL_PREFIX}/plug-ins"
        COMMENT "Copying Maya plug-ins to installation directory...")

    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${MAYA_LIBRARY_DIR}/rendererDesc" "${CMAKE_INSTALL_PREFIX}/rendererDesc"
        COMMENT "Copying Maya renderer XML configuration files to installation directory...")

    get_filename_component(INSTALL_PARENT_DIR ${CMAKE_INSTALL_PREFIX} DIRECTORY)
    set(FRAMEWORKS_INSTALL_DIR "${INSTALL_PARENT_DIR}/Frameworks")
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${MAYA_FRAMEWORK_DIR}" "${FRAMEWORKS_INSTALL_DIR}"
        COMMENT "Copying ${MAYA_FRAMEWORK_DIR} to ${FRAMEWORKS_INSTALL_DIR}...")
endif()
