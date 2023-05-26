#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>

/**
 * @brief The AudioPlayer class handles audio playback using SDL2 and SDL2_mixer.
 * @see   https://www.libsdl.org/
 */
class AudioPlayer {
public:
    /**
     * @brief Constructs an AudioPlayer object and initializes SDL audio subsystem.
     *        Opens the audio device for playback.
     */
    AudioPlayer();

    /**
     * @brief Destructs the AudioPlayer object and closes the audio device.
     */
    ~AudioPlayer();

    /**
     * @brief Constructs an AudioPlayer object as a copy of another AudioPlayer object.
     * @param other The AudioPlayer object to be copied.
     */
    AudioPlayer(const AudioPlayer& other);

    /**
     * @brief Assigns the contents of one AudioPlayer object to another.
     * @param other The AudioPlayer object to be assigned from.
     * @return A reference to the assigned AudioPlayer object.
     */
    AudioPlayer& operator=(const AudioPlayer& other);

    /**
     * @brief Constructs an AudioPlayer object by moving the resources from another AudioPlayer object.
     * @param other The AudioPlayer object to be moved from.
     */
    AudioPlayer(AudioPlayer&& other) noexcept;

    /**
     * @brief Moves the resources from one AudioPlayer object to another.
     * @param other The AudioPlayer object to be moved from.
     * @return A reference to the moved AudioPlayer object.
     */
    AudioPlayer& operator=(AudioPlayer&& other) noexcept;

    /**
     * @brief Loads an audio file for playback.
     * @param filePath The path to the audio file.
     * @return True if the audio file is successfully loaded, false otherwise.
     */
    bool loadAudio(const std::string& filePath);

    void play();
    void stop();
    void pause();
    void resume();

private:
    Mix_Music* music_ = nullptr; /**< Pointer to the loaded audio data. */
    std::string filePath_;       /**< Path of the loaded audio file. */
};

AudioPlayer::AudioPlayer() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL audio: " << SDL_GetError() << std::endl;
    } else {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "Failed to open audio: " << Mix_GetError() << std::endl;
        }
    }
}

AudioPlayer::~AudioPlayer() {
    Mix_CloseAudio();
    SDL_Quit();
}

AudioPlayer::AudioPlayer(const AudioPlayer& other) {
    if (other.music_) {
        filePath_ = other.filePath_;
        music_ = Mix_LoadMUS(filePath_.c_str());
        if (!music_) {
            std::cerr << "Failed to load audio: " << Mix_GetError() << std::endl;
        }
    }
}

AudioPlayer& AudioPlayer::operator=(const AudioPlayer& other) {
    if (this != &other) {
        Mix_FreeMusic(music_);
        if (other.music_) {
            filePath_ = other.filePath_;
            music_ = Mix_LoadMUS(filePath_.c_str());
            if (!music_) {
                std::cerr << "Failed to load audio: " << Mix_GetError() << std::endl;
            }
        }
    }
    return *this;
}

AudioPlayer::AudioPlayer(AudioPlayer&& other) noexcept
    : music_(other.music_) {
    other.music_ = nullptr;
}

AudioPlayer& AudioPlayer::operator=(AudioPlayer&& other) noexcept {
    if (this != &other) {
        Mix_FreeMusic(music_);
        music_ = other.music_;
        other.music_ = nullptr;
    }
    return *this;
}

bool AudioPlayer::loadAudio(const std::string& filePath) {
    music_ = Mix_LoadMUS(filePath.c_str());
    if (!music_) {
        std::cerr << "Failed to load audio: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void AudioPlayer::play() {
    if (music_) {
        if (Mix_PlayMusic(music_, 1) < 0) {
            std::cerr << "Failed to play audio: " << Mix_GetError() << std::endl;
        }
    }
}

void AudioPlayer::stop() {
    Mix_HaltMusic();
}

void AudioPlayer::pause() {
    Mix_PauseMusic();
}

void AudioPlayer::resume() {
    Mix_ResumeMusic();
}

#endif  // AUDIOPLAYER_H
