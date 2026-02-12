#pragma once
#include "library.h"
#include "miniaudio.h"

class Player {
private:
  Song currentSong;
  std::vector<std::string> queue;
  int position_in_queue;
  bool song_paused;
  bool sound_initialized;
  ma_sound sound;
  ma_engine engine;
  Library lib;

public:
  Player(Library l);
  ~Player();

  Song *GetCurrentSongObject();
  std::vector<std::string> GetSongsInQueue();

  bool IsPlaying();
  void PlaySong(const Song &song);
  void PauseOrResume();
  void PlaySongByPath(const std::string &filepath);
  void AddSongToQueue(const Song &song_name);
  void Loop(int times);
  void NextSong();

  uint64_t GetCurrentVelocityNormalized();
  uint64_t GetCurrentPositionInFrames();
  float GetCurrentPositionInSeconds();
  float GetSongLengthInSeconds();
  std::string FormatTime(float seconds);
  void SeekToPosition(float seconds);
  bool IsPaused() { return song_paused; }
};
