#pragma once
#include <SDL.h>

// Game constants
constexpr int thickness{15};
constexpr int paddleH{100};
constexpr float windowWidth{1024.0f};
constexpr float windowHeight{768.0f};
constexpr float paddleSpeed{300.0f};
constexpr float targetFrameTime{16.0f};

struct Vector2 {
  float x;
  float y;
};

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
  Vector2 mPaddlePos;
  Vector2 mBallPos;
  Uint32 mTicksCount;
  int mPaddleDir;
  Vector2 mBallVel;
};
