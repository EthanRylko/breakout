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
    sf::Vector2f block_to_ball = {m_position.x - block_pos.x, m_position.y - block_pos.y};

    std::cout << "m_angle before: " << m_angle;


    // TODO: Replace with acute angle between two lines formula (arctan)
    double dot = block_to_ball.x * m_velocity.x + block_to_ball.y * m_velocity.y;
    double arc_cos = dot / (block_to_ball.length() * m_velocity.length());
    arc_cos = std::max(-1.0, std::min(1.0, arc_cos));

    double angle = acos(arc_cos);
    angle = (angle > M_PI / 2.0 ? M_PI - angle : angle);
    std::cout << " found angle: " << angle;
    std::cout << " block to ball: " << block_to_ball.angle().asRadians();

    // TODO: Figure out retroreflection problem
    if (m_angle > block_to_ball.angle().asRadians()) {
      m_angle = block_to_ball.angle().asRadians() - angle;
      std::cout << " negative ting ";
    } else {
      m_angle = block_to_ball.angle().asRadians() - angle;
      std::cout << " positive ting ";
    }
    std::cout << " m_angle after: " << m_angle << std::endl;

    ResetVelocity();
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
      collision_type collision = BlockCollision(*block.second);
      if (collision != collision_type::NONE) {
        to_delete.insert(block.first);
        switch(collision) {
          case collision_type::VERTICAL:
            m_velocity.y *= -1;
            ResetAngle();
            break;
          case collision_type::HORIZONTAL:
            m_velocity.x *= -1;
            ResetAngle();
            break;
          case collision_type::CORNER:
            //CalculateCornerCollision(*block.second);
            CalculateCornerCollision(*block.second);
            std::cout << "TING" << std::endl;
            break;
          default:
            // should be impossible, but IDE throws warning
            break;
        }
        break;
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
  double GetAngle() {
    return m_angle;
  }
};