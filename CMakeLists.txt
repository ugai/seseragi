cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 23)

project(seseragi VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# Vcpkg ports
include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
find_package(Alembic CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(Imath CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# External git submodules
add_subdirectory(external/nativefiledialog-extended)
include_directories(external/strconv)

# Slint
find_package(Slint QUIET)
if (NOT Slint_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    # `release/1` will auto-upgrade to the latest Slint >= 1.0.0 and < 2.0.0
    # `release/1.0` will auto-upgrade to the latest Slint >= 1.0.0 and < 1.1.0
    GIT_TAG release/1
    SOURCE_SUBDIR api/cpp
  )
  FetchContent_MakeAvailable(Slint)
endif (NOT Slint_FOUND)

file(GLOB_RECURSE SOURCES src/*.cpp)
add_executable(seseragi ${SOURCES})
target_include_directories(seseragi PRIVATE include)
target_sources(seseragi PRIVATE src/seseragi.manifest)

# Vcpkg ports
target_link_libraries(seseragi PRIVATE Alembic::Alembic)
target_link_libraries(seseragi PRIVATE cxxopts::cxxopts)
target_link_libraries(seseragi PRIVATE Imath::Imath Imath::ImathConfig)
target_link_libraries(seseragi PRIVATE spdlog::spdlog)

# Native File Dialog Extended
target_link_libraries(seseragi PRIVATE nfd)

# Slint
slint_target_sources(seseragi ui/app_window.slint)
target_link_libraries(seseragi PRIVATE Slint::Slint)
# On Windows, copy the Slint DLL next to the application binary so that it's found.
if (WIN32)
  add_custom_command(TARGET seseragi POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:seseragi> $<TARGET_FILE_DIR:seseragi> COMMAND_EXPAND_LISTS)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
