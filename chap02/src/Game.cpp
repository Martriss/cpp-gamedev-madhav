#include "Game.h"

Game::Game() : mWindow{nullptr}, mIsRunning{true}, mTicksCount{0} {}

bool Game::Initialize() {

  int sdlResult = SDL_Init(SDL_INIT_VIDEO);
  if (sdlResult != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return false;
  }

  mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 01)",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             windowWidth, windowHeight, 0);

  if (!mWindow) {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    return false;
  }

  SDL_ShowWindow(mWindow);
  SDL_RaiseWindow(mWindow);

  mRenderer = SDL_CreateRenderer(
      mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (!mRenderer) {
    SDL_Log("Failed to create renderer: %s", SDL_GetError());
    return false;
  }

  return true;
}

void Game::Shutdown() {
  SDL_DestroyRenderer(mRenderer);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}

void Game::RunLoop() {
  while (mIsRunning) {
    ProcessInput();
    UpdateGame();
    GenerateOutput();
  }
}

void Game::ProcessInput() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      mIsRunning = false;
      break;
    }
  }

  const Uint8 *state = SDL_GetKeyboardState(NULL);
  if (state[SDL_SCANCODE_ESCAPE]) {
    mIsRunning = false;
  }
}

void Game::UpdateGame() {
  // Frame limiting: at 60fps each frame is ~16ms
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + targetFrameTime))
    ;

  // Calculate delta time: the diff in tick between current and last frame
  float deltaTime{(SDL_GetTicks() - mTicksCount) / 1000.0f};

  // Clamp delta time
  if (deltaTime > 0.05f) {
    deltaTime = 0.05f;
  }

  mTicksCount = SDL_GetTicks();
}

void Game::GenerateOutput() {
  // Background
  SDL_SetRenderDrawColor(mRenderer,
                         0,   // R
                         0,   // G
                         255, // B
                         255  // A
  );
  SDL_RenderClear(mRenderer);

  // Buffer swap
  SDL_RenderPresent(mRenderer);
}
