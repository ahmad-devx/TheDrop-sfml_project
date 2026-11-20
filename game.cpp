#include "game.hpp"
#include <cstdlib>

void initGame(GameData& game) {
    // 12x16 city grid: 0 = Road, 1 = Building, 2 = Hazard
    int baseMap[ROWS][COLS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1},
        {1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 2, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 2, 0, 1, 1, 0, 0, 1, 0, 0, 2, 0, 0, 0, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    // Copy baseMap into the active 2D array
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            game.cityMap[r][c] = baseMap[r][c];
        }
    }

    game.courierRow = 1;
    game.courierCol = 1;
    game.hasPackage = false;
    game.lives = 3;
    game.score = 0;
    game.deliveriesDone = 0;
    game.timeRemaining = 60.0f;
    game.isGameOver = false;
    game.isWin = false;
    game.statusMessage = "Pick up the yellow package!";

    spawnNextDelivery(game);
}

void spawnNextDelivery(GameData& game) {
    int pkgRow = 0;
    int pkgCol = 0;

    // Pick a random empty road tile for the package
    do {
        pkgRow = std::rand() % ROWS;
        pkgCol = std::rand() % COLS;
    } while (game.cityMap[pkgRow][pkgCol] != TILE_ROAD ||
    (pkgRow == game.courierRow && pkgCol == game.courierCol) ||
    (pkgRow == 1 && pkgCol == 1));

    game.cityMap[pkgRow][pkgCol] = TILE_PACKAGE;

    int destRow = 0;
    int destCol = 0;

    // Pick a different random empty road tile for the destination
    do {
        destRow = std::rand() % ROWS;
        destCol = std::rand() % COLS;
    } while (game.cityMap[destRow][destCol] != TILE_ROAD ||
        (destRow == game.courierRow && destCol == game.courierCol) ||
        (destRow == 1 && destCol == 1));

    game.cityMap[destRow][destCol] = TILE_DESTINATION;
}

void handleInput(GameData& game, sf::Keyboard::Key key) {
    // Allow restarting anytime with R
    if (key == sf::Keyboard::Key::R) {
        initGame(game);
        return;
    }

    if (game.isGameOver) {
        return;
    }

    int nextRow = game.courierRow;
    int nextCol = game.courierCol;

    if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W) {
        nextRow--;
    }
    else if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S) {
        nextRow++;
    }
    else if (key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::A) {
        nextCol--;
    }
    else if (key == sf::Keyboard::Key::Right || key == sf::Keyboard::Key::D) {
        nextCol++;
    }
    else {
        return;
    }

    // Boundary check
    if (nextRow < 0 || nextRow >= ROWS || nextCol < 0 || nextCol >= COLS) {
        return;
    }

    int targetTile = game.cityMap[nextRow][nextCol];

    // Collision detection using 2D array values
    if (targetTile == TILE_BUILDING) {
        game.statusMessage = "Blocked by a building!";
        return;
    }

    if (targetTile == TILE_HAZARD) {
        game.lives--;
        game.courierRow = 1;
        game.courierCol = 1;
        game.statusMessage = "Hit a hazard! -1 Life";

        if (game.lives <= 0) {
            game.lives = 0;
            game.isGameOver = true;
            game.isWin = false;
        }
        return;
    }

    // Move courier onto the valid tile
    game.courierRow = nextRow;
    game.courierCol = nextCol;

    // Check package pickup
    if (targetTile == TILE_PACKAGE && !game.hasPackage) {
        game.hasPackage = true;
        game.cityMap[nextRow][nextCol] = TILE_ROAD;
        game.score += 50;
        game.statusMessage = "Package collected! Deliver to the green zone.";
    }
    // Check package delivery
    else if (targetTile == TILE_DESTINATION && game.hasPackage) {
        game.hasPackage = false;
        game.cityMap[nextRow][nextCol] = TILE_ROAD;
        game.score += 100;
        game.deliveriesDone++;

        if (game.deliveriesDone >= TARGET_DELIVERIES) {
            game.isGameOver = true;
            game.isWin = true;
        }
        else {
            game.statusMessage = "Delivered! Next package spawned.";
            spawnNextDelivery(game);
        }
    }
}

void updateTimer(GameData& game, float dt) {
    if (game.isGameOver) {
        return;
    }

    game.timeRemaining -= dt;
    if (game.timeRemaining <= 0.0f) {
        game.timeRemaining = 0.0f;
        game.isGameOver = true;
        game.isWin = false;
    }
}

void renderGame(sf::RenderWindow& window, const GameData& game, const sf::Font& font) {
    window.clear(sf::Color(25, 25, 30));

    // 1. Draw Top HUD Bar
    sf::RectangleShape hudBar(sf::Vector2f(static_cast<float>(COLS * TILE_SIZE), static_cast<float>(HUD_HEIGHT)));
    hudBar.setFillColor(sf::Color(20, 24, 33));
    window.draw(hudBar);

    sf::Text statsText(font);
    statsText.setCharacterSize(18);
    statsText.setFillColor(sf::Color::White);
    statsText.setPosition(sf::Vector2f(15.0f, 8.0f));
    statsText.setString(
        "Lives: " + std::to_string(game.lives) +
        "   |   Score: " + std::to_string(game.score) +
        "   |   Deliveries: " + std::to_string(game.deliveriesDone) + "/" + std::to_string(TARGET_DELIVERIES) +
        "   |   Time: " + std::to_string(static_cast<int>(game.timeRemaining)) + "s"
    );
    window.draw(statsText);

    sf::Text statusText(font);
    statusText.setCharacterSize(15);
    statusText.setFillColor(sf::Color(180, 220, 255));
    statusText.setPosition(sf::Vector2f(15.0f, 34.0f));
    statusText.setString(game.statusMessage + "  (Press R to Restart)");
    window.draw(statusText);

    // 2. Draw 2D City Map
    sf::RectangleShape tileShape(sf::Vector2f(static_cast<float>(TILE_SIZE - 2), static_cast<float>(TILE_SIZE - 2)));

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            float posX = static_cast<float>(c * TILE_SIZE + 1);
            float posY = static_cast<float>(HUD_HEIGHT + r * TILE_SIZE + 1);
            tileShape.setPosition(sf::Vector2f(posX, posY));

            int cell = game.cityMap[r][c];

            if (cell == TILE_BUILDING) {
                tileShape.setFillColor(sf::Color(95, 110, 130)); // Gray-blue building
            }
            else if (cell == TILE_HAZARD) {
                tileShape.setFillColor(sf::Color(200, 50, 50));  // Red obstacle
            }
            else if (cell == TILE_DESTINATION) {
                tileShape.setFillColor(sf::Color(45, 180, 90));  // Green drop-off zone
            }
            else {
                tileShape.setFillColor(sf::Color(50, 52, 60));   // Dark road
            }

            window.draw(tileShape);

            // Draw yellow package box on top of road tile
            if (cell == TILE_PACKAGE) {
                sf::RectangleShape pkgShape(sf::Vector2f(26.0f, 26.0f));
                pkgShape.setFillColor(sf::Color(245, 195, 45));
                pkgShape.setPosition(sf::Vector2f(posX + 11.0f, posY + 11.0f));
                window.draw(pkgShape);
            }
        }
    }

    // 3. Draw Courier (turns orange when carrying a package)
    sf::CircleShape courierShape(18.0f);
    courierShape.setFillColor(game.hasPackage ? sf::Color(255, 140, 0) : sf::Color(50, 180, 255));
    courierShape.setPosition(sf::Vector2f(
        static_cast<float>(game.courierCol * TILE_SIZE + 7),
        static_cast<float>(HUD_HEIGHT + game.courierRow * TILE_SIZE + 7)
    ));
    window.draw(courierShape);

    // 4. Draw Win / Lose Overlay
    if (game.isGameOver) {
        sf::RectangleShape overlay(sf::Vector2f(
            static_cast<float>(COLS * TILE_SIZE),
            static_cast<float>(ROWS * TILE_SIZE + HUD_HEIGHT)
        ));
        overlay.setFillColor(sf::Color(0, 0, 0, 195));
        window.draw(overlay);

        sf::Text endTitle(font);
        endTitle.setCharacterSize(36);
        endTitle.setFillColor(game.isWin ? sf::Color(80, 230, 120) : sf::Color(240, 70, 70));
        endTitle.setPosition(sf::Vector2f(230.0f, 240.0f));
        endTitle.setString(game.isWin ? "YOU WIN!" : "GAME OVER!");
        window.draw(endTitle);

        sf::Text endDetails(font);
        endDetails.setCharacterSize(22);
        endDetails.setFillColor(sf::Color::White);
        endDetails.setPosition(sf::Vector2f(230.0f, 300.0f));
        endDetails.setString(
            "Final Score: " + std::to_string(game.score) +
            "\nDeliveries: " + std::to_string(game.deliveriesDone) + "/" + std::to_string(TARGET_DELIVERIES) +
            "\n\nPress R to Play Again"
        );
        window.draw(endDetails);
    }

    window.display();
}