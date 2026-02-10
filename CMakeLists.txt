cmake_minimum_required(VERSION 3.23)
project(music_player)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch FTXUI
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG main
)
FetchContent_MakeAvailable(ftxui)

# Add all your source files
add_executable(main 
  src/main.cpp
  src/player.cpp
  src/library.cpp
  src/ui.cpp
  src/miniaudio.cpp
)

# Include directories for headers
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE pthread
  PRIVATE m
  PRIVATE dl
)
