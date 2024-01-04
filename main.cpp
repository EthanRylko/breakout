#include <iostream>
#include <memory>
#include <map>
#include <set>
#include "SFML/Graphics.hpp"

#include "ball.h"
#include "block.h"
#include "constants.h"
#include "player.h"

void MultiplyBalls(std::map<uint32_t, std::shared_ptr<Ball>> &balls, uint32_t &ball_id) {
  std::map<uint32_t, std::shared_ptr<Ball>> temp;
  for (auto ball : balls) {
    double angle = ball.second->GetAngle() + M_PI / 2.0;
    temp[ball_id++] = std::make_shared<Ball>(ball.second->GetPosition(), angle);
    temp[ball_id++] = std::make_shared<Ball>(ball.second->GetPosition(), angle + M_PI / 2.0);
    temp[ball_id++] = std::make_shared<Ball>(ball.second->GetPosition(), angle + M_PI);
  }

  for (auto ball : temp) {
    balls[ball.first] = ball.second;
  }
}

int main() {
  sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Breakout");
  window.setFramerateLimit(FRAME_RATE);

  Player player;

  // init set of balls
  std::map<uint32_t, std::shared_ptr<Ball>> balls;
  uint32_t ball_id = 0;
  balls[ball_id++] = std::make_shared<Ball>();
  std::set<uint32_t> to_delete;

  // init grid of blocks
  std::map<uint32_t, std::shared_ptr<Block>> grid;
  for (int i = 0; i < GRID_WIDTH; i++) {
    for (int j = 0; j < GRID_HEIGHT; j++) {
      std::shared_ptr<Block> block = std::make_shared<Block>(i, j);
      grid[i * GRID_WIDTH + j] = block;
    }
  }

  bool click_lock = false;

  // Game loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left && !click_lock) {
          click_lock = true;
          MultiplyBalls(balls, ball_id);
        }
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          click_lock = false;
        }
      }
    }

    // Deal with player
    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
    player.Move(mousePosition);

    // Deal with balls
    for (const auto &ball : balls) {
      ball.second->Move();
      ball.second->PlayerCollision(player);
      ball.second->GridCollision(grid);
      if (ball.second->OutOfBounds()) {
        to_delete.insert(ball.first);
      }
    }

    // delete balls if out of bounds
    for (uint32_t id : to_delete) {
      balls.erase(id);
    }
    to_delete.clear();

    // draw step
    window.clear(BACKGROUND_COLOR);
    player.Draw(window);
    for (const auto &ball : balls) {
      ball.second->Draw(window);
    }
    for (const auto &block : grid) {
      block.second->Draw(window);
    }
    window.display();

    // lose condition
    if (balls.empty()) {
      std::cout << "Game over!" << std::endl;
      window.close();
    }

    // win condition
    if (grid.empty()) {
      std::cout << "You win!" << std::endl;
      window.close();
    }
  }

  return 0;
}