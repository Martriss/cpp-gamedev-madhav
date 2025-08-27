#include "Game.h"
#include <SDL_video.h>

Game::Game() : mWindow(nullptr), mIsRunning(true) {}

bool Game::Initialize() {

  int sdlResult = SDL_Init(SDL_INIT_VIDEO);
  if (sdlResult != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return false;
  }

  mWindow = SDL_CreateWindow("Game Programming in C++ (Chapter 01)",
                             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             1024, 768, SDL_WINDOW_SHOWN);

  if (!mWindow) {
    SDL_Log("Failed to create window: %s", SDL_GetError());
    return false;
  }

  SDL_ShowWindow(mWindow);
  SDL_RaiseWindow(mWindow);

  return true;
}

void Game::Shutdown() {
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
    if (event.type == SDL_QUIT) {
      mIsRunning = false;
    }
  }
}

void Game::UpdateGame() {
  // TODO: implement
}

void Game::GenerateOutput() {
  // SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);
  // SDL_RenderClear(mRenderer);
  // SDL_RenderPresent(mRenderer);
}
