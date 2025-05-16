#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "minesweeper.hpp"


struct game_data_t
{
    struct tile_t
    {
        // M = Mine
        char value;
        enum class STATE : unsigned char {
           INVISIBLE,
           FLAGGED,
           VISIBLE
        } state;
    };

    std::vector<tile_t> board;

    unsigned int flags;

    std::string status;
    unsigned int cursorpos;

    tile_t &current_tile() { return board[cursorpos]; };
    const tile_t &current_tile() const { return board[cursorpos]; };

    bool is_visible(const unsigned int loc) const { return board[loc].state == tile_t::STATE::VISIBLE; };
}; // struct game_data_t

unsigned int tile_index(const ms::opts_t &opts, const unsigned int &row_i, const unsigned int &col_i);
void init_game_data(const ms::opts_t &opts, game_data_t &gd, const unsigned int first_move);
