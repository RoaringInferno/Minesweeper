#ifndef minesweeper_hpp
#define minesweeper_hpp

#include <string>

/*
* Stores resources for Minesweeper game.
*/
namespace minesweeper {
    /*
    * Stores move data
    */
    struct Move {
        unsigned char x; // X-coordinate of the move
        unsigned char y; // Y-coordinate of the move
        bool type; // Move type: 1 = step, 0 = flag.

        /*
        * Generates a empty move.
        */
        Move() {}

        /*
        * Generates a populated Move.
        */
        Move(unsigned char _x, unsigned char _y, bool _type) : x(_x), y(_y), type(_type) {}
    };


    /*
    * Parameters of board.
    */
    template<unsigned char board_x, unsigned char board_y, unsigned char mines>

    /*
    * Creates a game instance of Minesweeper.
    */
    class Minesweeper {
        public:
        /*
        * Creates a Minesweeper game with the given parameters.
        */
        Minesweeper();

        /*
        * Displays a board of Xs, for the beginning of the game.
        */
        void initialDisplay();

        /*
        * Gets the player's move.
        */
        Move getPlayerMove();
    };
}

// C++
// class Minesweeper
template<unsigned char board_x, unsigned char board_y, unsigned char mines>
minesweeper::Minesweeper<board_x, board_y, mines>::Minesweeper() {
    //
}

template<unsigned char board_x, unsigned char board_y, unsigned char mines>
void minesweeper::Minesweeper<board_x, board_y, mines>::initialDisplay() {
    std::string y_max_coord =  std::to_string(board_y);
    unsigned char max_y_coord_display_length = y_max_coord.length();


    for (unsigned char y_coord = 0; y_coord < board_y; ++y_coord)
    {
        std::string row_number = std::to_string(y_coord);
        std::cout << row_number;
        for (unsigned char i = 0; i < (max_y_coord_display_length - row_number.length()); ++i)
        {
            std::cout << " ";
        }
        for (unsigned char x_boord = 0; x_boord < board_x; ++x_boord)
        {
            std::cout << " | X";
        }
        std::cout << "\n";
    }
}

#endif