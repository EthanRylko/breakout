#include <iostream>
#include <fstream>
#include <map>

#include "SFML/graphics.hpp"
#include "../block.h"
#include "../constants.h"
#include "cursor.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Needs file name as program argument." << std::endl;
    return 1;
  }

  int width, height;
  std::string filename(argv[1]);
  std::ifstream in_file(argv[1]);
  in_file >> std::noskipws;

  // does file exist?
  if (in_file.is_open()) {
    uint8_t read_width, read_height;
    in_file >> read_width >> read_height;
    printf("read width: %u read height: %u", read_width, read_height);
    width = read_width;
    height = read_height;
    in_file.close();
  }
  else {
    // File does not exist, create the file
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
    std::ofstream outputFile(filename);

    if (!outputFile.is_open()) {
      std::cerr << "Error creating file: " << filename << std::endl;
      return 1;
    }

    outputFile << static_cast<uint8_t>(width) << static_cast<uint8_t>(height);
    outputFile.close();
  }

  sf::Vector2<unsigned int> window_size = {static_cast<unsigned int>(width) * BLOCK_SIZE_TOTAL,
                                           static_cast<unsigned int>(height) * BLOCK_SIZE_TOTAL};
  sf::RenderWindow window(sf::VideoMode(window_size), "Level Creator: " + filename);
  window.setFramerateLimit(FRAME_RATE);

  uint8_t data[height][width];
  Cursor cursor;

  // init grid of blocks
  std::map<uint32_t, std::shared_ptr<Block>> grid;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      std::shared_ptr<Block> block = std::make_shared<Block>(j, i, 7);
      grid[i * GRID_WIDTH + j] = block;
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

      // change block id
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

    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    cursor.SetPosition(mouse_pos);

    if (brush) {
      sf::Vector2i grid_pos = {mouse_pos.x / BLOCK_SIZE_TOTAL, mouse_pos.y / BLOCK_SIZE_TOTAL};
      grid[grid_pos.y * width + grid_pos.x] = std::make_shared<Block>(grid_pos.x, grid_pos.y, cursor.GetID());}
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