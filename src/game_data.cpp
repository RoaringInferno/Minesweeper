#include "game_data.hpp"

#include <random>
#include <array>
#include <algorithm>
#include <fstream>

#include <cassert>

#include "log_file.hpp"


void init_game_data(const ms::opts_t &opts, game_data_t &gd, unsigned int first_move)
{
    const unsigned int &height = opts.board_height;
    const unsigned int &width = opts.board_width;
    const unsigned int &mine_count = opts.mine_count;
    const unsigned int tile_count = height * width;

    gd.board.resize(tile_count);
    // Scatter Mines (Select Mine Locations)
    // Set Forbidden Locations
    std::vector<unsigned int> forbidden;
    log_file forb_lf("init_game_data_forb");
    forb_lf.enable();
    forb_lf << "first move: " << first_move << "\n";
    forbidden.reserve(9);
    forbidden.push_back(first_move);
    {
        const unsigned int row_i = first_move / width;
        const unsigned int col_i = first_move % width;

        bool top{true}, bottom{true}, left{true}, right{true};
        if (col_i == 0) {
            left = false;
        } else if (col_i == width-1) {
            right = false;
        }
        if (row_i == 0) {
            top = false;
        } else if (row_i == height-1) {
            bottom = false;
        }
        if (top) forb_lf << "top true\n";
        if (bottom) forb_lf << "bottom true\n";
        if (left) forb_lf << "left true\n";
        if (right) forb_lf << "right true\n";
        if (top && left) {
            const unsigned int loc = first_move - width - 1;
            assert(loc < tile_count);
            forb_lf << loc << " top && left\n";
            forbidden.push_back(loc);
        };
        if (top) {
            const unsigned int loc = first_move - width;
            assert(loc < tile_count);
            forb_lf << loc << " top\n";
            forbidden.push_back(loc);
        };
        if (top && right) {
            const unsigned int loc = first_move - width + 1;
            assert(loc < tile_count);
            forb_lf << loc << " top && right\n";
            forbidden.push_back(loc);
        };
        if (left) {
            const unsigned int loc = first_move - 1;
            assert(loc < tile_count);
            forb_lf << loc << " left\n";
            forbidden.push_back(loc);
        };
        if (right) {
            const unsigned int loc = first_move + 1;
            assert(loc < tile_count);
            forb_lf << loc << " right\n";
            forbidden.push_back(loc);
        };
        if (bottom && left) {
            const unsigned int loc = first_move + width - 1;
            assert(loc < tile_count);
            forb_lf << loc << " bottom && left\n";
            forbidden.push_back(loc);
        };
        if (bottom) {
            const unsigned int loc = first_move + width;
            assert(loc < tile_count);
            forb_lf << loc << " bottom\n";
            forbidden.push_back(loc);
        };
        if (bottom && right) {
            const unsigned int loc = first_move + width + 1;
            assert(loc < tile_count);
            forb_lf << loc << " bottom && right\n";
            forbidden.push_back(loc);
        };
    }
    forb_lf.disable();

    std::vector<unsigned int> indices;
    indices.reserve(tile_count);
    for (unsigned int i = 0; i < tile_count; i++)
    {
        if (std::find(forbidden.begin(), forbidden.end(), i) == forbidden.end())
        {
            indices.push_back(i);
        }
    }

    // Initialize all tiles
    for (unsigned int row_i = 0; row_i < height; row_i++)
    {
        for (unsigned int col_i = 0; col_i < width; col_i++)
        {
            game_data_t::tile_t &tile = gd.board[tile_index(opts, row_i, col_i)];
            tile.state = game_data_t::tile_t::STATE::INVISIBLE;
            tile.value = '0'; // Start the counter at "0"
        }
    }

    // Initialize all Mines
    log_file mines_lf("init_game_data_mines");
    mines_lf.enable_log_file();
    std::mt19937 rng(std::random_device{}());
    std::shuffle(indices.begin(), indices.end(), rng);
    const auto mines_begin = indices.begin();
    const auto mines_end = indices.begin() + mine_count;
    const auto mines_iterate = [&gd, &tile_count](const unsigned int &index)->void {
        assert(index < tile_count);
        if (gd.board[index].value != 'M') gd.board[index].value++;
    };
    for (auto loc_it = mines_begin; loc_it != mines_end; loc_it++)
    {
        mines_lf << *loc_it << "\n";
        auto &tile = gd.board[*loc_it];
        tile.value = 'M';

        const unsigned int row_i = *loc_it / width;
        const unsigned int col_i = *loc_it % width;

        bool top{true}, bottom{true}, left{true}, right{true};
        if (col_i == 0) {
            left = false;
        } else if (col_i == width-1) {
            right = false;
        }
        if (row_i == 0) {
            top = false;
        } else if (row_i == height-1) {
            bottom = false;
        }
        if (top && left) mines_iterate(*loc_it - width - 1);
        if (top) mines_iterate(*loc_it - width);
        if (top && right) mines_iterate(*loc_it - width + 1);
        if (left) mines_iterate(*loc_it - 1);
        if (right) mines_iterate(*loc_it + 1);
        if (bottom && left) mines_iterate(*loc_it + width - 1);
        if (bottom) mines_iterate(*loc_it + width);
        if (bottom && right) mines_iterate(*loc_it + width + 1);
    }

    gd.flags = opts.mine_count;
};

unsigned int tile_index(const ms::opts_t &opts, const unsigned int &row_i, const unsigned int &col_i)
{
    return row_i * opts.board_width + col_i;
};
