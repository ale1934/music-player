#include "../include/ui.h"

int main() {
  Library lib;

  Player player(lib);

  MusicPlayerTUI tui(player, lib);
  tui.Run();

  return 0;
}
