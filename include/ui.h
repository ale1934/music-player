#ifndef UI_H
#define UI_H
#include "library.h"
#include "player.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

using namespace ftxui;

// ui.h
class MusicPlayerTUI {
private:
  Player &player;
  Library &library;
  ScreenInteractive screen;

  std::vector<Song> songs;
  std::vector<Component> songButtons;
  Component libraryContainer;

  std::atomic<bool> running{true};

  void CreateLibraryButtons();
  void UpdateLoop();
  Element RenderLibraryTab();
  Element RenderNowPlayingTab();

public:
  MusicPlayerTUI(Player &p, Library &l);
  ~MusicPlayerTUI();
  void Run();
};

#endif
