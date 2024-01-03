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

public:

  /**
   * Default constructor
   * @param x x grid value (converts to real position)
   * @param y y grid value (converts to real position)
   */
  Block(int x, int y) {
    m_position.x = (x + 0.5f) * BLOCK_SIZE_TOTAL + BLOCK_SPACING;
    m_position.y = (y + 0.5f) * BLOCK_SIZE_TOTAL + BLOCK_SPACING;
    m_shape.setSize({BLOCK_SIZE, BLOCK_SIZE});
    m_shape.setOrigin({BLOCK_HALF_SIZE_TOTAL, BLOCK_HALF_SIZE_TOTAL});
    m_shape.setPosition(m_position);
    int color_idx = x + y;
    uint8_t r = COLOR_ARRAY[color_idx % 15];
    uint8_t g = COLOR_ARRAY[(color_idx + 10) % 15];
    uint8_t b = COLOR_ARRAY[(color_idx + 5) % 15];
    sf::Color color(r, g, b);
    m_shape.setFillColor(color);
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
};