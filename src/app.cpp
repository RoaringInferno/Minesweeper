#include <iostream>

#include "parameters.hpp"
#include "minesweeper.hpp"


/*
* Function that runs the game.
*/
void runMinesweeper() {
    minesweeper::Minesweeper<parameters::board_x, parameters::board_y, parameters::mines> game; // Creating game

    game.initialDisplay(); // Displays the initial board
}


int main() {
    runMinesweeper();
}