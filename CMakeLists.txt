cmake_minimum_required(VERSION 3.16)

project(
  grs_fft
  VERSION 0.0.3
  DESCRIPTION "Real-time FFT calculator for SpaceLab's Ground Station"
  LANGUAGES C CXX
)

include(GNUInstallDirs)

# ------------------------------------------------------------------
# CPM bootstrap (before any CPMAddPackage)
# ------------------------------------------------------------------
include(cmake/CPM.cmake OPTIONAL)
if(NOT COMMAND CPMAddPackage)
  include(FetchContent)
  FetchContent_Declare(
    CPM
    GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake.git
    GIT_TAG v0.43.1
  )
  FetchContent_MakeAvailable(CPM)
  include(${cpm_SOURCE_DIR}/cmake/CPM.cmake)
endif()

# FFTW 3.3.11 declares compatibility with an older CMake policy set. CMake 4
# requires a policy-version floor when configuring dependencies that old.
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0)
  set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

# Avoid building FFTW's tests and optional precision/thread variants.
set(BUILD_TESTS OFF CACHE BOOL "Build FFTW tests" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build dependencies as shared libraries" FORCE)
set(DISABLE_FORTRAN ON CACHE BOOL "Disable FFTW Fortran wrappers" FORCE)
set(ENABLE_FLOAT OFF CACHE BOOL "Build single-precision FFTW" FORCE)
set(ENABLE_LONG_DOUBLE OFF CACHE BOOL "Build long-double FFTW" FORCE)
set(ENABLE_QUAD_PRECISION OFF CACHE BOOL "Build quad-precision FFTW" FORCE)
set(ENABLE_THREADS OFF CACHE BOOL "Build threaded FFTW" FORCE)
set(ENABLE_OPENMP OFF CACHE BOOL "Build OpenMP FFTW" FORCE)

CPMAddPackage(
  NAME fftw
  VERSION 3.3.11
  URL https://www.fftw.org/fftw-3.3.11.tar.gz
  URL_HASH SHA256=5630c24cdeb33b131612f7eb4b1a9934234754f9f388ff8617458d0be6f239a1
  EXCLUDE_FROM_ALL YES
  SYSTEM YES
)

# The upstream target only publishes its installed include directory.
target_include_directories(
  fftw3
  INTERFACE "$<BUILD_INTERFACE:${fftw_SOURCE_DIR}/api>"
)

# Some GNU toolchains need the static archive index refreshed after FFTW's
# generated codelets are collected.
if(CMAKE_RANLIB)
  add_custom_target(
    fftw3_reindex
    COMMAND "${CMAKE_RANLIB}" "$<TARGET_FILE:fftw3>"
    DEPENDS fftw3
    VERBATIM
  )
endif()

# Build only the static ZeroMQ library without its optional components.
set(BUILD_SHARED OFF CACHE BOOL "Build shared libzmq" FORCE)
set(BUILD_STATIC ON CACHE BOOL "Build static libzmq" FORCE)
set(WITH_LIBSODIUM OFF CACHE BOOL "Build libzmq with libsodium" FORCE)
set(ENABLE_CURVE OFF CACHE BOOL "Enable libzmq CURVE security" FORCE)
set(ENABLE_DRAFTS OFF CACHE BOOL "Enable draft libzmq APIs" FORCE)
set(ENABLE_WS OFF CACHE BOOL "Enable libzmq WebSocket transport" FORCE)
set(WITH_TLS OFF CACHE BOOL "Enable TLS for libzmq WebSockets" FORCE)
set(WITH_LIBBSD OFF CACHE BOOL "Build libzmq with libbsd" FORCE)
set(WITH_DOCS OFF CACHE BOOL "Build libzmq HTML documentation" FORCE)
set(WITH_DOC OFF CACHE BOOL "Build libzmq reference documentation" FORCE)
set(ENABLE_CPACK OFF CACHE BOOL "Enable libzmq packaging" FORCE)

CPMAddPackage(
  NAME libzmq
  VERSION 4.3.5
  GITHUB_REPOSITORY zeromq/libzmq
  GIT_TAG v4.3.5
  EXCLUDE_FROM_ALL YES
  SYSTEM YES
)

add_executable(grs_fft main.c)
if(TARGET fftw3_reindex)
  add_dependencies(grs_fft fftw3_reindex)
endif()
target_compile_features(grs_fft PRIVATE c_std_99)
set_target_properties(grs_fft PROPERTIES C_EXTENSIONS ON LINKER_LANGUAGE CXX)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(
    grs_fft
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wpointer-arith
    -Wcast-qual
    -Wstrict-prototypes
  )
endif()
target_link_libraries(grs_fft PRIVATE fftw3 libzmq-static m)

install(TARGETS grs_fft RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
