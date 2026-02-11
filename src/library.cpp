#include "../include/library.h"
#include <filesystem>
#include <iostream>

void Library::ScanDirectory(const std::string &directory) {
  namespace fs = std::filesystem;

  for (const auto &entry : fs::recursive_directory_iterator(directory)) {
    if (entry.is_regular_file()) {
      std::string ext = entry.path().extension().string();
      if (ext == ".mp3" || ext == ".flac" || ext == ".wav" || ext == ".ogg") {
        Song song = GetSongMetadata(entry.path().string());
        songs.push_back(song);
      }
    }
  }
}

Song Library::GetSongMetadata(const std::string &filepath) {
  Song song;
  song.filepath = filepath;

  TagLib::FileRef file(filepath.c_str());

  if (!file.isNull() && file.tag()) {
    TagLib::Tag *tag = file.tag();
    song.title = tag->title().isEmpty()
                     ? std::filesystem::path(filepath).stem().string()
                     : tag->title().toCString(true);
    song.artist = tag->artist().toCString(true);
    song.album = tag->album().toCString(true);
  } else {
    // Fallback to filename
    song.title = std::filesystem::path(filepath).stem().string();
  }

  if (file.audioProperties()) {
    song.duration_seconds = file.audioProperties()->lengthInSeconds();
  }

  return song;
}

Library::Library() { ScanDirectory(music_folder); }

std::vector<Song> Library::GetAllSongs() const { return songs; }

Song *Library::FindSongByTitle(const std::string &title) {
  for (auto &song : songs) {
    if (song.title == title) {
      return &song;
    }
  }
  return nullptr;
}

Song *Library::FindSongByDisplayName(const std::string &display_name) {
  for (auto &song : songs) {
    if (song.GetDisplayName() == display_name) {
      return &song;
    }
  }
  return nullptr;
}

std::string Library::FetchSongPathFromTitle(const std::string &title) {
  Song *song = FindSongByTitle(title);
  if (song) {
    return song->filepath;
  }

  song = FindSongByDisplayName(title);
  if (song) {
    return song->filepath;
  }

  return ""; // Not found
}
