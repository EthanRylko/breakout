#include <iostream>
#include <fstream>
#include <memory>
#include <map>
#include <set>
#include "SFML/Graphics.hpp"

#include "ball.h"
#include "block.h"
#include "constants.h"
#include "grid.h"
#include "player.h"

/**
 * Multiply balls power up
 * @param balls set of currently active balls
 * @param ball_id current ball ID
 */
void MultiplyBalls(std::map<uint32_t, std::shared_ptr<Ball>> &balls, uint32_t &ball_id) {
  std::map<uint32_t, std::shared_ptr<Ball>> temp;
  for (const auto &ball : balls) {
    double angle = ball.second->GetAngle();
    temp[ball_id++] = std::make_shared<Ball>(ball.second->GetPosition(), angle + 2.0f * M_PI / 3.0);
    temp[ball_id++] = std::make_shared<Ball>(ball.second->GetPosition(), angle - 2.0f * M_PI / 3.0);
  }

  for (const auto &ball : temp) {
    balls[ball.first] = ball.second;
  }
}

/**
 * Main function
 * @return success
 */
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
  Grid grid("lvl/001.bin");

  // Game loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          MultiplyBalls(balls, ball_id);
        }
      }
    }

    // Deal with player
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    player.Move(window.mapPixelToCoords(mouse_pos));

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
    grid.Draw(window);
    window.display();

    // lose condition
    if (balls.empty()) {
      std::cout << "Game over!" << std::endl;
      window.close();
    }

    // win condition
    if (grid.Finished()) {
      std::cout << "You win!" << std::endl;
      window.close();
    }
  }

  return 0;
}