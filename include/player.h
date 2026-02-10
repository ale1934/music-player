#pragma once
#include "library.h"
#include "miniaudio.h"

class Player {
  private:
    std::string currently_playing;
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

    std::string GetCurrentSong();
    std::vector<std::string> GetSongsInQueue();
    
    bool IsPlaying();
    void PlaySong(const std::string& song_name);
    void PauseOrResume();
    void AddSongToQueue(std::string& song_name);
    void Loop(int times);
    void NextSong();
};
