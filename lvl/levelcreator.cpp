#include <iostream>
#include <fstream>
#include <map>

#include "SFML/graphics.hpp"
#include "../block.h"
#include "../constants.h"
#include "cursor.h"

/**
 * Save data to file
 * @param grid grid of blocks
 * @param filename name of file
 * @param width width of grid
 * @param height height of grid
 */
void Save(const std::map<uint32_t, std::shared_ptr<Block>> &grid,
          const std::string &filename,
          const int &width,
          const int &height)
{
  std::ofstream file(filename);
  file << static_cast<uint8_t>(width) << static_cast<uint8_t>(height);
  for (uint32_t i = 0; i < grid.size(); i += 2) {
    uint8_t data_pair = grid.at(i)->GetID();
    data_pair = (data_pair << 4) | grid.at(i + 1)->GetID();
    file << data_pair;
  }
}

/**
 * Load data from file
 * @param grid grid of blocks reference
 * @param filename name of file
 * @param width width of grid reference
 * @param height height of grid reference
 */
void Load(std::map<uint32_t, std::shared_ptr<Block>> &grid,
          const std::string &filename,
          int &width,
          int &height)
{
  std::ifstream file(filename);
  uint8_t read_width, read_height;
  file >> read_width >> read_height;
  for (uint32_t i = 0; i < read_width * read_height; i += 2) {
    uint8_t data_pair;
    file >> data_pair;
    grid[i] = std::make_shared<Block>(i % read_width, i / read_width, (data_pair >> 4));
    grid[i + 1] = std::make_shared<Block>((i + 1) % read_width, (i + 1) / read_width, (data_pair & 0b00001111));
  }
  width = static_cast<int>(read_width);
  height = static_cast<int>(read_height);
}

/**
 * Main function
 * Can take one argument (file name) if loading
 * To create new file, must specify name, width, and height
 * @return success
 */
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Needs file name as program argument." << std::endl;
    return 1;
  }

  int width, height;
  std::string filename(argv[1]);
  std::ifstream in_file(filename);
  in_file >> std::noskipws;
  bool loaded = false;
  std::map<uint32_t, std::shared_ptr<Block>> grid;

  // does file exist?
  if (in_file.is_open()) {
    in_file.close();
    Load(grid, filename, width, height);
    loaded = true;
  }
  else {
    if (argc < 4) {
      std::cerr << "File does not exist; add width and height as program arguments." << std::endl;
      return 1;
    }

    width = atoi(argv[2]);
    height = atoi(argv[3]);
    if (width > 255 || height > 255) {
      std::cerr << "Dimensions are too large, at most 255 in either direction." << std::endl;
      return 1;
    }

    std::cout << "Creating " << filename << std::endl;
    std::ofstream out_file(filename);

    if (!out_file.is_open()) {
      std::cerr << "Error creating file: " << filename << std::endl;
      return 1;
    }

    out_file.close();
  }

  sf::Vector2<unsigned int> window_size = {static_cast<unsigned int>(width) * BLOCK_SIZE_TOTAL,
                                           static_cast<unsigned int>(height) * BLOCK_SIZE_TOTAL};
  sf::RenderWindow window(sf::VideoMode(window_size), "Level Creator: " + filename);
  window.setFramerateLimit(FRAME_RATE);

  Cursor cursor;

  // init grid of blocks
  if (!loaded) {
    for (int i = 0; i < height; i++) {
      for (int j = 0; j < width; j++) {
        std::shared_ptr<Block> block = std::make_shared<Block>(j, i, 7);
        grid[i * GRID_WIDTH + j] = block;
      }
    }
  }

  bool brush = false;

  // loop
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // handle keyboard
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::Num1) {
          cursor.SetID(1);
        } else if (event.key.code == sf::Keyboard::Key::Num2) {
          cursor.SetID(2);
        } else if (event.key.code == sf::Keyboard::Key::Num3) {
          cursor.SetID(3);
        } else if (event.key.code == sf::Keyboard::Key::Num4) {
          cursor.SetID(4);
        } else if (event.key.code == sf::Keyboard::Key::Num5) {
          cursor.SetID(5);
        } else if (event.key.code == sf::Keyboard::Key::Num6) {
          cursor.SetID(6);
        } else if (event.key.code == sf::Keyboard::Key::Num7) {
          cursor.SetID(7);
        } else if (event.key.code == sf::Keyboard::Key::Num8) {
          cursor.SetID(8);
        } else if (event.key.code == sf::Keyboard::Key::Num0) {
          cursor.SetID(0);
        } else if (event.key.code == sf::Keyboard::Key::S) {
          Save(grid, filename, width, height);
        }
      }

      // place block
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          brush = true;
        }
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Button::Left) {
          brush = false;
        }
      }
    }

    // deal with cursor
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    cursor.SetPosition(window.mapPixelToCoords(mouse_pos));
    if (brush) {
      sf::Vector2f cursor_pos = cursor.GetPosition();
      sf::Vector2i grid_pos = {static_cast<int>(cursor_pos.x / BLOCK_SIZE_TOTAL),
                               static_cast<int>(cursor_pos.y / BLOCK_SIZE_TOTAL)};
      if (grid_pos.x < width && grid_pos.x >= 0) {
        grid[grid_pos.y * width + grid_pos.x] = std::make_shared<Block>(grid_pos.x, grid_pos.y, cursor.GetID());
      }
    }

    // Draw step
    window.clear(BACKGROUND_COLOR);
    for (const auto &block : grid) {
      block.second->Draw(window);
    }
    cursor.Draw(window);

    window.display();
  }

  return 0;
}