#pragma once

#include "SFML/Graphics.hpp"
#include "block.h"
#include "constants.h"
#include "player.h"
#include "grid.h"

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
   * @return type of collision
   */
  collision_type BlockCollision(const Block &block) const {
    sf::Vector2f block_pos = block.GetPosition();
    float dx = abs(block_pos.x - m_position.x);
    float dy = abs(block_pos.y - m_position.y);

    if (dx > BLOCK_HALF_SIZE_TOTAL + BALL_RADIUS) return collision_type::NONE;
    if (dy > BLOCK_HALF_SIZE_TOTAL + BALL_RADIUS) return collision_type::NONE;

    if (m_position.x <= block_pos.x + BLOCK_HALF_SIZE_TOTAL &&
        m_position.x >= block_pos.x - BLOCK_HALF_SIZE_TOTAL) {
      return collision_type::VERTICAL;
    }

    if (m_position.y <= block_pos.y + BLOCK_HALF_SIZE_TOTAL &&
        m_position.y >= block_pos.y - BLOCK_HALF_SIZE_TOTAL) {
      return collision_type::HORIZONTAL;
    }

    double corner_dist = pow(dx - BLOCK_HALF_SIZE_TOTAL, 2) + pow(dy - BLOCK_HALF_SIZE_TOTAL, 2);
    if (corner_dist <= pow(BALL_RADIUS, 2)) return collision_type::CORNER;

    return collision_type::NONE;
  }

  /**
   * Calculate new angle and velocity after corner collision
   * @param block block from grid
   */
  void CalculateCornerCollision(const Block &block) {
    sf::Vector2f block_pos = block.GetPosition();
    sf::Vector2f block_to_ball = (m_position - block_pos).normalized();
    float dot = block_to_ball.dot(m_velocity);

    // reflect through normal vector (block center to ball center)
    m_velocity -= 2.0f * dot * block_to_ball;
    ResetAngle();
  }

  /**
   * Correct ball position after collision
   * @param collision type of collision
   * @param block block in collision
   */
  void SnapBall(const collision_type &collision, const Block &block) {
    sf::Vector2f block_pos = block.GetPosition();
    if (collision == collision_type::HORIZONTAL) {
      float edge_x;
      if (m_position.x < block_pos.x) {
        edge_x = block_pos.x - BLOCK_HALF_SIZE_TOTAL - BALL_RADIUS;
      } else {
        edge_x = block_pos.x + BLOCK_HALF_SIZE_TOTAL + BALL_RADIUS;
      }
      float dx = edge_x - m_position.x;
      m_position.x += 2 * dx;
    }
    if (collision == collision_type::VERTICAL) {
      float edge_y;
      if (m_position.y < block_pos.y) {
        edge_y = block_pos.y - BLOCK_HALF_SIZE_TOTAL - BALL_RADIUS;
      } else {
        edge_y = block_pos.y + BLOCK_HALF_SIZE_TOTAL + BALL_RADIUS;
      }
      float dy = edge_y - m_position.y;
      m_position.y += 2 * dy;
    }
  }

  /**
   * Set new velocity/angle based on collision with block
   * @param collision type of collision
   * @param block block in collision
   */
  void HandleCollision(const collision_type &collision, const Block &block) {
    switch(collision) {
      case collision_type::VERTICAL:
        SnapBall(collision, block);
        m_velocity.y *= -1;
        ResetAngle();
        break;
      case collision_type::HORIZONTAL:
        SnapBall(collision, block);
        m_velocity.x *= -1;
        ResetAngle();
        break;
      case collision_type::CORNER:
        CalculateCornerCollision(block);
        break;
      default:
        // should be impossible, but IDE throws warning
        break;
    }
  }

public:
  /**
   * Default constructor
   */
  Ball() {
    m_position = {400, PLAYER_Y - BALL_RADIUS};
    m_shape.setRadius(BALL_RADIUS);
    m_shape.setOrigin({BALL_RADIUS, BALL_RADIUS});
    m_shape.setFillColor(BALL_COLOR);
    m_shape.setPosition(m_position);
    m_angle = -M_PI / 2.0f;
    ResetVelocity();
  }

  /**
   * Duplication constructor
   * @param pos position
   * @param angle angle of movement
   */
  Ball(sf::Vector2f pos, double angle) {
    m_position = pos;
    m_shape.setRadius(BALL_RADIUS);
    m_shape.setOrigin({BALL_RADIUS, BALL_RADIUS});
    m_shape.setFillColor(BALL_COLOR);
    m_shape.setPosition(m_position);
    m_angle = angle;
    ResetVelocity();
  }

  /**
   * Move ball based on velocity
   */
  void Move() {
    m_position.x += m_velocity.x;
    m_position.y += m_velocity.y;
    EdgeCollision();
  }

  /**
   * Collision check with player, set velocity
   * @param player player
   */
  void PlayerCollision(const Player &player) {
    sf::Vector2f player_pos = player.GetPosition();

    // TODO: Come up with better collision detection
    if (m_position.y >= player_pos.y - BALL_RADIUS &&
        m_position.y <= player_pos.y + BALL_RADIUS) {
      if (m_position.x > player_pos.x - PLAYER_HALF_WIDTH &&
          m_position.x < player_pos.x + PLAYER_HALF_WIDTH) {
        // linear mapping of ball relative to player to [-pi, 0]
        // then fix range to [-9/10 pi, -1/10 pi]
        m_angle = M_PI / PLAYER_WIDTH * (m_position.x - player_pos.x) - M_PI / 2.0f;
        m_angle = std::max(-9 * M_PI / 10.0, std::min(-M_PI / 10.0, m_angle));
        ResetVelocity();
      }
    }
  }

  /**
   * Collision check with all blocks in grid
   * @param grid grid of blocks
   */
  void GridCollision(Grid &grid) {
    // narrow search
    block_map subset = grid.GetSubset(m_position);
    if (subset.empty()) return;

    // find nearest block
    Block *nearest;
    float min_dist = -1;
    uint32_t nearest_id = 0;

    for (const auto &block : subset) {
      float dist = (block.second->GetPosition() - m_position).length();
      if (min_dist == -1 || dist < min_dist) {
        min_dist = dist;
        nearest_id = block.first;
        nearest = block.second.get();
      }
    }

    // has collision happened?
    collision_type collision = BlockCollision(*nearest);
    if (collision != collision_type::NONE) {
      HandleCollision(collision, *nearest);
      if (nearest->GetBreakable()) grid.Remove(nearest_id);
    }
  }

  /**
   * Draw ball to window
   * @param window window to draw on
   */
  void Draw(sf::RenderWindow &window) {
    m_shape.setPosition(m_position);
    window.draw(m_shape);
  }

  /**
   * Check if ball is out of bounds (bottom) for deletion
   * @return true if out of bounds
   */
  bool OutOfBounds() const {
    return (m_position.y >= WINDOW_HEIGHT);
  }

  /**
   * Get position of ball
   * @return position
   */
  sf::Vector2f GetPosition() const {
    return m_position;
  }

  /**
   * Get angle that ball is travelling
   * @return angle
   */
  double GetAngle() const {
    return m_angle;
  }
};