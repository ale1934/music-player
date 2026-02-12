#include "../include/downloader.h"
#include <cstdlib>

void Downloader::Download(std::string ytlink, std::string songname, Library l) {
  std::string cmd = "python ~/Projects/music-player/src/downloader.py " +
                    ytlink + " " + songname + " " + l.GetCurrentDir().string();
  std::system(cmd.c_str());
}
