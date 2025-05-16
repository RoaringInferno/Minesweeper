#include "display.hpp"

#include <iostream>

#include <ncurses.h>

void init_display_settings(const ms::opts_t &opts, display_settings_t &ds)
{
    const unsigned int &height = opts.board_height;
    const unsigned int &width = opts.board_width;
    ds.top_margin = 1;
    ds.header_margin = 1;
    ds.bottom_margin = 1;
    ds.footer_margin = 0;
    ds.right_margin = 6;
    ds.left_margin = 5;
    ds.header_width = 6;
    ds.cell_pad_height = 1;
    ds.cell_pad_width = 3;
    ds.board_height = (height + 1) * (ds.cell_pad_height + 1);
    ds.board_width = (width + 1) * (ds.cell_pad_width + 1) - 2;
    ds.height = ds.board_height + ds.top_margin + ds.bottom_margin + ds.header_margin + ds.footer_margin + 2 + 1;
    ds.width = ds.board_width + ds.right_margin + ds.left_margin + 2;
    ds.vert_buffer = center_begin_index(LINES, ds.height);
    ds.horiz_buffer = center_begin_index(COLS, ds.width);
    ds.dot_count = ds.height * ds.width;
    ds.horiz_line_trim = 1;
};

void initial_display(void *window_ptr, const ms::opts_t &opts, const display_settings_t &ds, game_data_t &gd)
{
    const unsigned int &tile_count = opts.board_width * opts.mine_count;
    gd.board.resize(tile_count);
    gd.status = "Initializing Game ..";
    gd.flags = opts.mine_count;
    gd.cursorpos = opts.board_width * (opts.board_height - 1) / 2; // Start in the middle
    for (auto &tile : gd.board) { tile.state = game_data_t::tile_t::STATE::INVISIBLE; tile.value = '0'; };
    return render_board(window_ptr, opts, ds, gd);
};

void render_board(void *window_ptr, const ms::opts_t &opts, const display_settings_t &ds, const game_data_t &gd)
{
    WINDOW *win = reinterpret_cast<WINDOW *>(window_ptr);
    // Header
    {
        const coords_t header_start = ds.header_coords();
        mvwprintw(win, header_start.row, header_start.col, "M: %03u", gd.flags);
    }
    // Board
    {
        const coords_t board_start = ds.board_coords();
        const std::string horiz_line(ds.board_width - ds.horiz_line_trim, '-');
        const std::string horiz_eraser(ds.cell_pad_width + 1, ' ');

        // Top Header
        wmove(win, board_start.row, board_start.col + ds.cell_pad_width - 1);
        wprintw(win, "  %2u", 1);
        for (unsigned int ci = 1; ci < opts.board_width; ci++)
        {
            wprintw(win, "| %2u", ci + 1);
        }
        // Body
        std::vector<bool> is_visible, is_visible_prev; // Whether or not the spaces are visible. First value is for the line number (always false)
        is_visible.resize(opts.board_width + 1);
        is_visible_prev.resize(opts.board_width + 1);
        unsigned int rloc = 0; // The board location of the row
        for (unsigned int ri = 0; ri < opts.board_height; ri++)
        {
            const coords_t row_horiz_pos = { board_start.row + (ri * 2) + 1, board_start.col };

            // Print Numbers and Spacers
            {
                wmove(win, row_horiz_pos.row + 1, board_start.col + 1);
                wprintw(win, "%2u", ri + 1); // Line Number
                unsigned int loc = rloc; // The board location of the cell
                for (unsigned int ci = 0; ci < opts.board_width - 1; ci++)
                {
                    if (is_visible[ci] && gd.is_visible(loc)) {
                        waddstr(win, "  "); // Remove spacer if the last one was blank
                    } else {
                        waddstr(win, "| ");
                    }
                    wprintw(win, "%c ", display_tile(gd.board[loc]));
                    is_visible[ci+1] = gd.is_visible(loc);

                    loc++;
                };
                if (is_visible[opts.board_width] && gd.is_visible(loc)) {
                    waddstr(win, "  ");
                } else {
                    waddstr(win, "| ");
                }
                wprintw(win, "%c", display_tile(gd.board[loc]));
            }

            // Print Line on top
            {
                wmove(win, row_horiz_pos.row, row_horiz_pos.col + ds.horiz_line_trim + 1);
                wprintw(win, horiz_line.c_str());
                // Remove lines if the top and bottom are both visible
                wmove(win, row_horiz_pos.row, row_horiz_pos.col);
                bool last = false; // if the last cell had its spacer removed
                unsigned int col = row_horiz_pos.col + ds.cell_pad_width; // The render x coord of the cell
                for (unsigned int ci = 0; ci < opts.board_width; ci++)
                {
                    wmove(win, row_horiz_pos.row, col);
                    if (is_visible[ci+1] && is_visible_prev[ci+1])
                    {
                        if (last)
                        {
                            waddch(win, ' ');
                        } else {
                            waddch(win, '+');
                        }
                        waddstr(win, horiz_eraser.c_str());
                        last = true;
                    } else {
                        waddch(win, '+');
                        last = false;
                    }
                    col += ds.cell_pad_width + 1;
                }
            }

            is_visible_prev = is_visible;
            rloc += opts.board_width;
        };
    }
    // Footer
    {
        const unsigned int footer_row = ds.height - ds.footer_margin - 2;
        const unsigned int footer_col = center_begin_index(ds.width, gd.status.length());
        wmove(win, footer_row, 1);
        wprintw(win, "%s", std::string(ds.width - 2, ' ').c_str());
        wmove(win, footer_row, footer_col);
        wprintw(win, "%s", gd.status.c_str());
    }
    // Move Cursor
    {
        const coords_t cursor = from_pos(opts, gd.cursorpos);
        const coords_t board_start = ds.board_coords();
        const unsigned int final_row = board_start.row + (cursor.row + 1) * (ds.cell_pad_height + 1);
        const unsigned int final_col = board_start.col + (cursor.col + 1) * (ds.cell_pad_width + 1) + 1;
        wmove(win, 
                final_row,
                final_col
             );
    }
    return;
};

char display_tile(const game_data_t::tile_t &tile)
{
    if (tile.state == game_data_t::tile_t::STATE::INVISIBLE) return ' '; // Not Visible
    if (tile.state == game_data_t::tile_t::STATE::FLAGGED) return 'F'; // Flagged
    if (tile.value == 'M') return 'X'; // Mined
    if (tile.value == '0') return ' '; // No Adjacent
    if (tile.value == '!') return 'R'; // At the end of the game, if the tile was incorrectly flagged
    return tile.value;
};

coords_t from_pos(const ms::opts_t &opts, const unsigned int &pos) { return { .row = pos / opts.board_width, .col = pos % opts.board_width }; };
unsigned int to_pos(const ms::opts_t &opts, const coords_t &coords) { return coords.row * opts.board_width + coords.col; };
