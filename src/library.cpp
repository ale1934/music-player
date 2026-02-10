#include "../include/library.h"
#include <filesystem>
#include <iostream>

void LoadLibrary(std::vector<std::string> &songs,
                 std::string directory) {
  songs.clear();
  
  // Check if directory exists
  if (!exists(directory)) {
    std::cerr << "Music directory doesn't exist: " << directory << std::endl;
    return;
  }
  
  for (const auto& entry: directory_iterator(directory)) {
    if (entry.is_regular_file()) {  // Only add files, not directories
      songs.push_back(entry.path().filename().string());
    }
  } 
}

Library::Library() {
  num_of_songs = 0;

  LoadLibrary(songs, music_folder);
  num_of_songs = songs.size();
}

std::string Library::FetchSongFromName(std::string name) {
  return music_folder + name; 
}

std::vector<std::string> Library::GetAllSongs() {
  return songs;
}

void Library::AddSongToLibrary(path file_path) {
  std::string new_path = music_folder 
                         + (std::string)file_path.filename(); 
  rename(file_path.c_str(), new_path.c_str());
}

void Library::DeleteSongFromLibrary(std::string name) {
  for (int i = 0; i < songs.size(); i++) 
    if (songs[i] == name) {
      remove((music_folder + name).c_str());
      songs.erase(songs.begin() + i);
    }
}

void Library::ChangeMusicFolder(std::string directory) {
  music_folder = directory;
}
