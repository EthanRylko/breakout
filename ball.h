#pragma once

#include "SFML/Graphics.hpp"
#include "block.h"
#include "constants.h"
#include "player.h"

#include <cmath>
#include <set>

/**
 * Ball class
 */
class Ball {
private:
  sf::CircleShape m_shape;  /// Shape that represents the ball
  sf::Vector2f m_position;  /// Position of the ball
  sf::Vector2f m_velocity;  /// Velocity of the ball
  double m_angle;           /// Angle at which the ball moves

  /**
   * Reset ball velocity based on angle
   */
  void ResetVelocity() {
    m_velocity.x = cos(m_angle) * BALL_SPEED;
    m_velocity.y = sin(m_angle) * BALL_SPEED;
  }

  /**
   * Reset ball angle based on velocity
   */
  void ResetAngle() {
    m_angle = atan2(m_velocity.y, m_velocity.x);
  }

  /**
   * Collide with edges of screen and change angle
   */
  void EdgeCollision() {
    // top edge collision
    if (m_position.y - BALL_RADIUS <= 0) {
      m_velocity.y *= -1;
      ResetAngle();
    }

    // side edge collision
    if (m_position.x - BALL_RADIUS <= 0 ||
        m_position.x + BALL_RADIUS >= WINDOW_WIDTH) {
      m_velocity.x *= -1;
      ResetAngle();
    }
  }

  /**
   * Check for collision with block in grid
   * @param block block from grid
   * @return true if collision
   */
  bool BlockCollision(const Block &block) const {
    float dx = abs(block.GetPosition().x - m_position.x);
    float dy = abs(block.GetPosition().y - m_position.y);
    if (dx > BLOCK_HALF_SIZE + BALL_RADIUS) return false;
    if (dy > BLOCK_HALF_SIZE + BALL_RADIUS) return false;
    if (dx <= BLOCK_HALF_SIZE) return true;
    if (dy <= BLOCK_HALF_SIZE) return true;
    return false;
  }

public:

  /**
   * Default constructor
   */
  Ball() {
    m_position = {400, 490};
    m_shape.setRadius(BALL_RADIUS);
    m_shape.setOrigin({BALL_RADIUS, BALL_RADIUS});
    m_shape.setFillColor(BALL_COLOR);
    m_shape.setPosition(m_position);
    m_angle = -M_PI / 2.0f;
    ResetVelocity();
  }

  /**
   * Move ball based on velocity
   */
  void Move() {
    m_position.x += m_velocity.x;
    m_position.y += m_velocity.y;
    EdgeCollision();

    m_shape.setPosition(m_position);
  }

  /**
   * Collision check with player, set velocity
   * @param player player
   */
  void PlayerCollision(const Player &player) {
    sf::Vector2f player_pos = player.GetPosition();

    // TODO: Come up with better collision detection
    if (m_position.y >= player_pos.y - BALL_RADIUS &&
        m_velocity.y > 0) {
      if (m_position.x > player_pos.x - PLAYER_HALF_WIDTH &&
          m_position.x < player_pos.x + PLAYER_HALF_WIDTH) {
        // linear mapping of ball relative to player to [-pi, 0]
        m_angle = M_PI / PLAYER_WIDTH * (m_position.x - player_pos.x) - M_PI / 2.0f;
        ResetVelocity();
      }
    }

  }

  /**
   * Collision check with all blocks in grid
   * @param grid grid of blocks
   */
  void GridCollision(std::map<uint32_t, std::shared_ptr<Block>> &grid) {
    // has the ball collided with a block?
    std::set<uint32_t> to_delete;
    for (const auto &block : grid) {
      if (BlockCollision(*block.second)) {
        to_delete.insert(block.first);
        // TODO: Calculate new angle, velocity
      }
    }

    // if ball collided, then delete block
    for (uint32_t id : to_delete) {
      grid.erase(id);
    }
  }

  /**
   * Draw ball to window
   * @param window window to draw on
   */
  void Draw(sf::RenderWindow &window) {
    window.draw(m_shape);
  }

  /**
   * Check if ball is out of bounds (bottom) for deletion
   * @return true if out of bounds
   */
  bool OutOfBounds() const {
    return (m_position.y >= WINDOW_HEIGHT);
  }
};