# CMake 2.8.11 and above required for linking to qtmain.lib library on Windows
cmake_minimum_required(VERSION 2.8.11)
set(PROJECT_NAME "maya_standalone_test")
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Set compiler flags for each platform
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Using Clang on XCode
    set(CMAKE_C_FLAGS "-DBits64_"
                      "-m64"
                      "-DUNIX"
                      "-D_BOOL"
                      "-DLINUX"
                      "-DFUNCPROTO"
                      "-D_GNU_SOURCE"
                      "-DLINUX_64"
                      "-fPIC"
                      "-fno-strict-aliasing"
                      "-DREQUIRE_IOSTREAM"
                      "-O3"
                      "-Wall"
                      "-Wno-multichar"
                      "-Wno-comment"
                      "-Wno-sign-compare"
                      "-funsigned-char"
                      "-pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} \
                        -Wall \
                        -Wno-deprecated \
                        -Wno-reorder \
                        -ftemplate-depth-25 \
                        -fno-gnu-keywords")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb -glldb")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
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
                        -Wall \
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

set(DEVKIT "${MAYA_LOCATION}/devkit")
set(DEVKIT_INCLUDE "${MAYA_LOCATION}/include")
set(DEVKIT_ALEMBIC_INCDIR "${DEVKIT}/Alembic/include")
set(DEVKIT_ALEMBIC_LIBDIR "${DEVKIT}/Alembic/lib")

# Add project sources
add_subdirectory(src)

find_package(Maya REQUIRED)

include_directories(
  ${MAYA_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/include
)

set(EXECUTABLE_NAME "maya_standalone_test_app")
add_executable (${EXECUTABLE_NAME} ${PROJECT_SOURCES})
link_directories(${MAYA_LIBRARY_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${MAYA_LIBRARIES})

install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
