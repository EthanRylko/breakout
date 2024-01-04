#pragma once

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int WINDOW_HALF_WIDTH = WINDOW_WIDTH / 2;
const int FRAME_RATE = 60;

const int PLAYER_WIDTH = 100;
const int PLAYER_HEIGHT = 10;
const int PLAYER_HALF_WIDTH = PLAYER_WIDTH / 2;
const int PLAYER_Y = WINDOW_HEIGHT - 50;

const int BALL_RADIUS = 5;
const int BALL_SPEED = 5;

const int BLOCK_SIZE_TOTAL = 10;
const int BLOCK_HALF_SIZE_TOTAL = BLOCK_SIZE_TOTAL / 2;
const int BLOCK_SPACING = 1;
const int BLOCK_SIZE = BLOCK_SIZE_TOTAL - 2 * BLOCK_SPACING;
const int BLOCK_HALF_SIZE = BLOCK_SIZE / 2;

const int GRID_WIDTH = WINDOW_WIDTH / BLOCK_SIZE_TOTAL;
const int GRID_HEIGHT = 32;

const sf::Color BACKGROUND_COLOR = sf::Color::Black;
const sf::Color PLAYER_COLOR = sf::Color::White;
const sf::Color BALL_COLOR = sf::Color::White;

const uint8_t COLOR_ARRAY[15] = {255, 255, 191, 127, 63, 0, 0, 0, 0, 0, 0, 63, 127, 191, 255};
enum class collision_type {NONE, VERTICAL, HORIZONTAL, CORNER};