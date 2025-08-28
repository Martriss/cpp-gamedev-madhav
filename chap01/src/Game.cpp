#include "Game.h"

Game::Game()
    : mWindow{nullptr}, mIsRunning{true}, mLeftPaddlePos{10, windowHeight / 2},
      mRightPaddlePos{windowWidth - 10, windowHeight / 2}, mTicksCount{0},
      mLeftPaddleDir{0}, mRightPaddleDir{0}, mBalls{},
      mRandomEngine{std::random_device{}()}, mVelXDist{-300.0f, 300.0f},
      mVelYDist{-200.0f, 200.0f} {}

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

  mBalls.reserve(ballsNumber);

  const Vector2 center{windowWidth / 2.0f, windowHeight / 2.0f};
  for (int i = 0; i < ballsNumber; ++i) {
    mBalls.emplace_back(Ball{center, RandomVelocity()});
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
  mLeftPaddleDir = 0;
  if (state[SDL_SCANCODE_W]) {
    mLeftPaddleDir -= 1;
  }
  if (state[SDL_SCANCODE_S]) {
    mLeftPaddleDir += 1;
  }

  mRightPaddleDir = 0;
  if (state[SDL_SCANCODE_I]) {
    mRightPaddleDir -= 1;
  }
  if (state[SDL_SCANCODE_K]) {
    mRightPaddleDir += 1;
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

  // Paddle movement
  if (mLeftPaddleDir != 0) {
    mLeftPaddlePos.y += mLeftPaddleDir * paddleSpeed * deltaTime;

    if (mLeftPaddlePos.y < (paddleH / 2.0f + thickness)) {
      mLeftPaddlePos.y = paddleH / 2.0f + thickness;
    } else if (mLeftPaddlePos.y > (windowHeight - paddleH / 2.0f - thickness)) {
      mLeftPaddlePos.y = windowHeight - paddleH / 2.0f - thickness;
    }
  }
  if (mRightPaddleDir != 0) {
    mRightPaddlePos.y += mRightPaddleDir * paddleSpeed * deltaTime;

    if (mRightPaddlePos.y < (paddleH / 2.0f + thickness)) {
      mRightPaddlePos.y = paddleH / 2.0f + thickness;
    } else if (mRightPaddlePos.y >
               (windowHeight - paddleH / 2.0f - thickness)) {
      mRightPaddlePos.y = windowHeight - paddleH / 2.0f - thickness;
    }
  }

  for (auto it = mBalls.begin(); it != mBalls.end();) {
    Ball &ball = *it;

    // Ball movement
    ball.pos.x += ball.vel.x * deltaTime;
    ball.pos.y += ball.vel.y * deltaTime;

    // Remove out of bounds balls
    if (ball.pos.x < 0.0f || ball.pos.x > windowWidth) {
      it = mBalls.erase(it);
      continue;
    }

    // Left paddle collision
    float leftDiff = mLeftPaddlePos.y - ball.pos.y;
    leftDiff = (leftDiff > 0.0f) ? leftDiff : -leftDiff;

    if (leftDiff <= paddleH / 2.0f && ball.pos.x <= 25.0f &&
        ball.pos.x >= 20.0f && ball.vel.x < 0.0f) {
      ball.vel.x *= -1.0f;
    }

    // Right paddle collision
    float rightDiff = mRightPaddlePos.y - ball.pos.y;
    rightDiff = (rightDiff > 0.0f) ? rightDiff : -rightDiff;

    if (rightDiff <= paddleH / 2.0f && ball.pos.x >= windowWidth - 25.0f &&
        ball.pos.x <= windowWidth - 20.0f && ball.vel.x > 0.0f) {
      ball.vel.x *= -1.0f;
    }

    // Top collision
    if (ball.pos.y <= thickness && ball.vel.y < 0.0f) {
      ball.vel.y *= -1.0f;
      // Bottom collision
    } else if (ball.pos.y >= (windowHeight - thickness) && ball.vel.y > 0.0f) {
      ball.vel.y *= -1.0f;
    }

    ++it;
  }

  // Ball intercollisions
  if (activateCollision) {
    for (size_t i = 0; i < mBalls.size(); ++i) {
      for (size_t j = i + 1; j < mBalls.size(); ++j) {
        Ball &ball1 = mBalls[i];
        Ball &ball2 = mBalls[j];

        float dx = ball1.pos.x - ball2.pos.x;
        float dy = ball1.pos.y - ball2.pos.y;
        float distanceSquared = dx * dx + dy * dy;
        float minDist = thickness;

        if (distanceSquared < minDist * minDist) {
          float distance = std::sqrt(distanceSquared);

          // Prevent by 0 division
          if (distance < 1.0f)
            distance = 1.0f;

          // Separate balls
          float overlap = (minDist - distance) / 2.0f;
          float separateX = (dx / distance) * overlap;
          float separateY = (dy / distance) * overlap;

          ball1.pos.x += separateX;
          ball1.pos.y += separateY;
          ball2.pos.x -= separateX;
          ball2.pos.y -= separateY;

          // Swap velocities
          float tempVelX = ball1.vel.x;
          float tempVelY = ball1.vel.y;
          ball1.vel.x = ball2.vel.x;
          ball1.vel.y = ball2.vel.y;
          ball2.vel.x = tempVelX;
          ball2.vel.y = tempVelY;
        }
      }
    }
  }

  if (mBalls.empty()) {
    mIsRunning = false;
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

  // Ball
  for (auto &ball : mBalls) {
    SDL_Rect ballRect{static_cast<int>(ball.pos.x - thickness / 2.0f),
                      static_cast<int>(ball.pos.y - thickness / 2.0f),
                      thickness, thickness};
    SDL_RenderFillRect(mRenderer, &ballRect);
  }
  // Paddle
  SDL_Rect leftPaddle{static_cast<int>(mLeftPaddlePos.x - thickness / 2.0f),
                      static_cast<int>(mLeftPaddlePos.y - paddleH / 2.0f),
                      thickness, paddleH};
  SDL_RenderFillRect(mRenderer, &leftPaddle);

  SDL_Rect rightPaddle{static_cast<int>(mRightPaddlePos.x - thickness / 2.0f),
                       static_cast<int>(mRightPaddlePos.y - paddleH / 2.0f),
                       thickness, paddleH};
  SDL_RenderFillRect(mRenderer, &rightPaddle);

  // Buffer swap
  SDL_RenderPresent(mRenderer);
}

Vector2 Game::RandomVelocity() {
  float velX = mVelXDist(mRandomEngine);
  if (std::abs(velX) < 200.0f) {
    velX = (velX >= 0) ? 200.0f : -200.0f;
  }

  return {velX, mVelYDist(mRandomEngine)};
}
