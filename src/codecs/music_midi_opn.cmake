option(USE_MIDI_OPNMIDI    "Build with libOPNMIDI OPN2 Emulator based MIDI sequencer support (GPL)" ON)
if(USE_MIDI_OPNMIDI AND MIXERX_GPL)
    option(USE_MIDI_OPNMIDI_DYNAMIC "Use dynamical loading of libOPNMIDI library" OFF)

    if(USE_SYSTEM_AUDIO_LIBRARIES)
        find_package(OPNMIDI QUIET)
        message("OPNMIDI: [${OPNMIDI_FOUND}] ${OPNMIDI_INCLUDE_DIRS} ${OPNMIDI_LIBRARIES}")
        if(USE_MIDI_OPNMIDI_DYNAMIC)
            list(APPEND SDL_MIXER_DEFINITIONS -DOPNMIDI_DYNAMIC=\"${OPNMIDI_DYNAMIC_LIBRARY}\")
            message("Dynamic libOPNMIDI: ${OPNMIDI_DYNAMIC_LIBRARY}")
        endif()

        if(OPNMIDI_FOUND)
            cpp_needed(${SDLMixerX_SOURCE_DIR}/cmake/tests/cpp_needed/opnmidi.c
                ""
                ${OPNMIDI_INCLUDE_DIRS}
                "${OPNMIDI_LIBRARIES};${M_LIBRARY}"
                STDCPP_NEEDED
            )
        endif()

    else()
        if(DOWNLOAD_AUDIO_CODECS_DEPENDENCY)
            set(OPNMIDI_LIBRARIES OPNMIDI${MIX_DEBUG_SUFFIX})
        else()
            find_library(OPNMIDI_LIBRARIES NAMES OPNMIDI HINTS "${AUDIO_CODECS_INSTALL_PATH}/lib")
        endif()
        if(OPNMIDI_LIBRARIES)
            set(OPNMIDI_FOUND 1)
            set(STDCPP_NEEDED 1) # Statically linking OPNMIDI which is C++ library
        endif()
        set(OPNMIDI_INCLUDE_DIRS "${AUDIO_CODECS_PATH}/libOPNMIDI/include")
    endif()

    if(OPNMIDI_FOUND)
        message("== using OPNMIDI (GPLv3+) ==")
        setLicense(LICENSE_GPL_3p)
        list(APPEND SDL_MIXER_DEFINITIONS -DMUSIC_MID_OPNMIDI)
        set(LIBMATH_NEEDED 1)
        list(APPEND SDL_MIXER_INCLUDE_PATHS ${OPNMIDI_INCLUDE_DIRS})
        if(NOT USE_SYSTEM_AUDIO_LIBRARIES OR NOT USE_MIDI_OPNMIDI_DYNAMIC)
            list(APPEND SDLMixerX_LINK_LIBS ${OPNMIDI_LIBRARIES})
        endif()
        list(APPEND SDLMixerX_SOURCES
            ${CMAKE_CURRENT_LIST_DIR}/music_midi_opn.c
            ${CMAKE_CURRENT_LIST_DIR}/music_midi_opn.h
        )
        appendMidiFormats("MIDI;RIFF MIDI;XMI;MUS")
    else()
        message("-- skipping OPNMIDI --")
    endif()
endif()
