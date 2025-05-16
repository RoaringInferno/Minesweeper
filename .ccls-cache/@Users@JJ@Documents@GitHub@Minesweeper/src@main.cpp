#include <iostream>

#include <CLI/CLI.hpp>

#include "minesweeper.hpp"


int main(int argc, char** argv)
{
    CLI::App app("TUI-based Minesweeper game.");
    app.footer("Minesweeper");

    ms::opts_t options;

    CLI::Option *width_opt = app.add_option("--width,--board-width", options.board_width, "The width of the game board. Must be >=3.");
    CLI::Option *height_opt = app.add_option("--height,--board-height", options.board_height, "The height of the game board. Must be >=3.");
    CLI::Option *mines_opt = app.add_option("--mines,--mine-count", options.mine_count, "The number of mines. (Must be less than width * height).");
    int difficulty = 0;
    CLI::Option *diff_opt = app.add_flag("--easy{1},--med{2},--medium{2},--hard{3},--difficulty,--diff,-d", difficulty, "Difficulty measurement. Conflicts with dimension and mine count options.");
    diff_opt->excludes(width_opt)->excludes(height_opt)->excludes(mines_opt);

    CLI11_PARSE(app, argc, argv);

    switch(difficulty)
    {
        case 1: // Easy
            options.board_width = 8;
            options.board_height = 8;
            options.mine_count = 10;
        case 2: // Medium
            options.board_width = 16;
            options.board_height = 16;
            options.mine_count = 40;
        case 3: // Hard (Default)
        case 0:
        default:
            break;
    }
    
    int retcode = ms::game(options);
    
    return retcode;
}
