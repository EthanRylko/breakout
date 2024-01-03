#pragma once

#include "SFML/Graphics.hpp"
#include "constants.h"

/**
 * Player class
 */
class Player {
private:
  sf::RectangleShape m_shape; /// Shape that represents the player
  sf::Vector2f m_position;    /// Position of the player

public:
  /**
   * Default constructor
   */
  Player() {
    m_position = {400, 500};
    m_shape.setSize({PLAYER_WIDTH, PLAYER_HEIGHT});
    m_shape.setOrigin({PLAYER_HALF_WIDTH, 0});
    m_shape.setFillColor(PLAYER_COLOR);
    m_shape.setPosition(m_position);
  }

  /**
   * Move player
   * @param pos mouse position
   */
  void Move(const sf::Vector2i &pos) {
    m_position.x = pos.x;
    m_shape.setPosition(m_position);
  }

  /**
   * Draw player to window
   * @param window window to draw on
   */
  void Draw(sf::RenderWindow &window) {
    window.draw(m_shape);
  }

  /**
   * Return position of player for collision purposes
   * @return position of player
   */
  sf::Vector2f GetPosition() const {
    return m_position;
  }
};