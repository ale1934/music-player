#ifndef UI_H
#define UI_H

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>
#include "player.h"
#include "library.h"

using namespace ftxui;

class MusicPlayerTUI {
private:
  Player& player;
  Library& library;
  
  int selected_tab;
  int selected_library_song;
  int selected_queue_song;
  std::string search_query;
  bool show_search;
  
  // Components
  Component container;
  Component tab_toggle;
  
  std::vector<std::string> GetFilteredLibrary();
  Element RenderLibraryTab();
  Element RenderNowPlayingTab();
  Element RenderSearchOverlay(Element base);
  void SetupComponents();
  
public:
  MusicPlayerTUI(Player& p, Library& l);
  void Run();
};

#endif
