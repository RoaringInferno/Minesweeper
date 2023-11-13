#ifndef minesweeper_hpp
#define minesweeper_hpp

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
    };
}

#endif