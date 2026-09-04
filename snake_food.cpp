#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>

using namespace std;


// =====================================================
// Position Class
// =====================================================

class Position {

public:

    int row;
    int col;

    Position(int r = 0, int c = 0) {
        row = r;
        col = c;
    }

    bool operator==(const Position& p) const {
        return row == p.row && col == p.col;
    }
};


// =====================================================
// Direction
// =====================================================

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};


// =====================================================
// Board Class
// =====================================================

class Board {

private:

    int width;
    int height;

public:

    Board(int w, int h) {
        width = w;
        height = h;
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }

    bool isInside(Position p) {

        return p.row >= 0 &&
               p.row < height &&
               p.col >= 0 &&
               p.col < width;
    }
};


// =====================================================
// Snake Class
// =====================================================

class Snake {

private:

    deque<Position> body;

public:

    Direction direction;


    Snake(Position start) {

        body.push_front(start);

        direction = RIGHT;
    }


    // Get head

    Position getHead() {

        return body.front();
    }


    // Get tail

    Position getTail() {

        return body.back();
    }


    // Add new head

    void addHead(Position p) {

        body.push_front(p);
    }


    // Remove tail

    void removeTail() {

        body.pop_back();
    }


    // Check whether snake contains position

    bool contains(Position p) {

        for (Position part : body) {

            if (part == p) {
                return true;
            }
        }

        return false;
    }


    // Display snake body

    void display() {

        cout << "Snake: ";

        for (Position p : body) {

            cout << "("
                 << p.row
                 << ","
                 << p.col
                 << ") ";
        }

        cout << endl;
    }
};


// =====================================================
// Food Class
// =====================================================

class Food {

private:

    Position position;

public:

    Food(Position p) {

        position = p;
    }


    Position getPosition() {

        return position;
    }


    void setPosition(Position p) {

        position = p;
    }
};


// =====================================================
// Game Class
// =====================================================

class Game {

private:

    Board board;
    Snake snake;
    Food food;

    int score;
    bool gameOver;


public:

    Game(int width, int height)
        : board(width, height),
          snake(Position(height / 2, width / 2)),
          food(Position(0, 0)) {

        score = 0;
        gameOver = false;

        // Generate first food

        generateFood();
    }


    // =================================================
    // Generate Food at Random Position
    // =================================================

    void generateFood() {

        int row;
        int col;

        while (true) {

            row = rand() % board.getHeight();
            col = rand() % board.getWidth();

            Position newFood(row, col);


            // Food should not be inside snake

            if (!snake.contains(newFood)) {

                food.setPosition(newFood);

                break;
            }
        }
    }


    // =================================================
    // Change Direction
    // =================================================

    void changeDirection(Direction newDirection) {

        // Prevent opposite movement

        if (snake.direction == UP &&
            newDirection == DOWN) {

            return;
        }

        if (snake.direction == DOWN &&
            newDirection == UP) {

            return;
        }

        if (snake.direction == LEFT &&
            newDirection == RIGHT) {

            return;
        }

        if (snake.direction == RIGHT &&
            newDirection == LEFT) {

            return;
        }


        snake.direction = newDirection;
    }


    // =================================================
    // Find Next Head Position
    // =================================================

    Position getNextPosition() {

        Position head = snake.getHead();


        if (snake.direction == UP) {

            head.row--;
        }

        else if (snake.direction == DOWN) {

            head.row++;
        }

        else if (snake.direction == LEFT) {

            head.col--;
        }

        else if (snake.direction == RIGHT) {

            head.col++;
        }


        return head;
    }


    // =================================================
    // Move Snake
    // =================================================

    void move() {

        if (gameOver) {
            return;
        }


        // Find next position

        Position newHead = getNextPosition();


        // ---------------------------------------------
        // 1. Wall Collision
        // ---------------------------------------------

        if (!board.isInside(newHead)) {

            gameOver = true;

            cout << "Game Over: Snake hit the wall!"
                 << endl;

            return;
        }


        // ---------------------------------------------
        // 2. Check Food
        // ---------------------------------------------

        bool eatingFood =
            (newHead == food.getPosition());


        // ---------------------------------------------
        // 3. Self Collision
        // ---------------------------------------------

        if (snake.contains(newHead)) {

            /*
                If the snake moves into its own body,
                game is over.
            */

            gameOver = true;

            cout << "Game Over: Snake hit itself!"
                 << endl;

            return;
        }


        // ---------------------------------------------
        // 4. Add New Head
        // ---------------------------------------------

        snake.addHead(newHead);


        // ---------------------------------------------
        // 5. Food Eaten
        // ---------------------------------------------

        if (eatingFood) {

            score++;

            cout << "Food eaten!"
                 << endl;

            /*
                Don't remove tail.

                Therefore snake grows.
            */

            generateFood();
        }


        // ---------------------------------------------
        // 6. Normal Movement
        // ---------------------------------------------

        else {

            /*
                Remove tail.

                Therefore snake length
                remains the same.
            */

            snake.removeTail();
        }
    }


    // =================================================
    // Display Board
    // =================================================

    void display() {

        cout << endl;


        for (int i = 0;
             i < board.getHeight();
             i++) {


            for (int j = 0;
                 j < board.getWidth();
                 j++) {


                Position current(i, j);


                // Snake

                if (snake.contains(current)) {

                    cout << "S ";
                }


                // Food

                else if (current == food.getPosition()) {

                    cout << "F ";
                }


                // Empty cell

                else {

                    cout << ". ";
                }
            }


            cout << endl;
        }


        cout << "Score: "
             << score
             << endl;
    }


    // =================================================
    // Check Game Over
    // =================================================

    bool isGameOver() {

        return gameOver;
    }


    // =================================================
    // Get Score
    // =================================================

    int getScore() {

        return score;
    }
};


// =====================================================
// Main
// =====================================================

int main() {

    // Random seed

    srand(time(0));


    // Create 10 x 10 game

    Game game(10, 10);


    // Initial board

    game.display();


    // -----------------------------------------------
    // Move Right
    // -----------------------------------------------

    game.move();

    game.display();


    // -----------------------------------------------
    // Move Down
    // -----------------------------------------------

    game.changeDirection(DOWN);

    game.move();

    game.display();


    // -----------------------------------------------
    // Move Left
    // -----------------------------------------------

    game.changeDirection(LEFT);

    game.move();

    game.display();


    // -----------------------------------------------
    // Move Up
    // -----------------------------------------------

    game.changeDirection(UP);

    game.move();

    game.display();


    return 0;
}