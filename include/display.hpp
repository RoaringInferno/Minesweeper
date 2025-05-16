#pragma once

#include "minesweeper.hpp"
#include "game_data.hpp"

#include <cstddef>
#include <string>

inline unsigned int center_begin_index(const unsigned int buff_size, const unsigned int block_size) { return (buff_size - block_size) / 2; };

struct coords_t { unsigned int row, col; };
struct display_settings_t
{
    unsigned int height, // Height of the window
                 width, // Width of the window
                 board_height, // Height of the board
                 board_width, // Width of the board
                 cell_pad_height, // The height of the padding between rows
                 cell_pad_width, // The width of the padding between columns
                 vert_buffer, // Space between the top of the window and the top of the screen
                 horiz_buffer, // Space between the sides of the window and the sides of the screen
                 header_width, // Total width of the header
                 dot_count, // The total number of characters stored in the screen (width * height)
                 top_margin, // Space between the top of the window and the header
                 header_margin, // Space between the header and the top of the board
                 footer_margin, // Space between the footer and the bottom of the board
                 right_margin, // Space between the right side of the board and the right side of the window
                 left_margin, // Space between the left side of the board and the left side of the window
                 horiz_line_trim, // Remove dashes from the front of the horizontal line
                 bottom_margin; // Space between the bottom of the board and the footer

    coords_t header_coords() const { return { .row = top_margin + 1, .col = center_begin_index(width, header_width) }; };
    coords_t board_coords() const { return { .row = top_margin + header_margin + 1 + 1, .col = left_margin + 1 }; };
}; // struct display_settings_t


unsigned int to_pos(const ms::opts_t &opts, const coords_t &coords);
coords_t from_pos(const ms::opts_t &opts, const unsigned int &pos);
char display_tile(const game_data_t::tile_t &tile);
void init_display_settings(const ms::opts_t &opts, display_settings_t &ds);
void initial_display(void* window_ptr, const ms::opts_t &opts, const display_settings_t &ds, game_data_t &initial_display);
void render_board(void *window_ptr, const ms::opts_t &opts, const display_settings_t &ds, const game_data_t &gd);
