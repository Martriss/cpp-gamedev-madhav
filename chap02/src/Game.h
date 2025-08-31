#pragma once
#include <SDL.h>

constexpr float windowWidth{1024.0f};
constexpr float windowHeight{768.0f};
constexpr float targetFrameTime{16.0f};

class Game {
public:
  Game();
  // Initialize the game
  bool Initialize();
  // Runs the game loop until the game is over
  void RunLoop();
  // Shutdown the game
  void Shutdown();

private:
  // Helper functions for the game loop
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  // Window created by SDL
  SDL_Window *mWindow;
  // Game should continue to run
  bool mIsRunning;
  SDL_Renderer *mRenderer;
  Uint32 mTicksCount;

};
