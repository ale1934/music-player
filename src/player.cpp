#include "../include/player.h"
#include <iostream>

Player::Player(Library l) : lib(l) {
  position_in_queue = 0;
  song_paused = false;
  sound_initialized = false;
  currentSong = Song{"", "Nothing Is Playing", "", "", 0}; // Empty song
  ma_engine_init(NULL, &engine);
}

uint64_t Player::GetCurrentPositionInFrames() {
  if (!sound_initialized)
    return 0;
  ma_uint64 cursor;
  ma_sound_get_cursor_in_pcm_frames(&sound, &cursor);
  return cursor;
}

float Player::GetCurrentPositionInSeconds() {
  if (!sound_initialized)
    return 0.0f;
  ma_uint64 cursor;
  ma_sound_get_cursor_in_pcm_frames(&sound, &cursor);
  ma_uint32 sampleRate;
  ma_sound_get_data_format(&sound, NULL, NULL, &sampleRate, NULL, 0);
  return (float)cursor / (float)sampleRate;
}

float Player::GetSongLengthInSeconds() {
  if (!sound_initialized)
    return 0.0f;
  ma_uint64 lengthInFrames;
  ma_sound_get_length_in_pcm_frames(&sound, &lengthInFrames);
  ma_uint32 sampleRate;
  ma_sound_get_data_format(&sound, NULL, NULL, &sampleRate, NULL, 0);
  return (float)lengthInFrames / (float)sampleRate;
}

std::string Player::FormatTime(float seconds) {
  int minutes = (int)seconds / 60;
  int secs = (int)seconds % 60;
  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%d:%02d", minutes, secs);
  return std::string(buffer);
}

Song *Player::GetCurrentSongObject() { return &currentSong; }

std::vector<std::string> Player::GetSongsInQueue() { return queue; }

bool Player::IsPlaying() {
  if (!sound_initialized)
    return false;
  return ma_sound_is_playing(&sound);
}

void Player::PauseOrResume() {
  if (currentSong.title.empty() || !sound_initialized)
    return;
  if (song_paused) {
    ma_sound_start(&sound);
    song_paused = false;
  } else {
    ma_sound_stop(&sound);
    song_paused = true;
  }
}

void Player::PlaySong(const Song &song) {
  if (song.filepath.empty()) {
    return;
  }

  if (sound_initialized) {
    ma_sound_uninit(&sound);
  }

  ma_result result = ma_sound_init_from_file(&engine, song.filepath.c_str(), 0,
                                             NULL, NULL, &sound);
  if (result != MA_SUCCESS) {
    sound_initialized = false;
    return;
  }

  sound_initialized = true;
  currentSong = song; // Store complete song metadata
  song_paused = false;
  ma_sound_start(&sound);
}

void Player::PlaySongByPath(const std::string &filepath) {
  if (filepath.empty()) {
    return;
  }

  if (sound_initialized) {
    ma_sound_uninit(&sound);
  }

  ma_result result =
      ma_sound_init_from_file(&engine, filepath.c_str(), 0, NULL, NULL, &sound);
  if (result != MA_SUCCESS) {
    sound_initialized = false;
    return;
  }

  sound_initialized = true;
  currentSong.filepath = filepath;
  song_paused = false;
  ma_sound_start(&sound);
}

void Player::AddSongToQueue(const Song &song) {
  queue.push_back(song.title);
  if (currentSong.title.empty()) {
    PlaySong(song);
  }
}

Player::~Player() {
  if (sound_initialized) {
    ma_sound_uninit(&sound);
  }
  ma_engine_uninit(&engine);
}
