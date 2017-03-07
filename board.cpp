#include "board.hpp"

/*
 * Make a standard 8x8 othello board and initialize it to the standard setup.
 */
Board::Board() {
    taken.set(3 + 8 * 3);
    taken.set(3 + 8 * 4);
    taken.set(4 + 8 * 3);
    taken.set(4 + 8 * 4);
    black.set(4 + 8 * 3);
    black.set(3 + 8 * 4);
}

/*
 * Destructor for the board.
 */
Board::~Board() {
}

/*
 * Returns a copy of this board.
 */
Board *Board::copy() {
    Board *newBoard = new Board();
    newBoard->black = black;
    newBoard->taken = taken;
    return newBoard;
}

bool Board::occupied(int x, int y) {
    return taken[x + 8*y];
}

bool Board::get(Side side, int x, int y) {
    return occupied(x, y) && (black[x + 8*y] == (side == BLACK));
}

void Board::set(Side side, int x, int y) {
    taken.set(x + 8*y);
    black.set(x + 8*y, side == BLACK);
}

bool Board::onBoard(int x, int y) {
    return(0 <= x && x < 8 && 0 <= y && y < 8);
}


/*
 * Returns true if the game is finished; false otherwise. The game is finished
 * if neither side has a legal move.
 */
bool Board::isDone() {
    return !(hasMoves(BLACK) || hasMoves(WHITE));
}

/*
 * Returns true if there are legal moves for the given side.
 */
bool Board::hasMoves(Side side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Move move(i, j);
            if (checkMove(&move, side)) return true;
        }
    }
    return false;
}

/*
 * CW
 * Fills a vector with the current available moves
 */
void Board::allMoves(std::vector<Move> *moves, Side side)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Move move(i,j);
            if (checkMove(&move, side))
            {
                moves->push_back(move);
            }
        }
    }
}

/*
 * CW
 * Returns a random available move, or returns nullptr
 */
Move *Board::randomMove(Side side)
{
    // Find all possible moves
    std::vector<Move> moves;
    allMoves(&moves, side);
    // If no moves, return nullptr
    if (moves.size() == 0)
    {
        return nullptr;
    }
    // Pick random move
    int index = rand() % moves.size();
    return new Move(moves[index]);
}

/*
 * CW
 * Returns the best move according to a heuristic.
 */
Move *Board::heuristicMove(Side side)
{
    // Find all possible moves
    std::vector<Move> moves;
    allMoves(&moves, side);
    // If no moves, return nullptr
    if (moves.size() == 0)
    {
        return nullptr;
    }
    // Find best move
    int best = 0;
    int tileAdvantage = -64 - 20;    // maximum tile difference on 8x8 board
    for (int i = 0; i < (int) moves.size(); i++)
    {
        // Simulate move
        Board *hypothetical = copy();
        hypothetical->doMove(&moves[i],side);
        // Evaluate strength of move
        int strength = hypothetical->heuristic(&moves[i], side);
        if (strength > tileAdvantage)
        {
            best = i;
            tileAdvantage = strength;
        }
    }
    return new Move(moves[best]);
}

/*
 * CW
 * Returns the best minimax move for n-ply depth.
 */
Move *Board::minimax(Side side, int depth)
{
    // Find all possible moves
    std::vector<Move> moves;
    allMoves(&moves, side);
    // If no moves, return nullptr
    if (moves.size() == 0)
    {
        return nullptr;
    }
    // Find the minimum score along each branch
    std::vector<int> minScores(moves.size());
    for (int i = 0; i < (int) moves.size(); i++)
    {
        // Create new board and make move
        Board *branch = copy(); //// Do I need to delete this memory?
        // Opposite side's turn
        if (side == BLACK)
        {
            minScores[i] = branch->findMinScores(WHITE, side, depth - 1);
        }
        else
        {
            minScores[i] = branch->findMinScores(BLACK, side, depth - 1);
        }
        // Apply heuristic to home side
        minScores[i] += heuristic(&moves[i], side);
    }
    // Find the highest minimum score
    int best = 0;
    int bestScore = -64 - 20;   // absolute minimum score
    for (int i = 0; i < (int) moves.size(); i++)
    {
        if (minScores[i] > bestScore)
        {
            best = i;
            bestScore = minScores[i];
        }
    }
    return new Move(moves[best]);
}

/*
 * CW
 * Find minimum score of this branch.
 */
int Board::findMinScores(Side side, Side home, int depth)
{
    if (depth == 0)
    {
        return 0;
    }
    
    // Find all possible moves
    std::vector<Move> moves;
    allMoves(&moves, side);
    // If no moves...
    if (moves.size() == 0)
    {
        // If home side, this is bad
        if (side == home)
        {
            return -20;
        }
        // If guest side, this is good
        return 20;
    }
    // Find the minimum score along each branch
    std::vector<int> minScores(moves.size());
    for (int i = 0; i < (int) moves.size(); i++)
    {
        // Create new board and make move
        Board *branch = copy();
        // Opposite side's turn
        if (side == BLACK)
        {
            minScores[i] = branch->findMinScores(WHITE, home, depth - 1);
        }
        else
        {
            minScores[i] = branch->findMinScores(BLACK, home, depth - 1);
        }
        // Apply heuristic to home side.
        minScores[i] = heuristic(&moves[i], home);
    }
    
    
    // Find the minimum score
    int min_score = 64 + 20;   // absolute maximum score
    for (int i = 0; i < (int) moves.size(); i++)
    {
        if (minScores[i] < min_score)
        {
            min_score = minScores[i];
        }
    }
    return min_score;
}

/*
 * Returns true if a move is legal for the given side; false otherwise.
 */
bool Board::checkMove(Move *m, Side side) {
    // Passing is only legal if you have no moves.
    if (m == nullptr) return !hasMoves(side);

    int X = m->getX();
    int Y = m->getY();

    // Make sure the square hasn't already been taken.
    if (occupied(X, Y)) return false;

    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            // Is there a capture in that direction?
            int x = X + dx;
            int y = Y + dy;
            if (onBoard(x, y) && get(other, x, y)) {
                do {
                    x += dx;
                    y += dy;
                } while (onBoard(x, y) && get(other, x, y));

                if (onBoard(x, y) && get(side, x, y)) return true;
            }
        }
    }
    return false;
}

/*
 * Modifies the board to reflect the specified move.
 */
void Board::doMove(Move *m, Side side) {
    // A nullptr move means pass.
    if (m == nullptr) return;

    // Ignore if move is invalid.
    if (!checkMove(m, side)) return;

    int X = m->getX();
    int Y = m->getY();
    Side other = (side == BLACK) ? WHITE : BLACK;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dy == 0 && dx == 0) continue;

            int x = X;
            int y = Y;
            do {
                x += dx;
                y += dy;
            } while (onBoard(x, y) && get(other, x, y));

            if (onBoard(x, y) && get(side, x, y)) {
                x = X;
                y = Y;
                x += dx;
                y += dy;
                while (onBoard(x, y) && get(other, x, y)) {
                    set(side, x, y);
                    x += dx;
                    y += dy;
                }
            }
        }
    }
    set(side, X, Y);
}

/*
 * CW
 * Heuristic that:
 * Adds 20 to score if move is at a corner.
 * Subtracts 20 to score if move is adjacent to a corner.
 */
int Board::heuristic(Move *m, Side side)
{
    // Count tile advantage.
    int score = countAdvantage(side);
    
    // Use addition instead of multiplication.
    // Multiplication will not help if you have fewer tiles
    
    // If move is in corner, favor this move!
    if (isCorner(m))
    {
        score += 40;
    }
    // If move is adjacent to corner, disfavor this move!
    if (isNextToCorner(m))
    {
        score -= 20;
    }
    return score;
}

/*
 * CW
 * If this space is a corner, return true.
 */
bool Board::isCorner(Move *m)
{
    // Check for 00,07,70,77
    if (m->x == 0 || m->x == 7)
    {
        if (m->y == 0 || m->y == 7)
        {
            return true;
        }
    }
    return false;
}

/*
 * CW
 * If this space is adjacent to a corner, return true.
 */
bool Board::isNextToCorner(Move *m)
{
    // Check for:
    // 01,06,
    // 71,76,
    // 10,11,16,17,
    // 60,61,66,67
    if (m->x == 1 || m->x == 6)
    {
        if (m->y == 1 || m->y == 6)
        {
            return true;
        }
    }
    if (m->x == 1 || m->x == 6)
    {
        if (m->y < 2 || m->y > 5)
        {
            return true;
        }
    }
    return false;
}

/*
 * Current count of given side's stone advantage.
 */
int Board::countAdvantage(Side side) {
    return (side == BLACK) ? countBlack() - countWhite()
                           : countWhite() - countBlack();
}

/*
 * Current count of given side's stones.
 */
int Board::count(Side side) {
    return (side == BLACK) ? countBlack() : countWhite();
}

/*
 * Current count of black stones.
 */
int Board::countBlack() {
    return black.count();
}

/*
 * Current count of white stones.
 */
int Board::countWhite() {
    return taken.count() - black.count();
}

/*
 * Sets the board state given an 8x8 char array where 'w' indicates a white
 * piece and 'b' indicates a black piece. Mainly for testing purposes.
 */
void Board::setBoard(char data[]) {
    taken.reset();
    black.reset();
    for (int i = 0; i < 64; i++) {
        if (data[i] == 'b') {
            taken.set(i);
            black.set(i);
        } if (data[i] == 'w') {
            taken.set(i);
        }
    }
}
