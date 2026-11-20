#include "game.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(
            static_cast<unsigned int>(COLS * TILE_SIZE),
            static_cast<unsigned int>(ROWS * TILE_SIZE + HUD_HEIGHT)
        )),
        "2D Courier Delivery Game"
    );
    window.setFramerateLimit(60);
   

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Error: Could not load arial.ttf. Place arial.ttf in the project folder.\n";
        return 1;
    }

    GameData game;
    initGame(game);

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                handleInput(game, keyPressed->code);
            }
        }

        updateTimer(game, dt);
        renderGame(window, game, font);
    }

    return 0;
}