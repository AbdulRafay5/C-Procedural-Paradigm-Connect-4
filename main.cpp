#include <SFML/Graphics.hpp>
#include <iostream>
#include <limits>
#include <vector>

constexpr int ROWS = 6;
constexpr int COLS = 7;
constexpr int CELL_SIZE = 100;
constexpr int WINDOW_WIDTH = COLS * CELL_SIZE;
constexpr int WINDOW_HEIGHT = (ROWS + 2) * CELL_SIZE;

enum Player { NONE = 0, HUMAN = 1, AI = 2 };

std::vector<std::vector<int>> board(ROWS, std::vector<int>(COLS, NONE));

bool isValidMove(int col) { return board[0][col] == NONE; }

int getNextRow(int col) {
  for (int r = ROWS - 1; r >= 0; r--)
    if (board[r][col] == NONE)
      return r;
  return -1;
}

void dropPiece(int row, int col, int player) { board[row][col] = player; }

bool checkWin(int player) {
  // Horizontal
  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c < COLS - 3; c++)
      if (board[r][c] == player && board[r][c + 1] == player &&
          board[r][c + 2] == player && board[r][c + 3] == player)
        return true;

  // Vertical
  for (int c = 0; c < COLS; c++)
    for (int r = 0; r < ROWS - 3; r++)
      if (board[r][c] == player && board[r + 1][c] == player &&
          board[r + 2][c] == player && board[r + 3][c] == player)
        return true;

  // Diagonal /
  for (int r = 3; r < ROWS; r++)
    for (int c = 0; c < COLS - 3; c++)
      if (board[r][c] == player && board[r - 1][c + 1] == player &&
          board[r - 2][c + 2] == player && board[r - 3][c + 3] == player)
        return true;

  // Diagonal \
    for (int r = 0; r < ROWS - 3; r++)
  for (int c = 0; c < COLS - 3; c++)
    if (board[r][c] == player && board[r + 1][c + 1] == player &&
        board[r + 2][c + 2] == player && board[r + 3][c + 3] == player)
      return true;

  return false;
}

bool isBoardFull() {
  for (int c = 0; c < COLS; c++)
    if (board[0][c] == NONE)
      return false;
  return true;
}

int evaluateBoard() {
  if (checkWin(AI))
    return 1000;
  if (checkWin(HUMAN))
    return -1000;
  return 0;
}

int minimax(int depth, bool maximizing) {
  int score = evaluateBoard();
  if (score == 1000 || score == -1000 || depth == 0 || isBoardFull())
    return score;

  if (maximizing) {
    int maxEval = std::numeric_limits<int>::min();
    for (int c = 0; c < COLS; c++) {
      if (isValidMove(c)) {
        int r = getNextRow(c);
        board[r][c] = AI;
        int eval = minimax(depth - 1, false);
        board[r][c] = NONE;
        maxEval = std::max(maxEval, eval);
      }
    }
    return maxEval;
  } else {
    int minEval = std::numeric_limits<int>::max();
    for (int c = 0; c < COLS; c++) {
      if (isValidMove(c)) {
        int r = getNextRow(c);
        board[r][c] = HUMAN;
        int eval = minimax(depth - 1, true);
        board[r][c] = NONE;
        minEval = std::min(minEval, eval);
      }
    }
    return minEval;
  }
}

int getBestMove() {
  int bestScore = std::numeric_limits<int>::min();
  int bestCol = 0;
  for (int c = 0; c < COLS; c++) {
    if (isValidMove(c)) {
      int r = getNextRow(c);
      board[r][c] = AI;
      int score = minimax(5, false);
      board[r][c] = NONE;
      if (score > bestScore) {
        bestScore = score;
        bestCol = c;
      }
    }
  }
  return bestCol;
}

void resetBoard() {
  for (int r = 0; r < ROWS; r++)
    for (int c = 0; c < COLS; c++)
      board[r][c] = NONE;
}

int main() {
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
                          "Connect 4 - Player vs AI");
  window.setFramerateLimit(60);

  sf::Font font;
  if (!font.loadFromFile(
          "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
    std::cerr << "Font not found!" << std::endl;
    return -1;
  }

  bool playerTurn = true;
  bool gameOver = false;
  std::string message = "";

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (!gameOver && event.type == sf::Event::MouseButtonPressed &&
          event.mouseButton.button == sf::Mouse::Left && playerTurn) {

        int col = event.mouseButton.x / CELL_SIZE;
        if (isValidMove(col)) {
          int row = getNextRow(col);
          dropPiece(row, col, HUMAN);

          if (checkWin(HUMAN)) {
            gameOver = true;
            message = "You Win! Click R to Restart, Q to Quit";
          } else if (isBoardFull()) {
            gameOver = true;
            message = "Draw! Click R to Restart, Q to Quit";
          } else
            playerTurn = false;
        }
      }

      if (gameOver && event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
          resetBoard();
          gameOver = false;
          playerTurn = true;
          message = "";
        } else if (event.key.code == sf::Keyboard::Q) {
          window.close();
        }
      }
    }

    if (!playerTurn && !gameOver) {
      int col = getBestMove();
      int row = getNextRow(col);
      dropPiece(row, col, AI);

      if (checkWin(AI)) {
        gameOver = true;
        message = "AI Wins! Click R to Restart, Q to Quit";
      } else if (isBoardFull()) {
        gameOver = true;
        message = "Draw! Click R to Restart, Q to Quit";
      } else
        playerTurn = true;
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

    // Draw message
    if (!message.empty()) {
      sf::Text text;
      text.setFont(font);
      text.setString(message);
      text.setCharacterSize(24);
      text.setFillColor(sf::Color::White);
      text.setPosition(10, 10);
      window.draw(text);
    }

    window.display();
  }

  return 0;
}