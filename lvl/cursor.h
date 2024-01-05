#pragma once

class Cursor {
private:
  uint8_t m_id = 0;           /// ID of currently selected tile
  sf::Vector2f m_position;    /// position on screen
  sf::RectangleShape m_shape; /// visual representation of tile

public:
  /**
   * Default constructor
   */
  Cursor() {
    m_shape.setSize({BLOCK_SIZE, BLOCK_SIZE});
    m_shape.setOrigin({BLOCK_HALF_SIZE_TOTAL, BLOCK_HALF_SIZE_TOTAL});
    m_shape.setPosition(m_position);
    SetID(m_id);
  }

  /**
   * Set cursor tile ID
   * @param id tile ID
   */
  void SetID(uint8_t id) {
    m_id = id;
    m_shape.setFillColor(GetColor());
  }

  /**
   * Get cursor block ID
   * @return block ID
   */
  uint8_t GetID() const {
    return m_id;
  }

  /**
   * Set position of cursor
   * @param position mouse position
   */
  void SetPosition(sf::Vector2f position) {
    m_position = position;
    m_shape.setPosition(position);
  }

  /**
   * Get position of cursor
   * @return cursor position
   */
  sf::Vector2f GetPosition() const {
    return m_position;
  }

  /**
   * Draw cursor to window
   * @param window window to draw on
   */
  void Draw(sf::RenderWindow &window) const {
    window.draw(m_shape);
  }

  /**
   * Get color of cursor block
   * @return color
   */
  sf::Color GetColor() {
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