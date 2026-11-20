#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <string>

// Grid and window constants
const int ROWS = 12;
const int COLS = 16;
const int TILE_SIZE = 50;
const int HUD_HEIGHT = 60;
const int TARGET_DELIVERIES = 3;

// Tile codes used in the 2D array
const int TILE_ROAD = 0;
const int TILE_BUILDING = 1;
const int TILE_HAZARD = 2;
const int TILE_PACKAGE = 3;
const int TILE_DESTINATION = 4;

struct GameData {
    int cityMap[ROWS][COLS];
    int courierRow;
    int courierCol;
    bool hasPackage;
    int lives;
    int score;
    int deliveriesDone;
    float timeRemaining;
    bool isGameOver;
    bool isWin;
    std::string statusMessage;
};

void initGame(GameData& game);
void spawnNextDelivery(GameData& game);
void handleInput(GameData& game, sf::Keyboard::Key key);
void updateTimer(GameData& game, float dt);
void renderGame(sf::RenderWindow& window, const GameData& game, const sf::Font& font);

#endif