/*
  SDL Mixer X:  An extended audio mixer library, forked from SDL_mixer
  Copyright (C) 2014-2022 Vitaly Novichkov <admin@wohlnet.ru>

  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SDL_MIXER_H_
#define SDL_MIXER_H_

#include "SDL_stdinc.h"
#include "SDL_rwops.h"
#include "SDL_audio.h"
#include "SDL_endian.h"
#include "SDL_version.h"
#include "begin_code.h"

/* Let applications recogonize which SDL Mixer edition is in use: Official or Extended fork by Wohlstand */
#define SDL_MIXER_X 1

#define MIXSDLCALL

#if defined(FORCE_STDCALLS) && defined(_WIN32)
#   define MIXCALL   __stdcall
#   define MIXCALLCC __stdcall
#else
#   define MIXCALL SDLCALL
#   define MIXCALLCC
#endif

#ifndef MIXERX_DEPRECATED
#   if defined(_MSC_VER) /* MSVC */
#       if _MSC_VER >= 1500 /* MSVC 2008 */
                            /*! Indicates that the following function is deprecated. */
#           define MIXERX_DEPRECATED(message) __declspec(deprecated(message))
#       endif
#   endif /* defined(_MSC_VER) */

#   if !defined(MIXERX_DEPRECATED)
#       if defined(__clang__)
#           if __has_extension(attribute_deprecated_with_message)
#               define MIXERX_DEPRECATED(message) __attribute__((deprecated(message)))
#           endif
#       elif defined(__GNUC__) /* not clang (gcc comes later since clang emulates gcc) */
#           if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#               define MIXERX_DEPRECATED(message) __attribute__((deprecated(message)))
#           elif (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#               define MIXERX_DEPRECATED(message) __attribute__((__deprecated__))
#           endif /* GNUC version */
#       endif /* __clang__ || __GNUC__ */
#   endif /* !defined(MIXERX_DEPRECATED) */
#endif

#if !defined(MIXERX_DEPRECATED)
#   define MIXERX_DEPRECATED(message)
#endif /* if !defined(MIXERX_DEPRECATED) */

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Printable format: "%d.%d.%d", MAJOR, MINOR, PATCHLEVEL
*/
#define SDL_MIXER_MAJOR_VERSION 2
#define SDL_MIXER_MINOR_VERSION 5
#define SDL_MIXER_PATCHLEVEL    1

/* This macro can be used to fill a version structure with the compile-time
 * version of the SDL_mixer library.
 */
#define SDL_MIXER_VERSION(X)                        \
{                                                   \
    (X)->major = SDL_MIXER_MAJOR_VERSION;           \
    (X)->minor = SDL_MIXER_MINOR_VERSION;           \
    (X)->patch = SDL_MIXER_PATCHLEVEL;              \
}

/* Backwards compatibility */
#define MIX_MAJOR_VERSION   SDL_MIXER_MAJOR_VERSION
#define MIX_MINOR_VERSION   SDL_MIXER_MINOR_VERSION
#define MIX_PATCHLEVEL      SDL_MIXER_PATCHLEVEL
#define MIX_VERSION(X)      SDL_MIXER_VERSION(X)

#if SDL_MIXER_MAJOR_VERSION < 3 && SDL_MAJOR_VERSION < 3
/**
 *  This is the version number macro for the current SDL_mixer version.
 *
 *  In versions higher than 2.9.0, the minor version overflows into
 *  the thousands digit: for example, 2.23.0 is encoded as 4300.
 *  This macro will not be available in SDL 3.x or SDL_mixer 3.x.
 *
 *  Deprecated, use SDL_MIXER_VERSION_ATLEAST or SDL_MIXER_VERSION instead.
 */
#define SDL_MIXER_COMPILEDVERSION \
    SDL_VERSIONNUM(SDL_MIXER_MAJOR_VERSION, SDL_MIXER_MINOR_VERSION, SDL_MIXER_PATCHLEVEL)
#endif /* SDL_MIXER_MAJOR_VERSION < 3 && SDL_MAJOR_VERSION < 3 */

/**
 *  This macro will evaluate to true if compiled with SDL_mixer at least X.Y.Z.
 */
#define SDL_MIXER_VERSION_ATLEAST(X, Y, Z) \
    ((SDL_MIXER_MAJOR_VERSION >= X) && \
     (SDL_MIXER_MAJOR_VERSION > X || SDL_MIXER_MINOR_VERSION >= Y) && \
     (SDL_MIXER_MAJOR_VERSION > X || SDL_MIXER_MINOR_VERSION > Y || SDL_MIXER_PATCHLEVEL >= Z))

/* This function gets the version of the dynamically linked SDL_mixer library.
   it should NOT be used to fill a version structure, instead you should
   use the SDL_MIXER_VERSION() macro.
 */
extern DECLSPEC const SDL_version * MIXCALL Mix_Linked_Version(void);

typedef enum
{
    MIX_INIT_FLAC   = 0x00000001,
    MIX_INIT_MOD    = 0x00000002,
    MIX_INIT_MP3    = 0x00000008,
    MIX_INIT_OGG    = 0x00000010,
    MIX_INIT_MID    = 0x00000020,
    MIX_INIT_OPUS   = 0x00000040
} MIX_InitFlags;

/* Loads dynamic libraries and prepares them for use.  Flags should be
   one or more flags from MIX_InitFlags OR'd together.
   It returns the flags successfully initialized, or 0 on failure.
 */
extern DECLSPEC int MIXCALL Mix_Init(int flags);

/* Unloads libraries loaded with Mix_Init */
extern DECLSPEC void MIXCALL Mix_Quit(void);


/* The default mixer has 8 simultaneous mixing channels */
#ifndef MIX_CHANNELS
#define MIX_CHANNELS    8
#endif

/* Good default values for a PC soundcard */
#define MIX_DEFAULT_FREQUENCY   44100
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define MIX_DEFAULT_FORMAT  AUDIO_S16LSB
#else
#define MIX_DEFAULT_FORMAT  AUDIO_S16MSB
#endif
#define MIX_DEFAULT_CHANNELS    2
#define MIX_MAX_VOLUME          SDL_MIX_MAXVOLUME /* Volume of a chunk */

/* The internal format for an audio chunk */
typedef struct Mix_Chunk {
    int allocated;
    Uint8 *abuf;
    Uint32 alen;
    Uint8 volume;       /* Per-sample volume, 0-128 */
} Mix_Chunk;

/* The different fading types supported */
typedef enum {
    MIX_NO_FADING,
    MIX_FADING_OUT,
    MIX_FADING_IN
} Mix_Fading;

/* These are types of music files (not libraries used to load them) */
typedef enum {
    MUS_NONE,
    MUS_CMD,
    MUS_WAV,
    MUS_MOD,
    MUS_MID,
    MUS_OGG,
    MUS_MP3,
    MUS_MP3_MAD_UNUSED,
    MUS_FLAC,
    MUS_MODPLUG_UNUSED,
    MUS_OPUS,
    /* MixerX specific codecs starts with 100 */
    MUS_GME = 100,
    /* Special cases to play formats IMF, MUS, or XMI are can't be played without specific MIDI libraries */
    MUS_ADLMIDI = 200,
    MUS_OPNMIDI,
    MUS_FLUIDLITE,
    MUS_EDMIDI,
    MUS_NATIVEMIDI
} Mix_MusicType;

typedef enum {
    MIDI_ADLMIDI,
    MIDI_Native,
    MIDI_Timidity,
    MIDI_OPNMIDI,
    MIDI_Fluidsynth,
    MIDI_EDMIDI,
    MIDI_ANY,
    MIDI_KnownDevices   /* Count of MIDI device types */
} Mix_MIDI_Device;

/* The field with a typo, kept for backward compatibility  */
#define MIDI_KnuwnDevices MIDI_KnownDevices

/* Volume model type in the ADLMIDI */
typedef enum {
    ADLMIDI_VM_AUTO,
    ADLMIDI_VM_GENERIC,
    ADLMIDI_VM_NATIVE,
    ADLMIDI_VM_DMX,
    ADLMIDI_VM_APOGEE,
    ADLMIDI_VM_9X,
    ADLMIDI_VM_DMX_FIXED,
    ADLMIDI_VM_APOGEE_FIXED,
    ADLMIDI_VM_AIL,
    ADLMIDI_VM_9X_GENERIC_FM,
    ADLMIDI_VM_HMI,
    ADLMIDI_VM_HMI_OLD,
    /* Deprecated */
    ADLMIDI_VM_CMF = ADLMIDI_VM_NATIVE
} Mix_ADLMIDI_VolumeModel;

/* OPL3 chip emulators for ADLMIDI */
typedef enum {
    ADLMIDI_OPL3_EMU_DEFAULT = -1,
    ADLMIDI_OPL3_EMU_NUKED = 0,
    ADLMIDI_OPL3_EMU_NUKED_1_7_4,
    ADLMIDI_OPL3_EMU_DOSBOX,
    ADLMIDI_OPL3_EMU_OPAL,
    ADLMIDI_OPL3_EMU_JAVA
} Mix_ADLMIDI_Emulator;

/* Volume model type in the OPNMIDI */
typedef enum {
    OPNMIDI_VM_AUTO,
    OPNMIDI_VM_GENERIC,
    OPNMIDI_VM_NATIVE,
    OPNMIDI_VM_DMX,
    OPNMIDI_VM_APOGEE,
    OPNMIDI_VM_9X
} Mix_OPNMIDI_VolumeModel;

/* OPN2 chip emulators for OPNMIDI */
typedef enum {
    OPNMIDI_OPN2_EMU_DEFAULT = -1,
    OPNMIDI_OPN2_EMU_MAME_OPN2 = 0,
    OPNMIDI_OPN2_EMU_NUKED,
    OPNMIDI_OPN2_EMU_GENS,
    OPNMIDI_OPN2_EMU_GX, /* Caution: THIS emulator is inavailable by default */
    OPNMIDI_OPN2_EMU_NP2,
    OPNMIDI_OPN2_EMU_MAME_OPNA,
    OPNMIDI_OPN2_EMU_PMDWIN,
    /* Deprecated */
    OPNMIDI_OPN2_EMU_MIME = 0 /*!!!TYPO!!!*/
} Mix_OPNMIDI_Emulator;

/* The internal format for a music chunk interpreted via codecs */
typedef struct _Mix_Music Mix_Music;

/* Open the mixer with a certain audio format */
extern DECLSPEC int MIXCALL Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);

/* Open the mixer with specific device and certain audio format */
extern DECLSPEC int MIXCALL Mix_OpenAudioDevice(int frequency, Uint16 format, int channels, int chunksize, const char* device, int allowed_changes);

/* Pause or resume the audio streaming */
extern DECLSPEC void MIXCALL Mix_PauseAudio(int pause_on);

/* Dynamically change the number of channels managed by the mixer.
   If decreasing the number of channels, the upper channels are
   stopped.
   This function returns the new number of allocated channels.
 */
extern DECLSPEC int MIXCALL Mix_AllocateChannels(int numchans);

/* Find out what the actual audio device parameters are.
   This function returns 1 if the audio has been opened, 0 otherwise.
 */
extern DECLSPEC int MIXCALL Mix_QuerySpec(int *frequency,Uint16 *format,int *channels);

/* Load a wave file or a music (.mod .s3m .it .xm) file
    IMPORTANT: To choice a track number of NSF, GBM, HES, etc file,
               you must append "|xxx" to end of file path for
               Mix_LoadMUS function.
               Where xxx - actual number of chip track, (from 0 to N-1)
               Examples: "file.nsf|12", "file.hes|2"
*/
extern DECLSPEC Mix_Chunk * MIXCALL Mix_LoadWAV_RW(SDL_RWops *src, int freesrc);
#define Mix_LoadWAV(file)   Mix_LoadWAV_RW(SDL_RWFromFile(file, "rb"), 1)
extern DECLSPEC Mix_Music * MIXCALL Mix_LoadMUS(const char *file);

/* Set the displayable filename used in cases of memory-read files */
extern DECLSPEC void MIXCALL Mix_SetMusicFileName(Mix_Music *music, const char *file);

/* Load a music file from an SDL_RWop object
 * Matt Campbell (matt@campbellhome.dhs.org) April 2000 */
extern DECLSPEC Mix_Music * MIXCALL Mix_LoadMUS_RW(SDL_RWops *src, int freesrc);

/* Load a music file from an SDL_RWop object with custom arguments (trackID for GME or settings for a MIDI playing)
 * Arguments are taking no effect for file formats which are not supports extra arguments.
  */
extern DECLSPEC Mix_Music *MIXCALL Mix_LoadMUS_RW_ARG(SDL_RWops *src, int freesrc, const char *args);

/* Load a music file from an SDL_RWop object with custom trackID for GME.
 * trackID argument takes no effect for non-NSF,HES,GBM,etc. file formats.
 * Default value should be 0
 */
extern DECLSPEC Mix_Music *MIXCALL Mix_LoadMUS_RW_GME(SDL_RWops *src, int freesrc, int trackID);

/* Load a music file from an SDL_RWop object assuming a specific format */
extern DECLSPEC Mix_Music * MIXCALL Mix_LoadMUSType_RW(SDL_RWops *src, Mix_MusicType type, int freesrc);

/* Load a music file from an SDL_RWop object assuming a specific format
   with custom arguments (trackID for GME or settings for a MIDI playing) */
extern DECLSPEC Mix_Music * MIXCALL Mix_LoadMUSType_RW_ARG(SDL_RWops *src, Mix_MusicType type, int freesrc, const char *args);

/* Load a wave file of the mixer format from a memory buffer */
extern DECLSPEC Mix_Chunk * MIXCALL Mix_QuickLoad_WAV(Uint8 *mem);

/* Load raw audio data of the mixer format from a memory buffer */
extern DECLSPEC Mix_Chunk * MIXCALL Mix_QuickLoad_RAW(Uint8 *mem, Uint32 len);

/* Free an audio chunk previously loaded */
extern DECLSPEC void MIXCALL Mix_FreeChunk(Mix_Chunk *chunk);
extern DECLSPEC void MIXCALL Mix_FreeMusic(Mix_Music *music);

/* Set the music to be self-destroyed once playback has get finished.
 Works only on multi-stream sub-system. */
extern DECLSPEC int MIXCALL Mix_SetFreeOnStop(Mix_Music *music, int free_on_stop);

/* Get a list of chunk/music decoders that this build of SDL_mixer provides.
   This list can change between builds AND runs of the program, if external
   libraries that add functionality become available.
   You must successfully call Mix_OpenAudio() before calling these functions.
   This API is only available in SDL_mixer 1.2.9 and later.

   // usage...
   int i;
   const int total = Mix_GetNumChunkDecoders();
   for (i = 0; i < total; i++)
       printf("Supported chunk decoder: [%s]\n", Mix_GetChunkDecoder(i));

   Appearing in this list doesn't promise your specific audio file will
   decode...but it's handy to know if you have, say, a functioning Timidity
   install.

   These return values are static, read-only data; do not modify or free it.
   The pointers remain valid until you call Mix_CloseAudio().
*/
extern DECLSPEC int MIXCALL Mix_GetNumChunkDecoders(void);
extern DECLSPEC const char * MIXCALL Mix_GetChunkDecoder(int index);
extern DECLSPEC SDL_bool MIXCALL Mix_HasChunkDecoder(const char *name);
extern DECLSPEC int MIXCALL Mix_GetNumMusicDecoders(void);
extern DECLSPEC const char * MIXCALL Mix_GetMusicDecoder(int index);
extern DECLSPEC SDL_bool MIXCALL Mix_HasMusicDecoder(const char *name);

/* Find out the music format of a mixer music, or the currently playing
   music, if 'music' is NULL.
*/
extern DECLSPEC Mix_MusicType MIXCALL Mix_GetMusicType(const Mix_Music *music);

/* Get music title from meta-tag if possible. If title tag is empty, filename will be returned */
extern DECLSPEC const char *MIXCALL Mix_GetMusicTitle(const Mix_Music *music);
/* Get music title from meta-tag if possible */
extern DECLSPEC const char *MIXCALL Mix_GetMusicTitleTag(const Mix_Music *music);
/* Get music artist from meta-tag if possible */
extern DECLSPEC const char *MIXCALL Mix_GetMusicArtistTag(const Mix_Music *music);
/* Get music album from meta-tag if possible */
extern DECLSPEC const char *MIXCALL Mix_GetMusicAlbumTag(const Mix_Music *music);
/* Get music copyright from meta-tag if possible */
extern DECLSPEC const char *MIXCALL Mix_GetMusicCopyrightTag(const Mix_Music *music);

/* Set a function that is called after all mixing is performed.
   This can be used to provide real-time visual display of the audio stream
   or add a custom mixer filter for the stream data.
*/
extern DECLSPEC void MIXCALL Mix_SetPostMix(void (SDLCALL *mix_func)(void *udata, Uint8 *stream, int len), void *arg);

/* Add your own music player or additional mixer function.
   If 'mix_func' is NULL, the default music player is re-enabled.
 */
extern DECLSPEC void MIXCALL Mix_HookMusic(void (SDLCALL *mix_func)(void *udata, Uint8 *stream, int len), void *arg);

/* Add your own callback for when the music has finished playing or when it is
 * stopped from a call to Mix_HaltMusic.
 */
extern DECLSPEC void MIXCALL Mix_HookMusicFinished(void (SDLCALL *music_finished)(void));
extern DECLSPEC void MIXCALL Mix_HookMusicStreamFinishedAny(void (SDLCALL *music_finished)(void));
extern DECLSPEC void MIXCALL Mix_HookMusicStreamFinished(Mix_Music *music, void (SDLCALL *music_finished)(Mix_Music*, void*), void *user_data);/*Mixer-X*/

/* Get a pointer to the user data for the current music hook */
extern DECLSPEC void * MIXCALL Mix_GetMusicHookData(void);

/*
 * Add your own callback when a channel has finished playing. NULL
 *  to disable callback. The callback may be called from the mixer's audio
 *  callback or it could be called as a result of Mix_HaltChannel(), etc.
 *  do not call SDL_LockAudio() from this callback; you will either be
 *  inside the audio callback, or SDL_mixer will explicitly lock the audio
 *  before calling your callback.
 */
extern DECLSPEC void MIXCALL Mix_ChannelFinished(void (SDLCALL *channel_finished)(int channel));


/* Special Effects API by ryan c. gordon. (icculus@icculus.org) */

#define MIX_CHANNEL_POST  (-2)

/* This is the format of a special effect callback:
 *
 *   myeffect(int chan, void *stream, int len, void *udata);
 *
 * (chan) is the channel number that your effect is affecting. (stream) is
 *  the buffer of data to work upon. (len) is the size of (stream), and
 *  (udata) is a user-defined bit of data, which you pass as the last arg of
 *  Mix_RegisterEffect(), and is passed back unmolested to your callback.
 *  Your effect changes the contents of (stream) based on whatever parameters
 *  are significant, or just leaves it be, if you prefer. You can do whatever
 *  you like to the buffer, though, and it will continue in its changed state
 *  down the mixing pipeline, through any other effect functions, then finally
 *  to be mixed with the rest of the channels and music for the final output
 *  stream.
 *
 * DO NOT EVER call SDL_LockAudio() from your callback function!
 */
typedef void (SDLCALL *Mix_EffectFunc_t)(int chan, void *stream, int len, void *udata);

typedef void (SDLCALL *Mix_CommonMixer_t)(void *udata, Uint8 *stream, int len);

/*
 * This is a callback that signifies that a channel has finished all its
 *  loops and has completed playback. This gets called if the buffer
 *  plays out normally, or if you call Mix_HaltChannel(), implicitly stop
 *  a channel via Mix_AllocateChannels(), or unregister a callback while
 *  it's still playing.
 *
 * DO NOT EVER call SDL_LockAudio() from your callback function!
 */
typedef void (SDLCALL *Mix_EffectDone_t)(int chan, void *udata);

/* This is the format of a special effect callback:
 *
 *   myeffect(Mix_Music *mus, void *stream, int len, void *udata);
 *
 * (mus) is the music instalce that your effect is affecting. (stream) is
 *  the buffer of data to work upon. (len) is the size of (stream), and
 *  (udata) is a user-defined bit of data, which you pass as the last arg of
 *  Mix_RegisterMusicEffect(), and is passed back unmolested to your callback.
 *  Your effect changes the contents of (stream) based on whatever parameters
 *  are significant, or just leaves it be, if you prefer. You can do whatever
 *  you like to the buffer, though, and it will continue in its changed state
 *  down the mixing pipeline, through any other effect functions, then finally
 *  to be mixed with the rest of the channels and music for the final output
 *  stream.
 *
 * DO NOT EVER call SDL_LockAudio() from your callback function!
 */
typedef void (SDLCALL *Mix_MusicEffectFunc_t)(Mix_Music *mus, void *stream, int len, void *udata); /*MIXER-X*/

/*
 * This is a callback that signifies that a music is going to be deleted.
 *  This gets called if implicitly stop the music or request closing via Mix_FreeMusic(),
 *  or unregister a callback while it's still playing.
 *
 * DO NOT EVER call SDL_LockAudio() from your callback function!
 */
typedef void (SDLCALL *Mix_MusicEffectDone_t)(Mix_Music *mus, void *udata); /*MIXER-X*/


/* Register a special effect function. At mixing time, the channel data is
 *  copied into a buffer and passed through each registered effect function.
 *  After it passes through all the functions, it is mixed into the final
 *  output stream. The copy to buffer is performed once, then each effect
 *  function performs on the output of the previous effect. Understand that
 *  this extra copy to a buffer is not performed if there are no effects
 *  registered for a given chunk, which saves CPU cycles, and any given
 *  effect will be extra cycles, too, so it is crucial that your code run
 *  fast. Also note that the data that your function is given is in the
 *  format of the sound device, and not the format you gave to Mix_OpenAudio(),
 *  although they may in reality be the same. This is an unfortunate but
 *  necessary speed concern. Use Mix_QuerySpec() to determine if you can
 *  handle the data before you register your effect, and take appropriate
 *  actions.
 * You may also specify a callback (Mix_EffectDone_t) that is called when
 *  the channel finishes playing. This gives you a more fine-grained control
 *  than Mix_ChannelFinished(), in case you need to free effect-specific
 *  resources, etc. If you don't need this, you can specify NULL.
 * You may set the callbacks before or after calling Mix_PlayChannel().
 * Things like Mix_SetPanning() are just internal special effect functions,
 *  so if you are using that, you've already incurred the overhead of a copy
 *  to a separate buffer, and that these effects will be in the queue with
 *  any functions you've registered. The list of registered effects for a
 *  channel is reset when a chunk finishes playing, so you need to explicitly
 *  set them with each call to Mix_PlayChannel*().
 * You may also register a special effect function that is to be run after
 *  final mixing occurs. The rules for these callbacks are identical to those
 *  in Mix_RegisterEffect, but they are run after all the channels and the
 *  music have been mixed into a single stream, whereas channel-specific
 *  effects run on a given channel before any other mixing occurs. These
 *  global effect callbacks are call "posteffects". Posteffects only have
 *  their Mix_EffectDone_t function called when they are unregistered (since
 *  the main output stream is never "done" in the same sense as a channel).
 *  You must unregister them manually when you've had enough. Your callback
 *  will be told that the channel being mixed is (MIX_CHANNEL_POST) if the
 *  processing is considered a posteffect.
 *
 * After all these effects have finished processing, the callback registered
 *  through Mix_SetPostMix() runs, and then the stream goes to the audio
 *  device.
 *
 * DO NOT EVER call SDL_LockAudio() from your callback function!
 *
 * returns zero if error (no such channel), nonzero if added.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_RegisterEffect(int chan, Mix_EffectFunc_t f, Mix_EffectDone_t d, void *arg);


/* You may not need to call this explicitly, unless you need to stop an
 *  effect from processing in the middle of a chunk's playback.
 * Posteffects are never implicitly unregistered as they are for channels,
 *  but they may be explicitly unregistered through this function by
 *  specifying MIX_CHANNEL_POST for a channel.
 *
 * returns zero if error (no such channel or effect), nonzero if removed.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_UnregisterEffect(int channel, Mix_EffectFunc_t f);


/* You may not need to call this explicitly, unless you need to stop all
 *  effects from processing in the middle of a chunk's playback. Note that
 *  this will also shut off some internal effect processing, since
 *  Mix_SetPanning() and others may use this API under the hood. This is
 *  called internally when a channel completes playback.
 * Posteffects are never implicitly unregistered as they are for channels,
 *  but they may be explicitly unregistered through this function by
 *  specifying MIX_CHANNEL_POST for a channel.
 *
 * returns zero if error (no such channel), nonzero if all effects removed.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_UnregisterAllEffects(int channel);


/* The function is like the Mix_RegisterEffect(), but works exclusively for music
 * streams. Unlike the channels API, all effects were assigned to every individual
 * opened music instance and will stay working until the music will be closed or
 * all effects has been changed or removed manually.
 *
 * DO NOT EVER call SDL_LockAudio() from your callback function!
 *
 * returns zero if error (no such music), nonzero if added.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_RegisterMusicEffect(Mix_Music *mus,
                                                    Mix_MusicEffectFunc_t f,
                                                    Mix_MusicEffectDone_t d,
                                                    void *arg); /*MIXER-X*/

/* You may not need to call this explicitly, unless you need to stop an
 *  effect from processing in the middle of a music's playback.
 *
 * returns zero if error (no such channel or effect), nonzero if removed.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_UnregisterMusicEffect(Mix_Music *mus,
                                                      Mix_MusicEffectFunc_t f); /*MIXER-X*/

/* You may not need to call this explicitly, unless you need to stop all
 *  effects from processing in the middle of a music's playback. Note that
 *  this will also shut off some internal effect processing, since
 *  Mix_SetMusicEffectPanning() and others may use this API under the hood. This is
 *  called internally when a channel completes playback.
 *
 * returns zero if error (no such channel), nonzero if all effects removed.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_UnregisterAllMusicEffects(Mix_Music *mus); /*MIXER-X*/


#define MIX_EFFECTSMAXSPEED  "MIX_EFFECTSMAXSPEED"

/*
 * These are the internally-defined mixing effects. They use the same API that
 *  effects defined in the application use, but are provided here as a
 *  convenience. Some effects can reduce their quality or use more memory in
 *  the name of speed; to enable this, make sure the environment variable
 *  MIX_EFFECTSMAXSPEED (see above) is defined before you call
 *  Mix_OpenAudio().
 */


/* Set the panning of a channel. The left and right channels are specified
 *  as integers between 0 and 255, quietest to loudest, respectively.
 *
 * Technically, this is just individual volume control for a sample with
 *  two (stereo) channels, so it can be used for more than just panning.
 *  If you want real panning, call it like this:
 *
 *   Mix_SetPanning(channel, left, 255 - left);
 *
 * ...which isn't so hard.
 *
 * Setting (channel) to MIX_CHANNEL_POST registers this as a posteffect, and
 *  the panning will be done to the final mixed stream before passing it on
 *  to the audio device.
 *
 * This uses the Mix_RegisterEffect() API internally, and returns without
 *  registering the effect function if the audio device is not configured
 *  for stereo output. Setting both (left) and (right) to 255 causes this
 *  effect to be unregistered, since that is the data's normal state.
 *
 * returns zero if error (no such channel or Mix_RegisterEffect() fails),
 *  nonzero if panning effect enabled. Note that an audio device in mono
 *  mode is a no-op, but this call will return successful in that case.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetPanning(int channel, Uint8 left, Uint8 right);


/* Set the position of a channel. (angle) is an integer from 0 to 360, that
 *  specifies the location of the sound in relation to the listener. (angle)
 *  will be reduced as neccesary (540 becomes 180 degrees, -100 becomes 260).
 *  Angle 0 is due north, and rotates clockwise as the value increases.
 *  For efficiency, the precision of this effect may be limited (angles 1
 *  through 7 might all produce the same effect, 8 through 15 are equal, etc).
 *  (distance) is an integer between 0 and 255 that specifies the space
 *  between the sound and the listener. The larger the number, the further
 *  away the sound is. Using 255 does not guarantee that the channel will be
 *  culled from the mixing process or be completely silent. For efficiency,
 *  the precision of this effect may be limited (distance 0 through 5 might
 *  all produce the same effect, 6 through 10 are equal, etc). Setting (angle)
 *  and (distance) to 0 unregisters this effect, since the data would be
 *  unchanged.
 *
 * If you need more precise positional audio, consider using OpenAL for
 *  spatialized effects instead of SDL_mixer. This is only meant to be a
 *  basic effect for simple "3D" games.
 *
 * If the audio device is configured for mono output, then you won't get
 *  any effectiveness from the angle; however, distance attenuation on the
 *  channel will still occur. While this effect will function with stereo
 *  voices, it makes more sense to use voices with only one channel of sound,
 *  so when they are mixed through this effect, the positioning will sound
 *  correct. You can convert them to mono through SDL before giving them to
 *  the mixer in the first place if you like.
 *
 * Setting (channel) to MIX_CHANNEL_POST registers this as a posteffect, and
 *  the positioning will be done to the final mixed stream before passing it
 *  on to the audio device.
 *
 * This is a convenience wrapper over Mix_SetDistance() and Mix_SetPanning().
 *
 * returns zero if error (no such channel or Mix_RegisterEffect() fails),
 *  nonzero if position effect is enabled.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetPosition(int channel, Sint16 angle, Uint8 distance);


/* Set the "distance" of a channel. (distance) is an integer from 0 to 255
 *  that specifies the location of the sound in relation to the listener.
 *  Distance 0 is overlapping the listener, and 255 is as far away as possible
 *  A distance of 255 does not guarantee silence; in such a case, you might
 *  want to try changing the chunk's volume, or just cull the sample from the
 *  mixing process with Mix_HaltChannel().
 * For efficiency, the precision of this effect may be limited (distances 1
 *  through 7 might all produce the same effect, 8 through 15 are equal, etc).
 *  (distance) is an integer between 0 and 255 that specifies the space
 *  between the sound and the listener. The larger the number, the further
 *  away the sound is.
 * Setting (distance) to 0 unregisters this effect, since the data would be
 *  unchanged.
 * If you need more precise positional audio, consider using OpenAL for
 *  spatialized effects instead of SDL_mixer. This is only meant to be a
 *  basic effect for simple "3D" games.
 *
 * Setting (channel) to MIX_CHANNEL_POST registers this as a posteffect, and
 *  the distance attenuation will be done to the final mixed stream before
 *  passing it on to the audio device.
 *
 * This uses the Mix_RegisterEffect() API internally.
 *
 * returns zero if error (no such channel or Mix_RegisterEffect() fails),
 *  nonzero if position effect is enabled.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetDistance(int channel, Uint8 distance);


/*
 * !!! FIXME : Haven't implemented, since the effect goes past the
 *              end of the sound buffer. Will have to think about this.
 *               --ryan.
 */
#if 0
/* Causes an echo effect to be mixed into a sound. (echo) is the amount
 *  of echo to mix. 0 is no echo, 255 is infinite (and probably not
 *  what you want).
 *
 * Setting (channel) to MIX_CHANNEL_POST registers this as a posteffect, and
 *  the reverbing will be done to the final mixed stream before passing it on
 *  to the audio device.
 *
 * This uses the Mix_RegisterEffect() API internally. If you specify an echo
 *  of zero, the effect is unregistered, as the data is already in that state.
 *
 * returns zero if error (no such channel or Mix_RegisterEffect() fails),
 *  nonzero if reversing effect is enabled.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern no_parse_DECLSPEC int MIXCALL Mix_SetReverb(int channel, Uint8 echo);
#endif

/* Causes a channel to reverse its stereo. This is handy if the user has his
 *  speakers hooked up backwards, or you would like to have a minor bit of
 *  psychedelia in your sound code.  :)  Calling this function with (flip)
 *  set to non-zero reverses the chunks's usual channels. If (flip) is zero,
 *  the effect is unregistered.
 *
 * This uses the Mix_RegisterEffect() API internally, and thus is probably
 *  more CPU intensive than having the user just plug in his speakers
 *  correctly. Mix_SetReverseStereo() returns without registering the effect
 *  function if the audio device is not configured for stereo output.
 *
 * If you specify MIX_CHANNEL_POST for (channel), then this the effect is used
 *  on the final mixed stream before sending it on to the audio device (a
 *  posteffect).
 *
 * returns zero if error (no such channel or Mix_RegisterEffect() fails),
 *  nonzero if reversing effect is enabled. Note that an audio device in mono
 *  mode is a no-op, but this call will return successful in that case.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetReverseStereo(int channel, int flip);


/* Set the panning of a music. The left and right channels are specified
 *  as integers between 0 and 255, quietest to loudest, respectively.
 *
 * Technically, this is just individual volume control for a sample with
 *  two (stereo) channels, so it can be used for more than just panning.
 *  If you want real panning, call it like this:
 *
 *   Mix_SetMusicEffectPanning(music, left, 255 - left);
 *
 * ...which isn't so hard.
 *
 * This uses the Mix_RegisterMusicEffect() API internally, and returns without
 *  registering the effect function if the audio device is not configured
 *  for stereo output. Setting both (left) and (right) to 255 causes this
 *  effect to be unregistered, since that is the data's normal state.
 *
 * returns zero if error (no such music or Mix_RegisterMusicEffect() fails),
 *  nonzero if panning effect enabled. Note that an audio device in mono
 *  mode is a no-op, but this call will return successful in that case.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetMusicEffectPanning(Mix_Music *mus, Uint8 left, Uint8 right); /*MIXER-X*/


/* Set the position of a music. (angle) is an integer from 0 to 360, that
 *  specifies the location of the sound in relation to the listener. (angle)
 *  will be reduced as neccesary (540 becomes 180 degrees, -100 becomes 260).
 *  Angle 0 is due north, and rotates clockwise as the value increases.
 *  For efficiency, the precision of this effect may be limited (angles 1
 *  through 7 might all produce the same effect, 8 through 15 are equal, etc).
 *  (distance) is an integer between 0 and 255 that specifies the space
 *  between the sound and the listener. The larger the number, the further
 *  away the sound is. Using 255 does not guarantee that the channel will be
 *  culled from the mixing process or be completely silent. For efficiency,
 *  the precision of this effect may be limited (distance 0 through 5 might
 *  all produce the same effect, 6 through 10 are equal, etc). Setting (angle)
 *  and (distance) to 0 unregisters this effect, since the data would be
 *  unchanged.
 *
 * If you need more precise positional audio, consider using OpenAL for
 *  spatialized effects instead of SDL_mixer. This is only meant to be a
 *  basic effect for simple "3D" games.
 *
 * If the audio device is configured for mono output, then you won't get
 *  any effectiveness from the angle; however, distance attenuation on the
 *  channel will still occur. While this effect will function with stereo
 *  voices, it makes more sense to use voices with only one channel of sound,
 *  so when they are mixed through this effect, the positioning will sound
 *  correct. You can convert them to mono through SDL before giving them to
 *  the mixer in the first place if you like.
 *
 * This is a convenience wrapper over Mix_SetMusicEffectDistance() and
 *  Mix_SetMusicEffectPanning().
 *
 * returns zero if error (no such music or Mix_RegisterMusicEffect() fails),
 *  nonzero if position effect is enabled.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetMusicEffectPosition(Mix_Music *mus, Sint16 angle, Uint8 distance); /*MIXER-X*/

/* Set the "distance" of a music. (distance) is an integer from 0 to 255
 *  that specifies the location of the sound in relation to the listener.
 *  Distance 0 is overlapping the listener, and 255 is as far away as possible
 *  A distance of 255 does not guarantee silence; in such a case, you might
 *  want to try changing the chunk's volume, or just cull the sample from the
 *  mixing process with Mix_HaltMusicStream().
 * For efficiency, the precision of this effect may be limited (distances 1
 *  through 7 might all produce the same effect, 8 through 15 are equal, etc).
 *  (distance) is an integer between 0 and 255 that specifies the space
 *  between the sound and the listener. The larger the number, the further
 *  away the sound is.
 * Setting (distance) to 0 unregisters this effect, since the data would be
 *  unchanged.
 * If you need more precise positional audio, consider using OpenAL for
 *  spatialized effects instead of SDL_mixer. This is only meant to be a
 *  basic effect for simple "3D" games.
 *
 * This uses the Mix_RegisterMusicEffect() API internally.
 *
 * returns zero if error (no such music or Mix_RegisterMusicEffect() fails),
 *  nonzero if position effect is enabled.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetMusicEffectDistance(Mix_Music *mus, Uint8 distance); /*MIXER-X*/

/* Causes a music to reverse its stereo. This is handy if the user has his
 *  speakers hooked up backwards, or you would like to have a minor bit of
 *  psychedelia in your sound code.  :)  Calling this function with (flip)
 *  set to non-zero reverses the chunks's usual channels. If (flip) is zero,
 *  the effect is unregistered.
 *
 * This uses the Mix_RegisterMusicEffect() API internally, and thus is probably
 *  more CPU intensive than having the user just plug in his speakers
 *  correctly. Mix_SetMusicEffectReverseStereo() returns without registering the effect
 *  function if the audio device is not configured for stereo output.
 *
 * returns zero if error (no such music or Mix_RegisterMusicEffect() fails),
 *  nonzero if reversing effect is enabled. Note that an audio device in mono
 *  mode is a no-op, but this call will return successful in that case.
 *  Error messages can be retrieved from Mix_GetError().
 */
extern DECLSPEC int MIXCALL Mix_SetMusicEffectReverseStereo(Mix_Music *mus, int flip); /*MIXER-X*/

/* end of effects API. --ryan. */


/* Reserve the first channels (0 -> n-1) for the application, i.e. don't allocate
   them dynamically to the next sample if requested with a -1 value below.
   Returns the number of reserved channels.
 */
extern DECLSPEC int MIXCALL Mix_ReserveChannels(int num);

/* Channel grouping functions */

/* Attach a tag to a channel. A tag can be assigned to several mixer
   channels, to form groups of channels.
   If 'tag' is -1, the tag is removed (actually -1 is the tag used to
   represent the group of all the channels).
   Returns true if everything was OK.
 */
extern DECLSPEC int MIXCALL Mix_GroupChannel(int which, int tag);
/* Assign several consecutive channels to a group */
extern DECLSPEC int MIXCALL Mix_GroupChannels(int from, int to, int tag);
/* Finds the first available channel in a group of channels,
   returning -1 if none are available.
 */
extern DECLSPEC int MIXCALL Mix_GroupAvailable(int tag);
/* Returns the number of channels in a group. This is also a subtle
   way to get the total number of channels when 'tag' is -1
 */
extern DECLSPEC int MIXCALL Mix_GroupCount(int tag);
/* Finds the "oldest" sample playing in a group of channels */
extern DECLSPEC int MIXCALL Mix_GroupOldest(int tag);
/* Finds the "most recent" (i.e. last) sample playing in a group of channels */
extern DECLSPEC int MIXCALL Mix_GroupNewer(int tag);

/* Play an audio chunk on a specific channel.
   If the specified channel is -1, play on the first free channel.
   If 'loops' is greater than zero, loop the sound that many times.
   If 'loops' is -1, loop inifinitely (~65000 times).
   Returns which channel was used to play the sound.
*/
#define Mix_PlayChannel(channel,chunk,loops) Mix_PlayChannelTimed(channel,chunk,loops,-1)
/* The same as above, but the sound is played at most 'ticks' milliseconds */
extern DECLSPEC int MIXCALL Mix_PlayChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ticks);
extern DECLSPEC int MIXCALL Mix_PlayMusic(Mix_Music *music, int loops);
#define Mix_PlayChannelVol(channel,chunk,loops,vol) Mix_PlayChannelTimedVolume(channel,chunk,loops,-1,vol)/*MIXER-X*/
extern DECLSPEC int MIXCALL Mix_PlayChannelTimedVolume(int which, Mix_Chunk *chunk, int loops, int ticks, int volume);/*MIXER-X*/

/* returns a pointer to the single-music mixer that can be used as a callback */
extern DECLSPEC Mix_CommonMixer_t MIXCALL Mix_GetMusicMixer(void);
/* returns a pointer to the multi-music mixer that can be used as a callback */
extern DECLSPEC Mix_CommonMixer_t MIXCALL Mix_GetMultiMusicMixer(void);

/* returns a pointer to the general mixer of music and channels that can be used as a callback
 *
 * This mixer processes the whole internality of the Mixer library: music, mutli-music, and channels
 * Use this callback if you want to make your own audio output processing. Use the Mix_InitMixer() to
 * initialize the Mixer without starting the SDL Audio instance and the Mix_FreeMixer() to close the library.
 *
 * CAUTION: using this callback, don't use Mix_GeneralMusicMixer() and Mix_GeneralMultiMusicMixeR() at all
 * they both already used in this mixer internally.
 */
extern DECLSPEC Mix_CommonMixer_t MIXCALL Mix_GetGeneralMixer(void);

/* Fade in music or a channel over "ms" milliseconds, same semantics as the "Play" functions */
extern DECLSPEC int MIXCALL Mix_FadeInMusic(Mix_Music *music, int loops, int ms);
extern DECLSPEC int MIXCALL Mix_FadeInMusicPos(Mix_Music *music, int loops, int ms, double position);
#define Mix_FadeInChannel(channel,chunk,loops,ms) Mix_FadeInChannelTimed(channel,chunk,loops,ms,-1)
extern DECLSPEC int MIXCALL Mix_FadeInChannelTimed(int channel, Mix_Chunk *chunk, int loops, int ms, int ticks);
#define Mix_FadeInChannelVolume(channel,chunk,loops,ms,vol) Mix_FadeInChannelTimedVolume(channel,chunk,loops,ms,-1,vol)/*MIXER-X*/
extern DECLSPEC int MIXCALL Mix_FadeInChannelTimedVolume(int which, Mix_Chunk *chunk, int loops, int ms, int ticks, int volume);/*MIXER-X*/

/* Multi-Music */
extern DECLSPEC int MIXCALL Mix_PlayMusicStream(Mix_Music *music, int loops); /*MIXER-X*/
extern DECLSPEC int MIXCALL Mix_FadeInMusicStream(Mix_Music *music, int loops, int ms); /*MIXER-X*/
extern DECLSPEC int MIXCALL Mix_FadeInMusicStreamPos(Mix_Music *music, int loops, int ms, double position); /*MIXER-X*/


/* Set the volume in the range of 0-128 of a specific channel or chunk.
   If the specified channel is -1, set volume for all channels.
   Returns the original volume.
   If the specified volume is -1, just return the current volume.
*/
extern DECLSPEC int MIXCALL Mix_Volume(int channel, int volume);
extern DECLSPEC int MIXCALL Mix_VolumeChunk(Mix_Chunk *chunk, int volume);
extern DECLSPEC int MIXCALL Mix_VolumeMusicStream(Mix_Music *music, int volume);
/* MIXERX_DEPRECATED("Use Mix_VolumeMusicStream(Mix_Music*,int) instead") */
extern DECLSPEC int MIXCALL Mix_VolumeMusic(int volume);
/* Get the current volume value in the range of 0-128 of a music stream */
extern DECLSPEC int MIXCALL Mix_GetMusicVolume(Mix_Music *music);
extern DECLSPEC int MIXCALL Mix_GetVolumeMusicStream(Mix_Music *music);

extern DECLSPEC void MIXCALL Mix_VolumeMusicGeneral(int volume);
extern DECLSPEC int MIXCALL Mix_GetVolumeMusicGeneral(void);

/* Set the master volume for all channels.
   This did not affect the member variables of channel or chunk volume.
   If the specified volume is -1, just return the current master volume.
*/
extern DECLSPEC int MIXCALL Mix_MasterVolume(int volume);

/* Halt playing of a particular channel */
extern DECLSPEC int MIXCALL Mix_HaltChannel(int channel);
extern DECLSPEC int MIXCALL Mix_HaltGroup(int tag);
extern DECLSPEC int MIXCALL Mix_HaltMusicStream(Mix_Music *music);
/* MIXERX_DEPRECATED("Use Mix_HaltMusicStream(Mix_Music*) instead") */
extern DECLSPEC int MIXCALL Mix_HaltMusic(void);

/* Change the expiration delay for a particular channel.
   The sample will stop playing after the 'ticks' milliseconds have elapsed,
   or remove the expiration if 'ticks' is -1
*/
extern DECLSPEC int MIXCALL Mix_ExpireChannel(int channel, int ticks);

/* Halt a channel, fading it out progressively till it's silent
   The ms parameter indicates the number of milliseconds the fading
   will take.
 */
extern DECLSPEC int MIXCALL Mix_FadeOutChannel(int which, int ms);
extern DECLSPEC int MIXCALL Mix_FadeOutGroup(int tag, int ms);
extern DECLSPEC int MIXCALL Mix_FadeOutMusicStream(Mix_Music *music, int ms);
/* MIXERX_DEPRECATED("Use Mix_FadeOutMusicStream(Mix_Music*,int) instead") */
extern DECLSPEC int MIXCALL Mix_FadeOutMusic(int ms);

extern DECLSPEC int MIXCALL Mix_CrossFadeMusicStream(Mix_Music *old_music, Mix_Music *new_music, int loops, int ms, int free_old);
extern DECLSPEC int MIXCALL Mix_CrossFadeMusicStreamPos(Mix_Music *old_music, Mix_Music *new_music, int loops, int ms, double pos, int free_old);

/* Query the fading status of a channel */
extern DECLSPEC Mix_Fading MIXCALL Mix_FadingMusicStream(Mix_Music *music);
/* MIXERX_DEPRECATED("Use Mix_FadingMusicStream(Mix_Music*) instead") */
extern DECLSPEC Mix_Fading MIXCALL Mix_FadingMusic(void);
extern DECLSPEC Mix_Fading MIXCALL Mix_FadingChannel(int which);

/* Pause/Resume a particular channel */
extern DECLSPEC void MIXCALL Mix_Pause(int channel);
extern DECLSPEC void MIXCALL Mix_Resume(int channel);
extern DECLSPEC int MIXCALL Mix_Paused(int channel);

/* Pause/Resume the music stream */
extern DECLSPEC void MIXCALL Mix_PauseMusicStream(Mix_Music *music);
extern DECLSPEC void MIXCALL Mix_ResumeMusicStream(Mix_Music *music);
extern DECLSPEC void MIXCALL Mix_RewindMusicStream(Mix_Music *music);
extern DECLSPEC int MIXCALL Mix_PausedMusicStream(Mix_Music *music);
extern DECLSPEC void MIXCALL Mix_PauseMusicStreamAll(void);
extern DECLSPEC void MIXCALL Mix_ResumeMusicStreamAll(void);

/* Pause/Resume the music stream (Deprecated calls) */
/* MIXERX_DEPRECATED("Use Mix_PauseMusicStream(Mix_Music*) instead") */
extern DECLSPEC void MIXCALL Mix_PauseMusic(void);
/* MIXERX_DEPRECATED("Use Mix_ResumeMusicStream(Mix_Music*) instead") */
extern DECLSPEC void MIXCALL Mix_ResumeMusic(void);
/* MIXERX_DEPRECATED("Use Mix_RewindMusicStream(Mix_Music*) instead") */
extern DECLSPEC void MIXCALL Mix_RewindMusic(void);
/* MIXERX_DEPRECATED("Use Mix_PausedMusicStream(Mix_Music*) instead") */
extern DECLSPEC int MIXCALL Mix_PausedMusic(void);

/* Jump to a given order in mod music.
   Returns 0 if successful, or -1 if failed or isn't implemented.
   Only for MOD music formats.
 */
extern DECLSPEC int MIXCALL Mix_ModMusicJumpToOrder(int order);
extern DECLSPEC int MIXCALL Mix_ModMusicStreamJumpToOrder(Mix_Music *music, int order);

/* Set the current position in the music stream (in seconds).
   This returns 0 if successful, or -1 if it failed or not implemented.
   This function is only implemented for MOD music formats (set pattern
   order number) and for WAV, OGG, FLAC, MP3, and MODPLUG music at the
   moment.
*/
extern DECLSPEC int MIXCALL Mix_SetMusicPositionStream(Mix_Music *music, double position);

MIXERX_DEPRECATED("Use Mix_SetMusicPositionStream(Mix_Music*, double) instead")
extern DECLSPEC int MIXCALL Mix_SetMusicStreamPosition(Mix_Music *music, double position);
/*
    Deprecated analogue of Mix_SetMusicStreamPosition() which lacks Mix_Music* argument
*/
/* MIXERX_DEPRECATED("Use Mix_SetMusicStreamPosition(Mix_Music*, double) instead") */
extern DECLSPEC int MIXCALL Mix_SetMusicPosition(double position);
/*
    Get the time current position of music stream
    returns -1.0 if this feature is not supported for some codec
 */
extern DECLSPEC double MIXCALL Mix_GetMusicPosition(Mix_Music *music);

/* Return music duration in seconds.
   If NULL is passed, returns duration of current playing music.
   Returns -1 on error.
 */
extern DECLSPEC double MIXCALL Mix_MusicDuration(Mix_Music *music);
/*
    Same as Mix_MusicDuration()
 */
/* MIXERX_DEPRECATED("Use Mix_MusicDuration(Mix_Music*) instead") */
extern DECLSPEC double MIXCALL Mix_GetMusicTotalTime(Mix_Music *music);

/*
    Set the current tempo multiplier in the music stream.
    This returns 0 if successful, or -1 if failed or isn't implemented.
    This function is only implemented for ADLMIDI, OPNMIDI MIDI synthesizers,
    GME chiptune formats, and for XMP library
 */
extern DECLSPEC int MIXCALL Mix_SetMusicTempo(Mix_Music *music, double tempo);

/*
    Get the current tempo multiplier of the music stream
 */
extern DECLSPEC double MIXCALL Mix_GetMusicTempo(Mix_Music *music);

/*
    Get the count of concurrently playing tracks at the song (MIDI, Tracker,.etc.)
 */
extern DECLSPEC int MIXCALL Mix_GetMusicTracks(Mix_Music *music);

/*
    Mute one of playing tracks at the song
 */
extern DECLSPEC int MIXCALL Mix_SetMusicTrackMute(Mix_Music *music, int track, int mute);

/*
    Get the loop start time position of music stream
    returns -1.0 if this feature is not used for this music or not supported for some codec
 */
extern DECLSPEC double MIXCALL Mix_GetMusicLoopStartTime(Mix_Music *music);
/*
    Get the loop end time position of music stream
    returns -1.0 if this feature is not used for this music or not supported for some codec
 */
extern DECLSPEC double MIXCALL Mix_GetMusicLoopEndTime(Mix_Music *music);
/*
    Get the loop time length of music stream
    returns -1.0 if this feature is not used for this music or not supported for some codec
 */
extern DECLSPEC double MIXCALL Mix_GetMusicLoopLengthTime(Mix_Music *music);


/* Check the status of a specific channel.
   If the specified channel is -1, check all channels.
*/
extern DECLSPEC int MIXCALL Mix_Playing(int channel);
extern DECLSPEC int MIXCALL Mix_PlayingMusicStream(Mix_Music *music);
/* MIXERX_DEPRECATED("Use Mix_PlayingMusicStream(Mix_Music*) instead") */
extern DECLSPEC int MIXCALL Mix_PlayingMusic(void);

/* Stop music and set external music playback command */
extern DECLSPEC int MIXCALL Mix_SetMusicCMD(const char *command);

/* Synchro value is set from modules while playing */
extern DECLSPEC int MIXCALL Mix_SetSynchroValue(int value);
extern DECLSPEC int MIXCALL Mix_GetSynchroValue(void);

/* Set/Get/Iterate SoundFonts paths to use by supported MIDI backends */
extern DECLSPEC int MIXCALL Mix_SetSoundFonts(const char *paths);
extern DECLSPEC const char* MIXCALL Mix_GetSoundFonts(void);
extern DECLSPEC int MIXCALL Mix_EachSoundFont(int (SDLCALL *function)(const char*, void*), void *data);
extern DECLSPEC int MIXCALL Mix_EachSoundFontEx(const char* cpaths, int (SDLCALL *function)(const char*, void*), void *data);

/* Set/Get full path of Timidity config file (e.g. /etc/timidity.cfg) */
extern DECLSPEC int MIXCALL Mix_SetTimidityCfg(const char *path);
extern DECLSPEC const char* MIXCALL Mix_GetTimidityCfg(void);

/* Get the Mix_Chunk currently associated with a mixer channel
    Returns NULL if it's an invalid channel, or there's no chunk associated.
*/
extern DECLSPEC Mix_Chunk * MIXCALL Mix_GetChunk(int channel);

/* Setup the mixer without taking over the callback, using an existing spec.
   These Only initialize or free the Mixer internals */
extern DECLSPEC int MIXCALL Mix_InitMixer(const SDL_AudioSpec *spec, SDL_bool skip_init_check);
extern DECLSPEC void MIXCALL Mix_FreeMixer(void);

/* Close the mixer, halting all playing audio */
extern DECLSPEC void MIXCALL Mix_CloseAudio(void);

/* ADLMIDI Setup functions */
/* Get count of available hardcoded banks */
extern DECLSPEC int    MIXCALL Mix_ADLMIDI_getTotalBanks(void);
/* Get array of the bank names */
extern DECLSPEC const char *const *MIXCALL Mix_ADLMIDI_getBankNames(void);
/* Get bank ID */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getBankID(void);
/* Set bank ID (Applying on stop/play) */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setBankID(int bnk);
/* Get state of deep vibrato */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getTremolo(void);
/* Set deep tremolo mode (0 off, 1 on) (Applying on stop/play) */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setTremolo(int tr);
/* Get state of deep vibrato */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getVibrato(void);
/* Set deep vibrato mode (0 off, 1 on) (Applying on stop/play) */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setVibrato(int vib);
/* Get state of scalable modulation mode */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getScaleMod(void);
/* Set scalable modulation mode (0 off, 1 on) (Applying on stop/play) */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setScaleMod(int sc);
/* Get state of adlib drums mode */
MIXERX_DEPRECATED("This function is no more useful, rhythm-mode is now fully automated")
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getAdLibMode(void);
/* Set adlib drums mode mode (0 off, 1 on) (Applying on stop/play) */
MIXERX_DEPRECATED("This function is no more useful, rhythm-mode is now fully automated")
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setAdLibMode(int tr);
/* Get state of logarithmic mode */
MIXERX_DEPRECATED("This function is no longer has effect. Please use the Mix_ADLMIDI_getVolumeModel()")
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getLogarithmicVolumes(void);
/* Set logarithmic volumes mode in the generic/CMF volume models (0 off, 1 on) (Applying on stop/play) */
MIXERX_DEPRECATED("This function is no longer has effect. Please use the Mix_ADLMIDI_setVolumeModel()")
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setLogarithmicVolumes(int lv);
/* Get current volume model ID */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getVolumeModel(void);
/* Change current volumes model (Applying on stop/play) */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setVolumeModel(int vm);
/* Get full range mode for CC74-Brightness controller */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getFullRangeBrightness(void);
/* Set full range mode for CC74-Brightness controller */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setFullRangeBrightness(int frb);
/* Get the automatic arpeggio enabled or disabled setting */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getAutoArpeggio();
/* Set the automatic arpeggio enabled or disabled */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setAutoArpeggio(int aa_en);
/* Get full panning stereo mode */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getFullPanStereo(void);
/* Set full panning stereo mode */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setFullPanStereo(int fp);
/* Get the current OPL3 Emulator for ADLMIDI */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getEmulator(void);
/* Select the OPL3 Emulator for ADLMIDI */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setEmulator(int emu);
/* Get the setup of count of virtual chips */
extern DECLSPEC int  MIXCALL Mix_ADLMIDI_getChipsCount(void);
/* Set count of virtual chips (integer between 1 and 100, or -1 to restore default setup) */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setChipsCount(int chips);
/* Reset all ADLMIDI properties to default state */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setSetDefaults(void);

/* Sets WOPL bank file for ADLMIDI playing device, affects on MIDI file reopen */
extern DECLSPEC void MIXCALL Mix_ADLMIDI_setCustomBankFile(const char *bank_wonl_path);

/* Reset all OPNMIDI properties to default state */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setSetDefaults(void);
/* Get current volume model ID */
extern DECLSPEC int  MIXCALL Mix_OPNMIDI_getVolumeModel(void);
/* Change current volumes model (Applying on stop/play) */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setVolumeModel(int vm);
/* Get full range mode for CC74-Brightness controller */
extern DECLSPEC int  MIXCALL Mix_OPNMIDI_getFullRangeBrightness(void);
/* Set full range mode for CC74-Brightness controller */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setFullRangeBrightness(int frb);
/* Get the automatic arpeggio enabled or disabled setting */
extern DECLSPEC int  MIXCALL Mix_OPNMIDI_getAutoArpeggio();
/* Set the automatic arpeggio enabled or disabled */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setAutoArpeggio(int aa_en);
/* Get full panning stereo mode */
extern DECLSPEC int  MIXCALL Mix_OPNMIDI_getFullPanStereo(void);
/* Set full panning stereo mode */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setFullPanStereo(int fp);
/* Get the OPN2 Emulator for OPNMIDI */
extern DECLSPEC int  MIXCALL Mix_OPNMIDI_getEmulator(void);
/* Select the OPN2 Emulator for OPNMIDI */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setEmulator(int emu);
/* Get the setup of count of virtual chips */
extern DECLSPEC int  MIXCALL Mix_OPNMIDI_getChipsCount(void);
/* Set count of virtual chips (integer between 1 and 100, or -1 to restore default setup) */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setChipsCount(int chips);
/* Sets WOPN bank file for OPNMIDI playing device, affects on MIDI file reopen */
extern DECLSPEC void MIXCALL Mix_OPNMIDI_setCustomBankFile(const char *bank_wonp_path);

/* Disables/enables built-in echo effect for playing SPC files */
extern DECLSPEC void MIXCALL Mix_GME_SetSpcEchoDisabled(Mix_Music *music, int disabled);
extern DECLSPEC int MIXCALL Mix_GME_GetSpcEchoDisabled(Mix_Music *music);

/* Get type of MIDI player library currently in use */
extern DECLSPEC int  MIXCALL Mix_GetMidiPlayer(void);

/* Get type of MIDI player library prepared for next opening of MIDI file */
extern DECLSPEC int  MIXCALL Mix_GetNextMidiPlayer(void);

/* Set the MIDI playing library (ADLMIDI, Timidity, Native MIDI (if available) and FluidSynth) */
extern DECLSPEC int  MIXCALL Mix_SetMidiPlayer(int player);

/* Disables support of MIDI file arguments */
extern DECLSPEC void MIXCALL Mix_SetLockMIDIArgs(int lock_midiargs);


/*  DEPRECATED FUNCTIONS */

MIXERX_DEPRECATED("Use Mix_SetTimidityCfg(path) instead")
extern DECLSPEC void MIXCALL Mix_Timidity_addToPathList(const char *path);

MIXERX_DEPRECATED("Use Mix_GetMidiPlayer() instead")
extern DECLSPEC int  MIXCALL Mix_GetMidiDevice(void);

MIXERX_DEPRECATED("Use Mix_GetNextMidiPlayer() instead")
extern DECLSPEC int  MIXCALL Mix_GetNextMidiDevice(void);

MIXERX_DEPRECATED("Use Mix_SetMidiPlayer() instead")
extern DECLSPEC int  MIXCALL Mix_SetMidiDevice(int player);

/*  DEPRECATED FUNCTIONS, END */

/* We'll use SDL for reporting errors */
#define Mix_SetError    SDL_SetError
#define Mix_GetError    SDL_GetError
#define Mix_ClearError  SDL_ClearError
#define Mix_OutOfMemory SDL_OutOfMemory

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* SDL_MIXER_H_ */

/* vi: set ts=4 sw=4 expandtab: */
