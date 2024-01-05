#pragma once

#include "SFML/Graphics.hpp"
#include "constants.h"

/**
 * Block class
 */
class Block {
private:
  sf::RectangleShape m_shape;   /// Shape that represent the block
  sf::Vector2f m_position;      /// Position of the block
  uint8_t m_id = 0;             /// ID of block

public:

  /**
   * Default constructor
   * @param x x grid value (converts to real position)
   * @param y y grid value (converts to real position)
   */
  Block(int x, int y, int id) {
    m_position.x = (x + 0.5f) * BLOCK_SIZE_TOTAL + BLOCK_SPACING;
    m_position.y = (y + 0.5f) * BLOCK_SIZE_TOTAL + BLOCK_SPACING;
    m_id = id;
    m_shape.setSize({BLOCK_SIZE, BLOCK_SIZE});
    m_shape.setOrigin({BLOCK_HALF_SIZE_TOTAL, BLOCK_HALF_SIZE_TOTAL});
    m_shape.setPosition(m_position);
    m_shape.setFillColor(GetColor());
  }

  /**
   * Draw block to window
   * @param window window to draw on
   */
  void Draw(sf::RenderWindow &window) const {
    window.draw(m_shape);
  }

  /**
   * Get position for collision purposes
   * @return position of block
   */
  sf::Vector2f GetPosition() const {
    return m_position;
  }

  /**
   * Get block ID
   * @return ID
   */
  uint8_t GetID() const {
    return m_id;
  }

  /**
   * Get color of block
   * @return color
   */
  sf::Color GetColor() const {
    uint8_t r = ((m_id >> 2) & 1) * 255;
    uint8_t g = ((m_id >> 1) & 1) * 255;
    uint8_t b = ((m_id) & 1) * 255;

    sf::Color color;
    if ((m_id >> 3) & 1) {
      color = {127, 127, 127};
    } else {
      color = {r, g, b};
    }
    return color;
  }
};