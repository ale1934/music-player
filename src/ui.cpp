#include "../include/ui.h"
#include <cmath>
#include <ftxui/component/component.hpp>
#include <thread>

void DrawSpiralGalaxy(Canvas &c, int frame, int width, int height) {
  if (width <= 0 || height <= 0)
    return;

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

    auto button = Button(display_name, [this, i] {
      player.PlaySong(songs[i]);
      current_song_index = i;
    });

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
    songs.push_back(button->Render() | center | size(HEIGHT, EQUAL, 5));
    songs.push_back(separatorLight());
  }

  return vbox({text("Library") | bold |center, separator(),
               vbox(std::move(songs)) | yframe | flex}) |
         size(WIDTH, EQUAL, 36) | border;
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

  auto dynamicAlbumArt =
      Renderer([this, currentPos](bool focused) {
        return canvas([this, currentPos](Canvas &c) {
          if (c.width() > 0 && c.height() > 0) {
            DrawSpiralGalaxy(c, currentPos, c.width(), c.height());
          }
        });
      }) |
      size(WIDTH, GREATER_THAN, 10) | size(HEIGHT, GREATER_THAN, 5);

  return vbox({text("Now Playing") | bold | center, separator(), text(""),
               text(currentSong.GetDisplayName()) | center,
               text(currentSong.artist) | dim | center,
               text(currentSong.album) | dim | center, text(""),
               hbox({text(player.FormatTime(currentPos)), separator(),
                     gauge(progress) | flex, separator(),
                     text(player.FormatTime(totalLength))}),
               text(""), separator(), dynamicAlbumArt->Render() | flex}) |
         border | flex;
}
void MusicPlayerTUI::Run() {
  auto MainScreen = Renderer(libraryContainer, [&] {
    auto dims = Terminal::Size();

    if (show_library && dims.dimx >= 80) {
      return hbox(
          {RenderLibraryTab(), separator(), RenderNowPlayingTab() | flex});
    } else {
      return RenderNowPlayingTab() | flex;
    }
  });

  // Wrap with event handler for 'l' key
  auto MainWithEvents = CatchEvent(MainScreen, [&](Event event) {
    if (event == Event::Character('l') || event == Event::Character('L')) {
      show_library = !show_library;
      return true;
    }

    if (event == Event::ArrowLeft) {
      float cur_time = player.GetCurrentPositionInSeconds();

      if (cur_time - 5.0f > 0)
        player.SeekToPosition(cur_time - 5.0f);
      else
        player.SeekToPosition(0.0f);
      return true;
    }

    if (event == Event::ArrowRight) {
      float cur_time = player.GetCurrentPositionInSeconds();

      if (cur_time + 5.0f < player.GetSongLengthInSeconds())
        player.SeekToPosition(cur_time + 5.0f);
      else {
        if (current_song_index + 1 < songs.size())
          current_song_index++;
        else
          current_song_index = 0;
        player.PlaySong(songs[current_song_index]);
      }

      return true;
    }

    if (event == Event::Character(' ')) {
      player.PauseOrResume();

      return true;
    }

    return false;
  });

  std::thread update_thread([this] { UpdateLoop(); });
  screen.Loop(MainWithEvents);

  // Clean up
  running = false;
  if (update_thread.joinable()) {
    update_thread.join();
  }
}

MusicPlayerTUI::~MusicPlayerTUI() { running = false; }
