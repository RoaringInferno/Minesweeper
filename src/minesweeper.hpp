#ifndef minesweeper_hpp
#define minesweeper_hpp

/*
* Stores resources for Minesweeper game.
*/
namespace minesweeper {
    /*
    * Creates a game instance of Minesweeper.
    */
    class Minesweeper {
        public:
        /*
        * Creates a Minesweeper game with the given parameters.
        */
        Minesweeper(unsigned char _board_x, unsigned char _board_y, unsigned char _mines);
    };
}

#endif