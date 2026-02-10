#pragma once
#include <string>
#include <vector>
#include <filesystem>

using namespace std::filesystem;

class Library {
  private:
    std::vector<std::string> songs;
    std::string music_folder = "/home/ale/Music/";
    int num_of_songs;
  public:
    Library();

    std::string FetchSongFromName(std::string name);
    std::vector<std::string> GetAllSongs();
    
    void AddSongToLibrary(path file_path);
    void DeleteSongFromLibrary(std::string name);
    void ChangeMusicFolder(std::string directory);
};
