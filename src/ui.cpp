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
      int y = cy + r * std::sin(angle) * 0.5;

      if (x >= 0 && x < width && y >= 0 && y < height) {
        c.DrawPoint(x, y, true);
      }
    }
  }
}

void DrawPlasmaWave(Canvas &c, int frame, int width, int height) {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      float value = std::sin(x * 0.05 + frame * 0.1) +
                    std::sin(y * 0.05 + frame * 0.15) +
                    std::sin((x + y) * 0.03 + frame * 0.08);

      if (value > 0.5) {
        c.DrawPoint(x, y, true);
      }
    }
  }
}

void DrawWaveform(Canvas &c, int frame, int width, int height) {
  int cy = height / 2;

  for (int x = 0; x < width; x++) {
    float y1 = cy + 15 * std::sin((x + frame) * 0.1);

    c.DrawPoint(x, (int)y1, true);
  }
}

void DrawParticleExplosion(Canvas &c, int frame, int width, int height,
                           std::vector<std::array<float, 4>> &particles) {
  int cx = width / 2;
  int cy = height / 2;

  if (particles.empty() || frame % 60 == 0) {
    particles.clear();
    for (int i = 0; i < 150; i++) {
      float angle = (rand() % 1000) / 1000.0f * 2 * M_PI;
      float speed = (rand() % 100 + 50) / 100.0f;
      particles.push_back({(float)cx, (float)cy, speed * std::cos(angle),
                           speed * std::sin(angle) * 0.5f});
    }
  }

  for (auto &p : particles) {
    p[0] += p[2];
    p[1] += p[3];

    if (p[0] >= 0 && p[0] < width && p[1] >= 0 && p[1] < height) {
      c.DrawPoint((int)p[0], (int)p[1], true);
    }
  }
}

void DrawDNAHelix(Canvas &c, int frame, int width, int height) {
  int cy = height / 2;

  for (int x = 0; x < width; x++) {
    float t = (x + frame) * 0.1;
    int y1 = cy + 15 * std::sin(t);
    int y2 = cy - 15 * std::sin(t);

    c.DrawPoint(x, y1, true);
    c.DrawPoint(x, y2, true);

    // Connect strands
    if ((int)(t / M_PI) % 2 == 0 && x % 5 == 0) {
      for (int y = std::min(y1, y2); y <= std::max(y1, y2); y += 2) {
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

  return vbox({text("Library") | bold | center, separator(),
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
            switch (visualizer_style) {
            case 0:
              DrawDNAHelix(c, currentPos, c.width(), c.height());
              return;
            case 1:
              DrawParticleExplosion(c, currentPos, c.width(), c.height(),
                                    particles);
              return;
            case 2:
              DrawPlasmaWave(c, currentPos, c.width(), c.height());
              return;
            case 3:
              DrawWaveform(c, currentPos, c.width(), c.height());
              return;
            case 4:
              DrawSpiralGalaxy(c, currentPos, c.width(), c.height());
              return;
            default:
              return;
            };
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
  std::string yt_content = "";
  std::string yt_placeholder = "Enter yt link here";
  Component yt_input = Input({
      .content = &yt_content,
      .placeholder = &yt_placeholder,
  });

  libraryContainer->Add(yt_input);

  auto MainScreen = Renderer(libraryContainer, [&] {
    auto dims = Terminal::Size();

    if (show_library && dims.dimx >= 80) {
      return hbox(
          {RenderLibraryTab(), separator(), RenderNowPlayingTab() | flex});
    } else {
      return vbox({RenderNowPlayingTab() | flex, yt_input->Render() | border});
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

    if (event == Event::Character('1')) {
      visualizer_style = 0;
      return true;
    }

    if (event == Event::Character('2')) {
      visualizer_style = 1;
      return true;
    }

    if (event == Event::Character('3')) {
      visualizer_style = 2;
      return true;
    }

    if (event == Event::Character('4')) {
      visualizer_style = 3;
      return true;
    }

    if (event == Event::Character('5')) {
      visualizer_style = 4;
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
