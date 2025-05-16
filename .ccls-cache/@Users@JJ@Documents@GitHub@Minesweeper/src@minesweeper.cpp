#include "minesweeper.hpp"

#include <ncurses.h>
#include <string>
#include <vector>

#include "log_file.hpp"
#include "game_data.hpp"
#include "display.hpp"


struct move_t
{
    unsigned int loc;
    enum TYPE
    {
        FLAG,
        UNFLAG,
        STEP
    } type;
};
move_t get_move(WINDOW *win, const ms::opts_t &opts, game_data_t &gd, const display_settings_t &ds);
unsigned int clear(const ms::opts_t &opts, game_data_t &gd, const unsigned int &loc);

int ms::game(const ms::opts_t &opts)
{
    initscr();

    display_settings_t ds;
    init_display_settings(opts, ds);
    game_data_t gd;

    WINDOW *win = newwin(ds.height, ds.width, ds.vert_buffer, ds.horiz_buffer);
    keypad(win, TRUE);
    curs_set(1);
    box(win, 0, 0);
    initial_display(win, opts, ds, gd);
    wrefresh(win);

    log_file mv_lf("moves");
    // First move
    {
        do {
            move_t move = get_move(win, opts, gd, ds);
            if (move.type == move_t::TYPE::STEP)
            {
                mv_lf << "Move 0: STEP " << move.loc << "\n";
                init_game_data(opts, gd, move.loc);
                clear(opts, gd, move.loc);

                // Debugging
#ifndef NDEBUG
                log_file mines_lf("first_move_mines");
                mines_lf.enable_log_file();
                unsigned int loc = 0;
                for (const auto &tile : gd.board)
                {
                    if (tile.value == 'M')
                    {
                        mines_lf << loc << "\n";
                    }
                    loc++;
                }
#endif
                break;
            }
            gd.status = "First move must be a step.";
            render_board(win, opts, ds, gd);
            wrefresh(win);
        } while(true);
        render_board(win, opts, ds, gd);
        wrefresh(win);
    }

    // Game Loop
    unsigned int correct_flags = 0;
    bool do_game = true;
    unsigned int move_num = 1;
    while(do_game)
    {
        move_t move = get_move(win, opts, gd, ds);
        mv_lf << "Move " << move_num << ": ";
        game_data_t::tile_t &tile = gd.board[move.loc];
        switch (move.type)
        {
            case move_t::TYPE::STEP:
                mv_lf << "STEP " << move.loc;
                if (tile.value == 'M')
                {
                    // Mine
                    mv_lf << " (mined)\n";
                    do_game = false;
                    gd.status = "Stepped on a mine, Game Over. (Space to exit)";
                    for (auto &tile: gd.board) {
                        if (tile.value == 'M' && tile.state != game_data_t::tile_t::STATE::FLAGGED) {
                            tile.state = game_data_t::tile_t::STATE::VISIBLE;
                        } else if (tile.value != 'M' && tile.state == game_data_t::tile_t::STATE::FLAGGED) {
                            tile.state = game_data_t::tile_t::STATE::VISIBLE;
                            tile.value = '!';
                        }
                    }
                    break;
                }
                mv_lf << "\n";
                if (tile.value == '0')
                {
                    // Empty Tile
                    unsigned int count = clear(opts, gd, move.loc);
                    gd.status = "Stepped; Cleared " + std::to_string(count) + " tiles.";
                    break;
                }
                tile.state = game_data_t::tile_t::STATE::VISIBLE;
                break;
            case move_t::TYPE::FLAG:
                mv_lf << "FLAG " << move.loc << " ";
                gd.flags--;
                tile.state = game_data_t::tile_t::STATE::FLAGGED;
                if (tile.value == 'M')
                {
                    correct_flags++;
                    mv_lf << "(correct)";
                } else {
                    mv_lf << "(incorrect)";
                }
                mv_lf << "\n";
                break;
            case move_t::TYPE::UNFLAG:
                mv_lf << "UNFLAG " << move.loc << " ";
                gd.flags++;
                tile.state = game_data_t::tile_t::STATE::INVISIBLE;
                if (tile.value == 'M')
                {
                    correct_flags--;
                    mv_lf << "(incorrect)";
                } else {
                    mv_lf << "(correct)";
                }
                mv_lf << "\n";
                break;
        }
        if (correct_flags == opts.mine_count)
        {
            gd.status = "All mines cleared, Game Over.";
            for (auto &tile : gd.board) tile.state = game_data_t::tile_t::STATE::VISIBLE;
            do_game = false;
        }
        render_board(win, opts, ds, gd);
        wrefresh(win);
    }

    std::vector<int> exit_keys = { static_cast<int>(' '), static_cast<int>('\n') };
    while (true) {
        const int ch = wgetch(win);
        switch (ch) {
            // Left
            case KEY_LEFT:
            case 'h':
            case 'a':
                if (gd.cursorpos % opts.board_width != 0) gd.cursorpos--;
                gd.status = "Moving Left ...";
                break;

                // Right
            case KEY_RIGHT:
            case 'l':
            case 'd':
                if (gd.cursorpos % opts.board_width != opts.board_width - 1) gd.cursorpos++;
                gd.status = "Moving Right ...";
                break;

                // Up
            case KEY_UP:
            case 'k':
            case 'w':
                if (gd.cursorpos >= opts.board_width) gd.cursorpos -= opts.board_width;
                gd.status = "Moving Up ...";
                break;

                // Down
            case KEY_DOWN:
            case 'j':
            case 's':
                if (gd.cursorpos / opts.board_width < opts.board_height) gd.cursorpos += opts.board_width;
                gd.status = "Moving Down ...";
                break;

                // Exit
            case ' ':
            case '\n':
            delwin(win);
            endwin();
            return 0;
        }
        render_board(win, opts, ds, gd);
    };
}; // int ms::game(const opts_t &)

move_t get_move(WINDOW *win, const ms::opts_t &opts, game_data_t &gd, const display_settings_t &ds)
{
    move_t retval;
    while (true)
    {
        const int ch = wgetch(win);

        coords_t coords = from_pos(opts, gd.cursorpos);
        switch (ch)
        {
            // Left
            case KEY_LEFT:
            case 'h':
            case 'a':
                if (gd.cursorpos % opts.board_width != 0) gd.cursorpos--;
                gd.status = "Moving Left ...";
                break;

                // Right
            case KEY_RIGHT:
            case 'l':
            case 'd':
                if (gd.cursorpos % opts.board_width != opts.board_width - 1) gd.cursorpos++;
                gd.status = "Moving Right ...";
                break;

                // Up
            case KEY_UP:
            case 'k':
            case 'w':
                if (gd.cursorpos >= opts.board_width) gd.cursorpos -= opts.board_width;
                gd.status = "Moving Up ...";
                break;

                // Down
            case KEY_DOWN:
            case 'j':
            case 's':
                if (gd.cursorpos / opts.board_width < opts.board_height) gd.cursorpos += opts.board_width;
                gd.status = "Moving Down ...";
                break;

                // Step
            case '\n':
            case ' ':
                if (gd.current_tile().state == game_data_t::tile_t::STATE::VISIBLE)
                {
                    gd.status = "Cannot step on visible tiles.";
                    break;
                }
                if (gd.current_tile().state == game_data_t::tile_t::STATE::FLAGGED)
                {
                    gd.status = "Cannot step on flagged tiles.";
                    break;
                }
                retval.loc = gd.cursorpos;
                gd.status = "Stepping at " + std::to_string(gd.cursorpos) + "(" + std::to_string(coords.col + 1) + ", " + std::to_string(coords.row + 1) + ")...";
                retval.type = move_t::TYPE::STEP;
                return retval;

                // Flag
            case '?':
            case '!':
            case 'f':
                if (gd.current_tile().state == game_data_t::tile_t::STATE::VISIBLE)
                {
                    gd.status = "Cannot mark visible tiles.";
                    break;
                }
                retval.loc = gd.cursorpos;
                if (gd.current_tile().state == game_data_t::tile_t::STATE::FLAGGED)
                {
                    gd.status = "Unmarking at " + std::to_string(gd.cursorpos) + "(" + std::to_string(coords.col + 1) + ", " + std::to_string(coords.row + 1) + ")...";
                    retval.type = move_t::TYPE::UNFLAG;
                    return retval;
                }
                gd.status = "Marking at " + std::to_string(gd.cursorpos) + "(" + std::to_string(coords.col + 1) + ", " + std::to_string(coords.row + 1) + ")...";
                retval.type = move_t::TYPE::FLAG;
                return retval;

                // Other
            default:
                gd.status = "(" + (ch < 256 ? std::string(1, (char)ch) : std::to_string(ch)) + ") key does not have a binding.";
                break;
        };

        render_board(win, opts, ds, gd);
        wrefresh(win);
    }
};

unsigned int clear(const ms::opts_t &opts, game_data_t &gd, const unsigned int &loc)
{
    gd.board[loc].state = game_data_t::tile_t::STATE::VISIBLE;
    unsigned int retval = 0;
    const unsigned int row_i = loc / opts.board_width;
    const unsigned int col_i = loc % opts.board_width;

    bool top{true}, bottom{true}, left{true}, right{true};
    if (col_i == 0) {
        left = false;
    } else if (col_i == opts.board_width-1) {
        right = false;
    }
    if (row_i == 0) {
        top = false;
    } else if (row_i == opts.board_height-1) {
        bottom = false;
    }
    if (top && left)
    {
        if (gd.board[loc - opts.board_width - 1].value == '0')
        {
            if (gd.board[loc - opts.board_width - 1].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc - opts.board_width - 1);
            }
        } else {
            gd.board[loc - opts.board_width - 1].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    if (top)
    {
        if (gd.board[loc - opts.board_width].value == '0')
        {
            if (gd.board[loc - opts.board_width].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc - opts.board_width);
            }
        } else {
            gd.board[loc - opts.board_width].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    if (top && right)
    {
        if (gd.board[loc - opts.board_width + 1].value == '0')
        {
            if (gd.board[loc - opts.board_width + 1].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc - opts.board_width + 1);
            }
        } else {
            gd.board[loc - opts.board_width + 1].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    if (left)
    {
        if (gd.board[loc - 1].value == '0')
        {
            if (gd.board[loc - 1].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc - 1);
            }
        } else {
            gd.board[loc - 1].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    if (right)
    {
        if (gd.board[loc + 1].value == '0')
        {
            if (gd.board[loc + 1].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc + 1);
            }
        } else {
            gd.board[loc + 1].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    if (bottom && left)
    {
        if (gd.board[loc + opts.board_width - 1].value == '0')
        {
            if (gd.board[loc + opts.board_width - 1].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc + opts.board_width - 1);
            }
        } else {
            gd.board[loc + opts.board_width - 1].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    if (bottom)
    {
        if (gd.board[loc + opts.board_width].value == '0')
        {
            if (gd.board[loc + opts.board_width].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc + opts.board_width);
            }
        } else {
            gd.board[loc + opts.board_width].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    if (bottom && right)
    {
        if (gd.board[loc + opts.board_width + 1].value == '0')
        {
            if (gd.board[loc + opts.board_width + 1].state == game_data_t::tile_t::STATE::INVISIBLE)
            {
                retval += clear(opts, gd, loc + opts.board_width + 1);
            }
        } else {
            gd.board[loc + opts.board_width + 1].state = game_data_t::tile_t::STATE::VISIBLE;
        }
    }
    return retval;
};
