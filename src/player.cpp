#include "../include/player.h"

Player::Player(Library l) {
  position_in_queue = 0;
  song_paused = false;
  sound_initialized = false; 
  currently_playing = "";   
  lib = l;

  ma_engine_init(NULL, &engine);
}

// Get songs & queue
std::string Player::GetCurrentSong() { return currently_playing; }
std::vector<std::string> Player::GetSongsInQueue() { return queue; }

// Returns whether or not a song is currently playing
bool Player::IsPlaying() {
  if (!sound_initialized)  // Check this first!
    return false;
  return ma_sound_is_playing(&sound);
}

// Pause if song is currently playing or resume if song is paused
void Player::PauseOrResume() {
  // If not playing a song then just return
  if (currently_playing == "" || !sound_initialized)
    return;

  if (song_paused == true) {
    ma_sound_start(&sound);
    song_paused = false;
  } else {
    ma_sound_stop(&sound);
    song_paused = true;
  }
}

void Player::PlaySong(const std::string& song_name) {
  // Use library to get file path
  std::string file_path = lib.FetchSongFromName(song_name.c_str());
 
  // Bad path then exit function
  if (file_path.empty()) {
    return;
  }
 
  if (sound_initialized) {
    ma_sound_uninit(&sound);
  }
  
  // Initialize sound player
  ma_result result = ma_sound_init_from_file(&engine, file_path.c_str(), 
                                             0, NULL, NULL, &sound);
  if (result != MA_SUCCESS) {
    sound_initialized = false;
    return;
  }
  
  sound_initialized = true;
  currently_playing = song_name;
  song_paused = false;
  ma_sound_start(&sound);
}

void Player::AddSongToQueue(std::string& song_name) {
  queue.push_back(song_name);
  
  // If nothing is playing, start playing the first song
  if (currently_playing == "") {
    PlaySong(song_name);
  } 
}

// Destroy sound player & engine
Player::~Player() {
  if (sound_initialized) {
    ma_sound_uninit(&sound);
  }
  ma_engine_uninit(&engine);
}
