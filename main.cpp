#include <SFML/Graphics.hpp>
#include <vector>

constexpr int ROWS = 6;
constexpr int COLS = 7;
constexpr int CELL_SIZE = 100;
constexpr int WINDOW_WIDTH = COLS * CELL_SIZE;
constexpr int WINDOW_HEIGHT = (ROWS + 2) * CELL_SIZE;

enum Player { NONE = 0, HUMAN = 1, AI = 2 };
std::vector<std::vector<int>> board(ROWS, std::vector<int>(COLS, NONE));

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Connect 4");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Blue);

        // Draw board
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                sf::CircleShape piece(40);
                piece.setPosition(c * CELL_SIZE + 10, (r + 1) * CELL_SIZE + 10);

                if (board[r][c] == HUMAN)
                    piece.setFillColor(sf::Color::Red);
                else if (board[r][c] == AI)
                    piece.setFillColor(sf::Color::Yellow);
                else
                    piece.setFillColor(sf::Color::Black);

                window.draw(piece);
            }
        }

        window.display();
    }

    return 0;
}