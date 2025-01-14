
# This file downloads, configures and build AudioCodecs dependencies package.
#
# Output Variables:
# AUDIO_CODECS_INSTALL_DIR      The install directory
# AUDIO_CODECS_REPOSITORY_PATH  The reposotory directory

# Require ExternalProject and GIT!
include(ExternalProject)
find_package(Git REQUIRED)

# Posttible Input Vars:
# <None>

# SET OUTPUT VARS
#set(AUDIO_CODECS_INSTALL_DIR ${CMAKE_BINARY_DIR}/external/install)
set(AUDIO_CODECS_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(AUDIO_CODECS_REPOSITORY_PATH ${CMAKE_BINARY_DIR}/external/AudioCodecs)

#set(AUDIO_CODECS_REPO_PATH "" CACHE PATH "Path to the AudioCodecs dependencies pack")

set(AUDIO_CODECS_SDL2_GIT_BRANCH "" CACHE STRING "GIT commit hash for SDL2 repository")

option(WITH_SDL2_WASAPI "Enable WASAPI audio output support for Windows build of SDL2" ON)
if(WIN32)
    message("== SDL Mixer X: WITH_SDL2_WASAPI = ${WITH_SDL2_WASAPI}")
    set(SDL2_WASAPI_FLAG "-DWITH_SDL2_WASAPI=${WITH_SDL2_WASAPI}")
endif()

set(SDL2_TAGS)
if(AUDIO_CODECS_SDL2_GIT_BRANCH)
    set(SDL2_TAGS ${SDL2_TAGS} "-DSDL2_GIT_BRANCH=${AUDIO_CODECS_SDL2_GIT_BRANCH}")
endif()
if(USE_SYSTEM_SDL2)
    set(AUDIO_CODECS_DOWNLOAD_SDL OFF)
else()
    set(AUDIO_CODECS_DOWNLOAD_SDL ON)
endif()

set(ADLMIDI_FLAGS)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    set(ADLMIDI_FLAGS "${ADLMIDI_FLAGS} -DADLMIDI_USE_DOSBOX_EMULATOR=ON")
endif()

set(APPLE_FLAGS)
if(APPLE)
    set(APPLE_FLAGS
        "-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
        "-DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}"
    )
endif()


option(AUDIOCODECS_BUILD_OGG_VORBIS "Build OGG Vorbis at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_FLAC "Build FLAC at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_MPG123 "Build libMPG123 MP3 decoder at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_GME "Build GME at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_TIMIDITYSDL "Build Timidity-SDL at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_MODPLUG "Build libModPlug at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_LIBXMP "Build libXMP at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_OPUS "Build Opus at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_FLUIDLITE "Build FluidLite at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_ADLMIDI "Build libADLMIDI at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_OPNMIDI "Build libOPNMIDI at AudioCodecs" ON)
option(AUDIOCODECS_BUILD_EDMIDI "Build libEDMIDI at AudioCodecs" ON)

option(AUDIO_CODECS_BUILD_LOCAL_SDL2_SHARED "Build shared version of SDL2 from local AudioCodecs' copy" ON)

ExternalProject_Add(
    AudioCodecs
    PREFIX ${CMAKE_BINARY_DIR}/external/AudioCodecs
    GIT_REPOSITORY https://github.com/WohlSoft/AudioCodecs.git
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_DEBUG_POSTFIX=${MIX_DEBUG_SUFFIX}"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${AUDIO_CODECS_INSTALL_DIR}"
        "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
        "-DDOWNLOAD_SDL2_DEPENDENCY=${AUDIO_CODECS_DOWNLOAD_SDL}"
        "-DUSE_LOCAL_SDL2=${AUDIO_CODECS_BUILD_LOCAL_SDL2}"
        "-DBUILD_SDL2_SHARED=${AUDIO_CODECS_BUILD_LOCAL_SDL2_SHARED}"
        ${SDL2_WASAPI_FLAG}
        ${SDL2_TAGS}
        ${ADLMIDI_FLAGS}
        ${APPLE_FLAGS}
        # Library enable/disable
        "-DBUILD_OGG_VORBIS=${AUDIOCODECS_BUILD_OGG_VORBIS}"
        "-DBUILD_FLAC=${AUDIOCODECS_BUILD_FLAC}"
        "-DBUILD_MPG123=${AUDIOCODECS_BUILD_MPG123}"
        "-DBUILD_GME=${AUDIOCODECS_BUILD_GME}"
        "-DBUILD_TIMIDITYSDL=${AUDIOCODECS_BUILD_TIMIDITYSDL}"
        "-DBUILD_MODPLUG=${AUDIOCODECS_BUILD_MODPLUG}"
        "-DBUILD_LIBXMP=${AUDIOCODECS_BUILD_LIBXMP}"
        "-DBUILD_OPUS=${AUDIOCODECS_BUILD_OPUS}"
        "-DBUILD_FLUIDLITE=${AUDIOCODECS_BUILD_FLUIDLITE}"
        "-DBUILD_ADLMIDI=${AUDIOCODECS_BUILD_ADLMIDI}"
        "-DBUILD_OPNMIDI=${AUDIOCODECS_BUILD_OPNMIDI}"
        "-DBUILD_EDMIDI=${AUDIOCODECS_BUILD_EDMIDI}"
)

if(WIN32) # For VB6-targeted build only
    if(AUDIO_CODECS_BUILD_LOCAL_SDL2_SHARED OR NOT MINGW)
        set(AUDIOCODECS_SDL2_LIB_NAME SDL2-static${MIX_DEBUG_SUFFIX})
    else()
        set(AUDIOCODECS_SDL2_LIB_NAME SDL2${MIX_DEBUG_SUFFIX})
    endif()
endif()

message("AudioCodecs can see SDL2 is stored in ${SDL2_REPO_PATH}...")
