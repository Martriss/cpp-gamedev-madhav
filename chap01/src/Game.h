#pragma once
#include <SDL.h>
#include <random>

// Game constants
constexpr int thickness{15};
constexpr int paddleH{100};
constexpr int ballsNumber{50};
constexpr float windowWidth{1024.0f};
constexpr float windowHeight{768.0f};
constexpr float paddleSpeed{300.0f};
constexpr float targetFrameTime{16.0f};
constexpr bool activateCollision{true};

struct Vector2 {
  float x;
  float y;
};

struct Ball {
  Vector2 pos;
  Vector2 vel;
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
  Vector2 RandomVelocity();

  // Window created by SDL
  SDL_Window *mWindow;
  // Game should continue to run
  bool mIsRunning;
  SDL_Renderer *mRenderer;
  Vector2 mLeftPaddlePos;
  Vector2 mRightPaddlePos;
  Uint32 mTicksCount;
  int mLeftPaddleDir;
  int mRightPaddleDir;
  std::vector<Ball> mBalls;
  std::mt19937 mRandomEngine;
  std::uniform_real_distribution<float> mVelXDist;
  std::uniform_real_distribution<float> mVelYDist;
};
