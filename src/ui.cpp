#include "../include/ui.h"
#include <cmath>
#include <ftxui/component/component.hpp>
#include <thread>

void DrawSpiralGalaxy(Canvas &c, int frame, int width, int height) {
  int cx = width / 2;
  int cy = height / 2;

  for (float r = 0; r < 80; r += 0.5) {
    for (int arm = 0; arm < 5; arm++) {
      float angle = r * 0.2 + arm * (2 * M_PI / 5) + frame * 0.02;
      int x = cx + r * std::cos(angle);
      int y =
          cy + r * std::sin(angle) * 0.5; // Squash for terminal aspect ratio

      if (x >= 0 && x < width && y >= 0 && y < height) {
        c.DrawPoint(x, y, true);
      }
    }
  }
}

MusicPlayerTUI::MusicPlayerTUI(Player &p, Library &l)
    : player(p), library(l), screen(ScreenInteractive::Fullscreen()) {
  CreateLibraryButtons();
}

void MusicPlayerTUI::CreateLibraryButtons() {
  songButtons.clear();
  songs = library.GetAllSongs();

  for (size_t i = 0; i < songs.size(); i++) {
    std::string display_name = songs[i].GetDisplayName();

    auto button =
        Button(display_name, [this, i] { player.PlaySong(songs[i]); });

    songButtons.push_back(button);
  }

  libraryContainer = Container::Vertical(songButtons);
}

void MusicPlayerTUI::UpdateLoop() {
  using namespace std::chrono_literals;

  while (running) {
    std::this_thread::sleep_for(100ms);
    screen.Post(Event::Custom);
  }
}

Element MusicPlayerTUI::RenderLibraryTab() {
  Elements songs;

  for (auto &button : songButtons) {
    songs.push_back(button->Render() | center | size(HEIGHT, EQUAL, 5) | xflex);
    songs.push_back(separatorLight());
  }

  return vbox(std::move(songs)) | yframe | border | size(WIDTH, EQUAL, 40);
}

Element MusicPlayerTUI::RenderNowPlayingTab() {
  Song *currentSongPtr = player.GetCurrentSongObject();

  if (!currentSongPtr) {
    return vbox({text("Nothing Playing") | center}) | border | flex;
  }

  Song currentSong = *currentSongPtr;

  float currentPos = player.GetCurrentPositionInSeconds();
  float totalLength = player.GetSongLengthInSeconds();
  float progress = totalLength > 0 ? currentPos / totalLength : 0.0f;

  auto albumArt = Canvas(100, 50);
  DrawSpiralGalaxy(albumArt, currentPos, 100, 50);

  return vbox({text("Now Playing") | bold | center, separator(), text(""),
               text(currentSong.GetDisplayName()) | center,
               text(currentSong.artist) | dim | center,
               text(currentSong.album) | dim | center, text(""),
               hbox({text(player.FormatTime(currentPos)), separator(),
                     gauge(progress) | flex, separator(),
                     text(player.FormatTime(totalLength))}),
               text(""), separator(),
               hbox({// TODO
                     canvas(std::move(albumArt))}) |
                   center}) |
         border | flex;
}
void MusicPlayerTUI::Run() {
  auto MainScreen = Renderer(libraryContainer, [&] {
    return hbox(
        {RenderLibraryTab(), separator(), RenderNowPlayingTab() | flex});
  });

  std::thread update_thread([this] { UpdateLoop(); });

  screen.Loop(MainScreen);

  // Clean up
  running = false;
  if (update_thread.joinable()) {
    update_thread.join();
  }
}

MusicPlayerTUI::~MusicPlayerTUI() { running = false; }
