#pragma once

#include <cstddef>

namespace ms
{
    struct opts_t
    {
        enum class INTERFACE
        {
            NONE,
            CLI,
            TUI
        } interface;

        unsigned int board_width = 30;
        unsigned int board_height = 16;
        unsigned int mine_count = 99;
    };

    int game(const opts_t &opts = opts_t());
} // namespace ms
