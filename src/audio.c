#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

typedef struct {
    int       channels;
    int       sample_rate;
    char*     sink_title;
    ma_engine engine;
} Audio;

typedef struct {
    ma_sound ma;
} Sound;

Audio audio = {
    2,
    48000,
    "\0"
};

// Engine "no sound preloaded"

ma_result ma_engine_mod_init(const ma_engine_config* pConfig, ma_engine* pEngine,const char* titlesink)
{ // High Level API miniaudio made of LowLevel Calls mod to have titlesink
    ma_result result;
    ma_node_graph_config nodeGraphConfig;
    ma_engine_config engineConfig;
    ma_spatializer_listener_config listenerConfig;
    ma_uint32 iListener;
    if (pEngine == NULL) {
        return MA_INVALID_ARGS;
    }
    MA_ZERO_OBJECT(pEngine);
    if (pConfig != NULL) {
        engineConfig = *pConfig;
    } else {
        engineConfig = ma_engine_config_init();
    }
    pEngine->monoExpansionMode = engineConfig.monoExpansionMode;
    pEngine->defaultVolumeSmoothTimeInPCMFrames = engineConfig.defaultVolumeSmoothTimeInPCMFrames;
    pEngine->onProcess = engineConfig.onProcess;
    pEngine->pProcessUserData = engineConfig.pProcessUserData;
    ma_allocation_callbacks_init_copy(&pEngine->allocationCallbacks, &engineConfig.allocationCallbacks);
    #if !defined(MA_NO_RESOURCE_MANAGER)
    {
        pEngine->pResourceManager = engineConfig.pResourceManager;
    }
    #endif
    #if !defined(MA_NO_DEVICE_IO)
    {
        pEngine->pDevice = engineConfig.pDevice;
        if (pEngine->pDevice == NULL && engineConfig.noDevice == MA_FALSE) {
            ma_device_config deviceConfig;

            pEngine->pDevice = (ma_device*)ma_malloc(sizeof(*pEngine->pDevice), &pEngine->allocationCallbacks);
            if (pEngine->pDevice == NULL) {
                return MA_OUT_OF_MEMORY;
            }
            deviceConfig = ma_device_config_init(ma_device_type_playback);
            deviceConfig.playback.pDeviceID        = engineConfig.pPlaybackDeviceID;
            deviceConfig.playback.format           = ma_format_f32;
            deviceConfig.playback.channels         = engineConfig.channels;
            deviceConfig.sampleRate                = engineConfig.sampleRate;
            deviceConfig.dataCallback              = (engineConfig.dataCallback != NULL) ? engineConfig.dataCallback : ma_engine_data_callback_internal;
            deviceConfig.pUserData                 = pEngine;
            deviceConfig.notificationCallback      = engineConfig.notificationCallback;
            deviceConfig.periodSizeInFrames        = engineConfig.periodSizeInFrames;
            deviceConfig.periodSizeInMilliseconds  = engineConfig.periodSizeInMilliseconds;
            deviceConfig.pulse.pStreamNamePlayback = titlesink;
            deviceConfig.pulse.pStreamNameCapture  = titlesink;
            deviceConfig.noPreSilencedOutputBuffer = MA_TRUE;
            deviceConfig.noClip                    = MA_TRUE;
            if (engineConfig.pContext == NULL) {
                ma_context_config contextConfig = ma_context_config_init();
                contextConfig.allocationCallbacks = pEngine->allocationCallbacks;
                contextConfig.pLog = engineConfig.pLog;
                #ifndef MA_NO_RESOURCE_MANAGER
                {
                    if (contextConfig.pLog == NULL && engineConfig.pResourceManager != NULL) {
                        contextConfig.pLog = ma_resource_manager_get_log(engineConfig.pResourceManager);
                    }
                }
                #endif
                result = ma_device_init_ex(NULL, 0, &contextConfig, &deviceConfig, pEngine->pDevice);
            } else {
                result = ma_device_init(engineConfig.pContext, &deviceConfig, pEngine->pDevice);
            }
            if (result != MA_SUCCESS) {
                ma_free(pEngine->pDevice, &pEngine->allocationCallbacks);
                pEngine->pDevice = NULL;
                return result;
            }

            pEngine->ownsDevice = MA_TRUE;
        }
        if (pEngine->pDevice != NULL) {
            engineConfig.channels   = pEngine->pDevice->playback.channels;
            engineConfig.sampleRate = pEngine->pDevice->sampleRate;
        }
    }
    #endif
    if (engineConfig.channels == 0 || engineConfig.sampleRate == 0) {
        return MA_INVALID_ARGS;
    }
    pEngine->sampleRate = engineConfig.sampleRate;
    if (engineConfig.pLog != NULL) {
        pEngine->pLog = engineConfig.pLog;
    } else {
        #if !defined(MA_NO_DEVICE_IO)
        {
            pEngine->pLog = ma_device_get_log(pEngine->pDevice);
        }
        #else
        {
            pEngine->pLog = NULL;
        }
        #endif
    }
    nodeGraphConfig = ma_node_graph_config_init(engineConfig.channels);
    nodeGraphConfig.nodeCacheCapInFrames = (engineConfig.periodSizeInFrames > 0xFFFF) ? 0xFFFF : (ma_uint16)engineConfig.periodSizeInFrames;
    result = ma_node_graph_init(&nodeGraphConfig, &pEngine->allocationCallbacks, &pEngine->nodeGraph);
    if (result != MA_SUCCESS) {
        goto on_error_1;
    }
    if (engineConfig.listenerCount == 0) {
        engineConfig.listenerCount = 1;
    }
    if (engineConfig.listenerCount > MA_ENGINE_MAX_LISTENERS) {
        result = MA_INVALID_ARGS;
        goto on_error_1;
    }
    for (iListener = 0; iListener < engineConfig.listenerCount; iListener += 1) {
        listenerConfig = ma_spatializer_listener_config_init(ma_node_graph_get_channels(&pEngine->nodeGraph));
        #if !defined(MA_NO_DEVICE_IO)
        {
            if (pEngine->pDevice != NULL) {
            }
        }
        #endif
        result = ma_spatializer_listener_init(&listenerConfig, &pEngine->allocationCallbacks, &pEngine->listeners[iListener]);  /* TODO: Change this to a pre-allocated heap. */
        if (result != MA_SUCCESS) {
            goto on_error_2;
        }
        pEngine->listenerCount += 1;
    }
    pEngine->gainSmoothTimeInFrames = engineConfig.gainSmoothTimeInFrames;
    if (pEngine->gainSmoothTimeInFrames == 0) {
        ma_uint32 gainSmoothTimeInMilliseconds = engineConfig.gainSmoothTimeInMilliseconds;
        if (gainSmoothTimeInMilliseconds == 0) {
            gainSmoothTimeInMilliseconds = 8;
        }

        pEngine->gainSmoothTimeInFrames = (gainSmoothTimeInMilliseconds * ma_engine_get_sample_rate(pEngine)) / 1000;  /* 8ms by default. */
    }
    #ifndef MA_NO_RESOURCE_MANAGER
    {
        if (pEngine->pResourceManager == NULL) {
            ma_resource_manager_config resourceManagerConfig;
            pEngine->pResourceManager = (ma_resource_manager*)ma_malloc(sizeof(*pEngine->pResourceManager), &pEngine->allocationCallbacks);
            if (pEngine->pResourceManager == NULL) {
                result = MA_OUT_OF_MEMORY;
                goto on_error_2;
            }
            resourceManagerConfig = ma_resource_manager_config_init();
            resourceManagerConfig.pLog              = pEngine->pLog;    /* Always use the engine's log for internally-managed resource managers. */
            resourceManagerConfig.decodedFormat     = ma_format_f32;
            resourceManagerConfig.decodedChannels   = 0;  /* Leave the decoded channel count as 0 so we can get good spatialization. */
            resourceManagerConfig.decodedSampleRate = ma_engine_get_sample_rate(pEngine);
            ma_allocation_callbacks_init_copy(&resourceManagerConfig.allocationCallbacks, &pEngine->allocationCallbacks);
            resourceManagerConfig.pVFS              = engineConfig.pResourceManagerVFS;
            #if defined(MA_EMSCRIPTEN)
            {
                resourceManagerConfig.jobThreadCount = 0;
                resourceManagerConfig.flags |= MA_RESOURCE_MANAGER_FLAG_NO_THREADING;
            }
            #endif
            result = ma_resource_manager_init(&resourceManagerConfig, pEngine->pResourceManager);
            if (result != MA_SUCCESS) {
                goto on_error_3;
            }
            pEngine->ownsResourceManager = MA_TRUE;
        }
    }
    #endif
    pEngine->inlinedSoundLock  = 0;
    pEngine->pInlinedSoundHead = NULL;
    #if !defined(MA_NO_DEVICE_IO)
    {
        if (engineConfig.noAutoStart == MA_FALSE && pEngine->pDevice != NULL) {
            result = ma_engine_start(pEngine);
            if (result != MA_SUCCESS) {
                goto on_error_4;
            }
        }
    }
    #endif
    return MA_SUCCESS;
    #if !defined(MA_NO_DEVICE_IO)
    on_error_4:
    #endif
    #if !defined(MA_NO_RESOURCE_MANAGER)
    on_error_3:
        if (pEngine->ownsResourceManager) {
            ma_free(pEngine->pResourceManager, &pEngine->allocationCallbacks);
        }
    #endif  /* MA_NO_RESOURCE_MANAGER */
    on_error_2:
        for (iListener = 0; iListener < pEngine->listenerCount; iListener += 1) {
            ma_spatializer_listener_uninit(&pEngine->listeners[iListener], &pEngine->allocationCallbacks);
        }
        ma_node_graph_uninit(&pEngine->nodeGraph, &pEngine->allocationCallbacks);
    on_error_1:
        #if !defined(MA_NO_DEVICE_IO)
        {
            if (pEngine->ownsDevice) {
                ma_device_uninit(pEngine->pDevice);
                ma_free(pEngine->pDevice, &pEngine->allocationCallbacks);
            }
        }
        #endif
        return result;
}

void AudioInit(){       
    ma_result result;
    ma_engine_config engineConfig;
    engineConfig = ma_engine_config_init();
    engineConfig.channels   = audio.channels;
    engineConfig.sampleRate = audio.sample_rate;
    if(audio.sink_title != "\0") {
        result = ma_engine_mod_init(&engineConfig, &audio.engine,audio.sink_title);
    } else {
        result = ma_engine_mod_init(&engineConfig, &audio.engine,window.title);
    }
    if (result != MA_SUCCESS) {
        printf("Audio Engine initialization failed");
    }
}

void AudioVolume(float value){
    ma_engine_set_volume(&audio.engine,value);
}

float GetAudioVolume(){
    return ma_engine_get_volume(&audio.engine);
}

void AudioPlay(char *file){
    ma_engine_play_sound(&audio.engine, file, NULL);
}

void AudioStop() {
    if(&audio.engine){ma_engine_uninit(&audio.engine);}
}

// Sound loading

Sound* SoundLoad(char *file) {
    Sound* sound = (Sound*)malloc(sizeof(Sound));
    if (sound == NULL) {
        printf("Failed to allocate memory for Sound\n");
        return NULL;
    }
    ma_result result = ma_sound_init_from_file(&audio.engine, file, 0, NULL, NULL,&sound->ma);
    if (result != MA_SUCCESS) {
        printf("Failed to load sound: %s\n", file);
        free(sound);
        return NULL;
    }
    return sound;
}

void SoundPlay(Sound* sound) {
    ma_sound_start(&sound->ma);
}

void SoundStop(Sound* sound) {
    ma_sound_stop(&sound->ma);
}

void SetSoundStartTime(Sound* sound, ma_uint64 time) {
    ma_sound_set_start_time_in_milliseconds(&sound->ma, time);
}

void SetSoundEndTime(Sound* sound, ma_uint64 time) {
    ma_sound_set_stop_time_in_milliseconds(&sound->ma, time);
}

ma_uint64 GetSoundTime(Sound* sound) {
    return ma_sound_get_time_in_milliseconds(&sound->ma);
}

bool GetSoundEnd(Sound* sound) {
    return ma_sound_at_end(&sound->ma);
}

bool GetSoundPlaying(Sound* sound) {
    return ma_sound_is_playing(&sound->ma);
}

float GetSoundPitch(Sound* sound) {
    return ma_sound_get_pitch(&sound->ma);
}

void SetSoundPitch(Sound* sound, float value) {
    ma_sound_set_pitch(&sound->ma, value);
}

void SetSoundPitchSemitones(Sound* sound, float semitones) {
    float pitchMultiplier = pow(2.0f, semitones / 12.0f);
    ma_sound_set_pitch(&sound->ma, pitchMultiplier);
}

float GetSoundPan(Sound* sound) {
    return ma_sound_get_pan(&sound->ma);
}

void SetSoundPan(Sound* sound, float value) {
    ma_sound_set_pan(&sound->ma, value);
}

bool GetSoundLoop(Sound* sound) {
    return ma_sound_is_looping(&sound->ma);
}

void SetSoundLoop(Sound* sound, bool value) {
    ma_sound_set_looping(&sound->ma, value);
}

void SoundSetPositioning(ma_sound* pSound, ma_positioning positioning) {
    ma_sound_set_positioning(pSound, positioning);
}

ma_positioning SoundGetPositioning(const ma_sound* pSound) {
    return ma_sound_get_positioning(pSound);
}

void SoundSetPosition(ma_sound* pSound, float x, float y, float z) {
    ma_sound_set_position(pSound, x, y, z);
}

ma_vec3f SoundGetPosition(const ma_sound* pSound) {
    return ma_sound_get_position(pSound);
}

void SoundSetDirection(ma_sound* pSound, float x, float y, float z) {
    ma_sound_set_direction(pSound, x, y, z);
}

ma_vec3f SoundGetDirection(const ma_sound* pSound) {
    return ma_sound_get_direction(pSound);
}

void SoundSetVelocity(ma_sound* pSound, float x, float y, float z) {
    ma_sound_set_velocity(pSound, x, y, z);
}

ma_vec3f SoundGetVelocity(const ma_sound* pSound) {
    return ma_sound_get_velocity(pSound);
}

void SoundEnableSpatialization(ma_sound* pSound) {
    ma_sound_set_spatialization_enabled(pSound, MA_TRUE);
}

void SoundDisableSpatialization(ma_sound* pSound) {
    ma_sound_set_spatialization_enabled(pSound, MA_FALSE);
}

void SoundSetCone(ma_sound* pSound, float innerAngleInRadians, float outerAngleInRadians, float outerGain) {
    ma_sound_set_cone(pSound, innerAngleInRadians, outerAngleInRadians, outerGain);
}

void SoundGetCone(const ma_sound* pSound, float* pInnerAngleInRadians, float* pOuterAngleInRadians, float* pOuterGain) {
    ma_sound_get_cone(pSound, pInnerAngleInRadians, pOuterAngleInRadians, pOuterGain);
}

void SoundSetDopplerFactor(ma_sound* pSound, float dopplerFactor) {
    ma_sound_set_doppler_factor(pSound, dopplerFactor);
}

float SoundGetDopplerFactor(const ma_sound* pSound) {
    return ma_sound_get_doppler_factor(pSound);
}

void SoundSetStopTimeWithFadeInMilliseconds(ma_sound* pSound, ma_uint64 stopAbsoluteGlobalTimeInMilliseconds, ma_uint64 fadeLengthInMilliseconds) {
    ma_sound_set_stop_time_with_fade_in_milliseconds(pSound, stopAbsoluteGlobalTimeInMilliseconds, fadeLengthInMilliseconds);
}

ma_result SoundStopWithFadeInMilliseconds(ma_sound* pSound, ma_uint64 fadeLengthInFrames) {
    return ma_sound_stop_with_fade_in_milliseconds(pSound, fadeLengthInFrames);
}

void SoundSetFadeStartInMilliseconds(ma_sound* pSound, float volumeBeg, float volumeEnd, ma_uint64 fadeLengthInMilliseconds, ma_uint64 absoluteGlobalTimeInMilliseconds) {
    ma_sound_set_fade_start_in_milliseconds(pSound, volumeBeg, volumeEnd, fadeLengthInMilliseconds, absoluteGlobalTimeInMilliseconds);
}

void SoundSetFadeInMilliseconds(ma_sound* pSound, float volumeBeg, float volumeEnd, ma_uint64 fadeLengthInMilliseconds) {
    ma_sound_set_fade_in_milliseconds(pSound, volumeBeg, volumeEnd, fadeLengthInMilliseconds);
}

float SoundGetCurrentFadeVolume(const ma_sound* pSound) {
    return ma_sound_get_current_fade_volume(pSound);
}

void SoundSetAttenuationModel(ma_sound* pSound, ma_attenuation_model attenuationModel) {
    ma_sound_set_attenuation_model(pSound, attenuationModel);
}

ma_attenuation_model SoundGetAttenuationModel(const ma_sound* pSound) {
    return ma_sound_get_attenuation_model(pSound);
}

void SoundSetRolloff(ma_sound* pSound, float rolloff) {
    ma_sound_set_rolloff(pSound, rolloff);
}

float SoundGetRolloff(const ma_sound* pSound) {
    return ma_sound_get_rolloff(pSound);
}

void SoundSetMinGain(ma_sound* pSound, float minGain) {
    ma_sound_set_min_gain(pSound, minGain);
}

float SoundGetMinGain(const ma_sound* pSound) {
    return ma_sound_get_min_gain(pSound);
}

void SoundSetMaxGain(ma_sound* pSound, float maxGain) {
    ma_sound_set_max_gain(pSound, maxGain);
}

float SoundGetMaxGain(const ma_sound* pSound) {
    return ma_sound_get_max_gain(pSound);
}

void SoundSetMinDistance(ma_sound* pSound, float minDistance) {
    ma_sound_set_min_distance(pSound, minDistance);
}

float SoundGetMinDistance(const ma_sound* pSound) {
    return ma_sound_get_min_distance(pSound);
}

void SoundSetMaxDistance(ma_sound* pSound, float maxDistance) {
    ma_sound_set_max_distance(pSound, maxDistance);
}

float SoundGetMaxDistance(const ma_sound* pSound) {
    return ma_sound_get_max_distance(pSound);
}

void SoundSetPinnedListenerIndex(ma_sound* pSound, ma_uint32 listenerIndex) {
    ma_sound_set_pinned_listener_index(pSound, listenerIndex);
}

ma_uint32 SoundGetPinnedListenerIndex(const ma_sound* pSound) {
    return ma_sound_get_pinned_listener_index(pSound);
}

ma_uint32 SoundGetListenerIndex(const ma_sound* pSound) {
    return ma_sound_get_listener_index(pSound);
}

ma_vec3f SoundGetDirectionToListener(const ma_sound* pSound) {
    return ma_sound_get_direction_to_listener(pSound);
}
