#include <iostream>
#include <vector>
#include <stack>
using namespace std;

// -------------------- Player --------------------

class Player {
private:
    int id;
    char symbol;

public:
    Player(int id, char symbol) {
        this->id = id;
        this->symbol = symbol;
    }

    int getId() {
        return id;
    }

    char getSymbol() {
        return symbol;
    }
};

// -------------------- Move --------------------

class Move {
private:
    int row;
    int col;
    int playerId;
    char symbol;

public:
    Move(int row, int col, int playerId, char symbol) {
        this->row = row;
        this->col = col;
        this->playerId = playerId;
        this->symbol = symbol;
    }

    int getRow() {
        return row;
    }

    int getCol() {
        return col;
    }

    int getPlayerId() {
        return playerId;
    }

    char getSymbol() {
        return symbol;
    }
};

// -------------------- Game Result --------------------

enum GameResult {
    FIRST,
    SECOND,
    DRAW,
    UNDECIDED
};

// -------------------- Board --------------------

class Board {
private:
    int n;
    vector<vector<char> > board;

public:
    Board(int n) {
        this->n = n;
        board.assign(n, vector<char>(n, '-'));
    }

    bool makeMove(int row, int col, char symbol) {

        if (row < 0 || row >= n ||
            col < 0 || col >= n) {
            return false;
        }

        if (board[row][col] != '-') {
            return false;
        }

        board[row][col] = symbol;
        return true;
    }

    void undoMove(int row, int col) {
        board[row][col] = '-';
    }

    void printBoard() {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << board[i][j] << " ";
            }
            cout << endl;
        }
    }

    bool isFull() {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (board[i][j] == '-')
                    return false;
            }
        }

        return true;
    }

    bool hasWon(int row, int col, char symbol) {

        // Check row
        bool rowWin = true;

        for (int j = 0; j < n; j++) {
            if (board[row][j] != symbol) {
                rowWin = false;
                break;
            }
        }

        if (rowWin)
            return true;

        // Check column
        bool colWin = true;

        for (int i = 0; i < n; i++) {
            if (board[i][col] != symbol) {
                colWin = false;
                break;
            }
        }

        if (colWin)
            return true;

        // Main diagonal
        if (row == col) {
            bool diagonalWin = true;

            for (int i = 0; i < n; i++) {
                if (board[i][i] != symbol) {
                    diagonalWin = false;
                    break;
                }
            }

            if (diagonalWin)
                return true;
        }

        // Anti-diagonal
        if (row + col == n - 1) {
            bool diagonalWin = true;

            for (int i = 0; i < n; i++) {
                if (board[i][n - 1 - i] != symbol) {
                    diagonalWin = false;
                    break;
                }
            }

            if (diagonalWin)
                return true;
        }

        return false;
    }
};

// -------------------- Game --------------------

class Game {
private:
    Board board;
    Player player1;
    Player player2;

    Player* currentPlayer;

    stack<Move> moves;

    GameResult result;

public:
    Game(int n)
        : board(n),
          player1(1, 'X'),
          player2(2, 'O') {

        currentPlayer = &player1;
        result = UNDECIDED;
    }

    void makeMove(int row, int col) {

        if (result != UNDECIDED) {
            cout << "Game is already over.\n";
            return;
        }

        bool success = board.makeMove(
            row,
            col,
            currentPlayer->getSymbol()
        );

        if (!success) {
            cout << "Invalid move!\n";
            return;
        }

        Move move(
            row,
            col,
            currentPlayer->getId(),
            currentPlayer->getSymbol()
        );

        moves.push(move);

        // Check winner
        if (board.hasWon(
                row,
                col,
                currentPlayer->getSymbol())) {

            if (currentPlayer->getId() == 1)
                result = FIRST;
            else
                result = SECOND;

            cout << "Player "
                 << currentPlayer->getId()
                 << " wins!\n";

            return;
        }

        // Check draw
        if (board.isFull()) {
            result = DRAW;

            cout << "Game is a draw!\n";
            return;
        }

        // Switch player
        if (currentPlayer == &player1)
            currentPlayer = &player2;
        else
            currentPlayer = &player1;
    }

    void undo() {

        if (moves.empty()) {
            cout << "Nothing to undo.\n";
            return;
        }

        Move lastMove = moves.top();
        moves.pop();

        board.undoMove(
            lastMove.getRow(),
            lastMove.getCol()
        );

        // Restore the player who made the move
        if (lastMove.getPlayerId() == 1)
            currentPlayer = &player1;
        else
            currentPlayer = &player2;

        result = UNDECIDED;

        cout << "Last move undone.\n";
    }

    void printBoard() {
        board.printBoard();
    }

    void getWinner() {

        if (result == FIRST)
            cout << "Winner: Player 1\n";

        else if (result == SECOND)
            cout << "Winner: Player 2\n";

        else if (result == DRAW)
            cout << "Result: Draw\n";

        else
            cout << "Game is still undecided.\n";
    }
};

// -------------------- Main --------------------

int main() {

    Game game(3);

    game.makeMove(0, 0); // X
    game.makeMove(1, 1); // O
    game.makeMove(0, 1); // X
    game.makeMove(1, 0); // O
    game.makeMove(0, 2); // X wins

    cout << "\nBoard:\n";
    game.printBoard();

    cout << "\n";
    game.getWinner();

    return 0;
}