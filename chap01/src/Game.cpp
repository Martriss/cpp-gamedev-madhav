#include "Game.h"
#include <SDL_blendmode.h>
#include <SDL_render.h>

Game::Game()
    : mWindow(nullptr), mIsRunning(true),
      mBallPos({windowWidth / 2, windowHeight / 2}),
      mPaddlePos({10, windowHeight / 2}), mTicksCount(0), mPaddleDir(0),
      mBallVel({-200.0f, 235.0f}) {}

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

  // Paddle controls
  mPaddleDir = 0;
  if (state[SDL_SCANCODE_W]) {
    mPaddleDir -= 1;
  }
  if (state[SDL_SCANCODE_S]) {
    mPaddleDir += 1;
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

  // Paddle movement
  if (mPaddleDir != 0) {
    mPaddlePos.y += mPaddleDir * paddleSpeed * deltaTime;

    if (mPaddlePos.y < (paddleH / 2.0f + thickness)) {
      mPaddlePos.y = paddleH / 2.0f + thickness;
    } else if (mPaddlePos.y > (windowHeight - paddleH / 2.0f - thickness)) {
      mPaddlePos.y = windowHeight - paddleH / 2.0f - thickness;
    }
  }

  // Ball movement
  mBallPos.x += mBallVel.x * deltaTime;
  mBallPos.y += mBallVel.y * deltaTime;

  // Paddle collision
  float diff = mPaddlePos.y - mBallPos.y;
  diff = (diff > 0.0f) ? diff : -diff;

  if (diff <= paddleH / 2.0f && mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
      mBallVel.x < 0.0f) {
    mBallVel.x *= -1.0f;
  } else if (mBallPos.x < 0.0f) {
    mIsRunning = false;
  }

  // Top collision
  if (mBallPos.y <= thickness && mBallVel.y < 0.0f) {
    mBallVel.y *= -1.0f;
    // Bottom collision
  } else if (mBallPos.y >= (windowHeight - thickness) && mBallVel.y > 0.0f) {
    mBallVel.y *= -1.0f;
    // Right collision
  } else if (mBallPos.x >= (windowWidth - thickness) && mBallVel.x > 0.0f) {
    mBallVel.x *= -1.0f;
  }
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

  // Objects color
  SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);

  // Top wall
  SDL_Rect wall{0, 0, static_cast<int>(windowWidth), thickness};
  SDL_RenderFillRect(mRenderer, &wall);

  // Bottom wall
  wall.y = windowHeight - thickness;
  SDL_RenderFillRect(mRenderer, &wall);

  // Right wall
  wall.x = windowWidth - thickness;
  wall.y = 0;
  wall.w = thickness;
  wall.h = windowWidth;
  SDL_RenderFillRect(mRenderer, &wall);

  // Ball
  SDL_Rect ball{static_cast<int>(mBallPos.x - thickness / 2.0f),
                static_cast<int>(mBallPos.y - thickness / 2.0f), thickness,
                thickness};
  SDL_RenderFillRect(mRenderer, &ball);

  // Paddle
  SDL_Rect paddle{static_cast<int>(mPaddlePos.x - thickness / 2.0f),
                  static_cast<int>(mPaddlePos.y - paddleH / 2.0f), thickness,
                  paddleH};
  SDL_RenderFillRect(mRenderer, &paddle);

  // Buffer swap
  SDL_RenderPresent(mRenderer);
}
