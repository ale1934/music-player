#pragma once
#include <filesystem>
#include <string>
#include <taglib/fileref.h>
#include <taglib/taglib.h>
#include <vector>

using namespace std::filesystem;

struct Song {
  std::string filepath;
  std::string title;
  std::string artist;
  std::string album;
  int duration_seconds;

  std::string GetDisplayName() const {
    if (!artist.empty()) {
      return artist + " - " + title;
    }
    return title;
  }
};

class Library {
private:
  std::vector<Song> songs;
  path music_folder = "/home/ale/Music/";

public:
  Library();

  void ScanDirectory(const std::string &directory);
  Song GetSongMetadata(const std::string &filepath);

  std::vector<Song> GetAllSongs() const;
  Song *FindSongByTitle(const std::string &title);
  Song *FindSongByDisplayName(const std::string &display_name);
  std::string FetchSongPathFromTitle(const std::string &title);
};
