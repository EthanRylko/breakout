#pragma once

#include "SFML/Graphics.hpp"
#include "block.h"
#include "constants.h"

typedef std::map<uint32_t, std::shared_ptr<Block>> block_map;

/**
 * Grid class
 */
class Grid {
private:
  sf::Vector2f m_origin;      /// Origin of the grid
  block_map m_blocks;
  int m_width;
  int m_height;

  /**
   * Load grid from file
   * @param filename name of file
   */
  void Load(const std::string &filename)
  {
    std::ifstream file(filename);
    uint8_t read_width, read_height;
    file >> read_width >> read_height;
    m_width = read_width;
    m_height = read_height;

    for (uint32_t i = 0; i < m_width * m_height; i += 2) {
      uint8_t data_pair, id_1, id_2;
      file >> data_pair;
      id_1 = (data_pair >> 4);
      id_2 = (data_pair & 0b00001111);

      if (id_1 != 0) {
        m_blocks[i] = std::make_shared<Block>(i % m_width, i / m_width, id_1);
      }

      if (id_2 != 0) {
        m_blocks[i + 1] = std::make_shared<Block>((i + 1) % m_width, (i + 1) / m_width, id_2);
      }
    }
  }

public:

    /**
     * Default constructor
     * @param filename name of file to load data from
     */
    Grid(const std::string &filename) {
      m_origin = {0, 0};
      Load(filename);
    }

    /**
     * Draw grid to window
     * @param window window to draw on
     */
    void Draw(sf::RenderWindow &window) const {
      for (const auto &block : m_blocks) {
        block.second->Draw(window);
      }
    }

    /**
     * Get position for collision purposes
     * @return origin of grid
     */
    sf::Vector2f GetOrigin() const {
      return m_origin;
    }

    /**
     * Get subset of blocks around position
     * @param pos position
     * @return subset of blocks
     */
    block_map GetSubset(const sf::Vector2f &pos) {
      block_map subset;
      int x = static_cast<int>(pos.x / BLOCK_SIZE_TOTAL);
      int y = static_cast<int>(pos.y / BLOCK_SIZE_TOTAL);

      for (int dy = -1; dy < 1; dy++) {
        if (y + dy < 0 || y + dy >= m_height) continue;

        for (int dx = -1; dx < 1; dx++) {
          if (x + dx < 0 || x + dx >= m_width) continue;
          uint32_t id = (x + dx) + (y + dy) * m_width;
          if (m_blocks.contains(id)) {
            subset[id] = m_blocks[id];
          }
        }
      }

      return subset;
    }

    /**
     * Remove block at ID
     * @param id ID
     */
    void Remove(uint32_t id) {
      m_blocks.erase(id);
    }
};