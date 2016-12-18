// rats.cpp

// Portions you are to complete are marked with a TODO: comment.
// We've provided some incorrect return statements (so indicated) just
// to allow this skeleton program to compile and run, albeit incorrectly.
// The first thing you probably want to do is implement the utterly trivial
// functions (marked TRIVIAL).  Then get Arena::display going.  That gives
// you more flexibility in the order you tackle the rest of the functionality.
// As you finish implementing each TODO: item, remove its TODO: comment.

#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <cstdlib>
#include <cctype>
using namespace std;

///////////////////////////////////////////////////////////////////////////
// Manifest constants
///////////////////////////////////////////////////////////////////////////

const int MAXROWS = 20;            // max number of rows in the arena
const int MAXCOLS = 20;            // max number of columns in the arena
const int MAXRATS = 100;           // max number of rats allowed

const int NORTH = 0;
const int EAST  = 1;
const int SOUTH = 2;
const int WEST  = 3;
const int NUMDIRS = 4;

const int EMPTY      = 0;
const int HAS_POISON = 1;

///////////////////////////////////////////////////////////////////////////
// Type definitions
///////////////////////////////////////////////////////////////////////////

class Arena;  // This is needed to let the compiler know that Arena is a
// type name, since it's mentioned in the Rat declaration.

class Rat
{
public:
    // Constructor
    Rat(Arena* ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    bool isDead() const;
    
    // Mutators
    void move();
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    int    pelletsEaten;        //variable to count how many pellets eaten
    bool   whichTurnIsIt;      //bool to check if rat with 1 pellet eaten can move in the current turn
};

class Player
{
public:
    // Constructor
    Player(Arena* ap, int r, int c);
    
    // Accessors
    int  row() const;
    int  col() const;
    bool isDead() const;
    
    // Mutators
    string dropPoisonPellet();
    string move(int dir);
    void   setDead();
    
private:
    Arena* m_arena;
    int    m_row;
    int    m_col;
    bool   m_dead;
};

class Arena
{
public:
    // Constructor/destructor
    Arena(int nRows, int nCols);
    ~Arena();
    
    // Accessors
    int     rows() const;
    int     cols() const;
    Player* player() const;
    int     ratCount() const;
    int     getCellStatus(int r, int c) const;
    int     numberOfRatsAt(int r, int c) const;
    void    display(string msg) const;
    
    // Mutators
    void setCellStatus(int r, int c, int status);
    bool addRat(int r, int c);
    bool addPlayer(int r, int c);
    void moveRats();
    
private:
    int     m_grid[MAXROWS][MAXCOLS];
    int     m_rows;
    int     m_cols;
    Player* m_player;
    Rat*    m_rats[MAXRATS];
    int     m_nRats;
    int     m_turns;
    
    // Helper functions
    void checkPos(int r, int c) const;
    bool isPosInBounds(int r, int c) const;
};

class Game
{
public:
    // Constructor/destructor
    Game(int rows, int cols, int nRats);
    ~Game();
    
    // Mutators
    void play();
    
private:
    Arena* m_arena;
    
    // Helper functions
    string takePlayerTurn();
};

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function declarations
///////////////////////////////////////////////////////////////////////////

int randInt(int min, int max);
bool decodeDirection(char ch, int& dir);
bool attemptMove(const Arena& a, int dir, int& r, int& c);
bool recommendMove(const Arena& a, int r, int c, int& bestDir);
void clearScreen();

///////////////////////////////////////////////////////////////////////////
//  Rat implementation
///////////////////////////////////////////////////////////////////////////

Rat::Rat(Arena* ap, int r, int c)
{
    if (ap == nullptr)
    {
        cout << "***** A rat must be created in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "***** Rat created with invalid coordinates (" << r << ","
        << c << ")!" << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    pelletsEaten = 0;
    whichTurnIsIt = true;      //boolean to check whether the rat moves or not
}

int Rat::row() const
{
    return m_row;
}

int Rat::col() const
{
    return m_col;
}

bool Rat::isDead() const
{
    
    if(pelletsEaten<2)
        return false;
    return true;
}

void Rat::move()
{
    //COMMENT
    
    if((pelletsEaten == 1 && whichTurnIsIt) || pelletsEaten == 0){
        int randomDirection = randInt(1,NUMDIRS);   //initialize an int for random direction
        switch(randomDirection)
        {
            case 1:
                if(row() !=  m_arena -> rows()){
                    m_row++;
                }
                break;
            case 2:
                if(row() != 1){
                    m_row--;
                }
                break;
            case 3:
                if(col() != m_arena -> cols()){
                    m_col++;
                }
                break;
            case 4:
                if(col() != 1){
                    m_col--;
                }
                break;
            default:
                break;
        }
        if (m_arena -> numberOfRatsAt(m_row, m_col) > 0 &&  m_arena->getCellStatus(m_row, m_col)==HAS_POISON){
            pelletsEaten++;
            m_arena-> setCellStatus(m_row, m_col, EMPTY);
        }
        whichTurnIsIt = false;
    }
    else
        whichTurnIsIt = true;
}


///////////////////////////////////////////////////////////////////////////
//  Player implementation
///////////////////////////////////////////////////////////////////////////

Player::Player(Arena* ap, int r, int c)
{
    if (ap == nullptr)
    {
        cout << "***** The player must be created in some Arena!" << endl;
        exit(1);
    }
    if (r < 1  ||  r > ap->rows()  ||  c < 1  ||  c > ap->cols())
    {
        cout << "**** Player created with invalid coordinates (" << r
        << "," << c << ")!" << endl;
        exit(1);
    }
    m_arena = ap;
    m_row = r;
    m_col = c;
    m_dead = false;
}

int Player::row() const
{
    return m_row;
}

int Player::col() const
{
    return m_col;
}

string Player::dropPoisonPellet()
{
    if (m_arena->getCellStatus(m_row, m_col) == HAS_POISON)
        return "There's already a poison pellet at this spot.";
    m_arena->setCellStatus(m_row, m_col, HAS_POISON);
    return "A poison pellet has been dropped.";
}

string Player::move(int dir)
{
    switch(dir){
            
        case(NORTH):
            if(row()!=1){       //if in the first row, rat cannot move north
                m_row--;
                if((m_arena -> numberOfRatsAt(m_row, col())) > 0){  //if there is a rat at that position
                    setDead();
                    return "Player walked into a rat and died.";
                }
                else
                    return "Player moved north.";
            }
            return "Player couldn't move; player stands.";
            break;
            
        case(SOUTH):
            if(row() != m_arena->rows()){       //if in the last row, rat cannot move south
                m_row++;
                if((m_arena -> numberOfRatsAt(m_row, col())) > 0){
                    setDead();
                    return "Player walked into a rat and died.";
                }
                else
                    return "Player moved south.";
            }
            return "Player couldn't move; player stands.";
            break;
            
        case(WEST):         //if it is in the first column, rat cannot move west
            if(col() !=1){
                m_col--;
                if((m_arena -> numberOfRatsAt(row(), m_col)) > 0){
                    setDead();
                    return "Player walked into a rat and died.";
                }
                
                else
                    return "Player moved west.";
            }
            return "Player couldn't move; player stands.";
            break;
            
        case(EAST):        //if it is in the last column, rat cannot move east
            if(col() != m_arena ->cols()){
                m_col++;
                if((m_arena-> numberOfRatsAt(row(), m_col)) > 0){
                    setDead();
                    return "Player walked into a rat and died.";
                }
                else
                    return "Player moved east.";
                
            }
            return "Player couldn't move; player stands.";
            break;
    }
    return "Player couldn't move; player stands.";
}

bool Player::isDead() const
{
    return m_dead;
}

void Player::setDead()
{
    m_dead = true;
}

///////////////////////////////////////////////////////////////////////////
//  Arena implementation
///////////////////////////////////////////////////////////////////////////

Arena::Arena(int nRows, int nCols)
{
    if (nRows <= 0  ||  nCols <= 0  ||  nRows > MAXROWS  ||  nCols > MAXCOLS)
    {
        cout << "***** Arena created with invalid size " << nRows << " by "
        << nCols << "!" << endl;
        exit(1);
    }
    m_rows = nRows;
    m_cols = nCols;
    m_player = nullptr;
    m_nRats = 0;
    m_turns = 0;
    for (int r = 1; r <= m_rows; r++)
        for (int c = 1; c <= m_cols; c++)
            setCellStatus(r, c, EMPTY);
}

Arena::~Arena()
{
    delete m_player;        //when arena is destroyed, so is the player
    
    for(int j=0; j<m_nRats; j++)        //go through array and delete all rates
        delete m_rats[j];
}

int Arena::rows() const
{
    return m_rows;
}

int Arena::cols() const
{
    return m_cols;
}

Player* Arena::player() const
{
    return m_player;
}

int Arena::ratCount() const
{
    return m_nRats;
}

int Arena::getCellStatus(int r, int c) const
{
    checkPos(r, c);
    return m_grid[r-1][c-1];
}

int Arena::numberOfRatsAt(int r, int c) const
{
    int numberOfRats=0;
    for(int i=0; i<m_nRats; i++){       //loop through each rat in the array
        if(m_rats[i] -> row() == r && m_rats[i] -> col() ==c){ //check if it is in the specific position
            numberOfRats++;
        }
    }
    return numberOfRats;
}

void Arena::display(string msg) const
{
    char displayGrid[MAXROWS][MAXCOLS];
    int r, c;
    
    // Fill displayGrid with dots (empty) and stars (poison pellets)
    for (r = 1; r <= rows(); r++)
        for (c = 1; c <= cols(); c++)
            displayGrid[r-1][c-1] = (getCellStatus(r,c) == EMPTY ? '.' : '*');
    
    // Indicate each rat's position
    
    for(r=0; r<rows(); r++){
        for(c=0; c<cols(); c++){
            if(numberOfRatsAt(r+1, c+1) == 1)       //if there is one rat at that position, display 'R'
                displayGrid[r][c] = 'R';
            //if number of rats between 1 and 9, display the number corresponding to number of rats
            if(numberOfRatsAt(r+1, c+1)>1 && numberOfRatsAt(r+1, c+1)< 9)
                displayGrid[r][c] = numberOfRatsAt(r+1, c+1) + '0';
            //if greater than 9 just display 9
            if(numberOfRatsAt(r+1, c+1)>=9)
                displayGrid[r][c] = '9';
        }
    }
    
    // Indicate player's position
    if (m_player != nullptr)
        displayGrid[m_player->row()-1][m_player->col()-1] = (m_player->isDead() ? 'X' : '@');
    
    // Draw the grid
    clearScreen();
    for (r = 1; r <= rows(); r++)
    {
        for (c = 1; c <= cols(); c++)
            cout << displayGrid[r-1][c-1];
        cout << endl;
    }
    cout << endl;
    
    // Write message, rat, and player info
    if (msg != "")
        cout << msg << endl;
    cout << "There are " << ratCount() << " rats remaining." << endl;
    if (m_player == nullptr)
        cout << "There is no player!" << endl;
    else if (m_player->isDead())
        cout << "The player is dead." << endl;
    cout << m_turns << " turns have been taken." << endl;
}

void Arena::setCellStatus(int r, int c, int status)
{
    checkPos(r, c);
    m_grid[r-1][c-1] = status;
}

bool Arena::addRat(int r, int c)
{
    if (! isPosInBounds(r, c))
        return false;
    
    // Don't add a rat on a spot with a poison pellet
    if (getCellStatus(r, c) != EMPTY)
        return false;
    
    // Don't add a rat on a spot with a player
    if (m_player != nullptr  &&  m_player->row() == r  &&  m_player->col() == c)
        return false;
    
    if(ratCount() == MAXRATS)   //don't add anything if number of rats is at a maximum
        return false;
    
    else{
        m_rats[m_nRats] = new Rat(this, r, c);
        m_nRats++;      //pointer to the newly allocated rat is m_nRats-1 in the array
        return true;
    }
}

bool Arena::addPlayer(int r, int c)
{
    if (! isPosInBounds(r, c))
        return false;
    
    // Don't add a player if one already exists
    if (m_player != nullptr)
        return false;
    
    // Don't add a player on a spot with a rat
    if (numberOfRatsAt(r, c) > 0)
        return false;
    
    m_player = new Player(this, r, c);
    return true;
}

void Arena::moveRats()
{
   //m_turns++;
    for(int i=0; i<m_nRats; i++){
        m_rats[i]->move();  //calls the move function in the Rat class
        if((m_player->row() == m_rats[i]->row()) &&     //if the player is in the same spot as a rat
           (m_player->col() == m_rats[i]->col())){      //the player dies
            m_player -> setDead();
        }
        else if(m_rats[i]->isDead()){       //checks if rat is dead (eats 2 pellets)
            delete m_rats[i];               //deletes it
            m_rats[i] = m_rats[m_nRats-1];  //reallocates the pointer to the last pointer in array
            m_nRats--;                      //decrement so that this rat is never checked
        }
    }
    m_turns++;
}

bool Arena::isPosInBounds(int r, int c) const
{
    return (r >= 1  &&  r <= m_rows  &&  c >= 1  &&  c <= m_cols);
}

void Arena::checkPos(int r, int c) const
{
    if (r < 1  ||  r > m_rows  ||  c < 1  ||  c > m_cols)
    {
        cout << "***** " << "Invalid arena position (" << r << ","
        << c << ")" << endl;
        exit(1);
    }
}


///////////////////////////////////////////////////////////////////////////
//  Game implementation
///////////////////////////////////////////////////////////////////////////

Game::Game(int rows, int cols, int nRats)
{
    if (nRats < 0)
    {
        cout << "***** Cannot create Game with negative number of rats!" << endl;
        exit(1);
    }
    if (nRats > MAXRATS)
    {
        cout << "***** Trying to create Game with " << nRats
        << " rats; only " << MAXRATS << " are allowed!" << endl;
        exit(1);
    }
    if (rows == 1  &&  cols == 1  &&  nRats > 0)
    {
        cout << "***** Cannot create Game with nowhere to place the rats!" << endl;
        exit(1);
    }
    
    // Create arena
    m_arena = new Arena(rows, cols);
    
    // Add player
    int rPlayer;
    int cPlayer;
    do
    {
        rPlayer = randInt(1, rows);
        cPlayer = randInt(1, cols);
    } while (m_arena->getCellStatus(rPlayer, cPlayer) != EMPTY);
    m_arena->addPlayer(rPlayer, cPlayer);
    
    // Populate with rats
    while (nRats > 0)
    {
        int r = randInt(1, rows);
        int c = randInt(1, cols);
        if (r == rPlayer && c == cPlayer)
            continue;
        m_arena->addRat(r, c);
        nRats--;
    }
}

Game::~Game()
{
    delete m_arena;
}

string Game::takePlayerTurn()
{
    for (;;)
    {
        cout << "Your move (n/e/s/w/x or nothing): ";
        string playerMove;
        getline(cin, playerMove);
        
        Player* player = m_arena->player();
        int dir;
        
        if (playerMove.size() == 0)
        {
            if (recommendMove(*m_arena, player->row(), player->col(), dir))
                return player->move(dir);
            else
                return player->dropPoisonPellet();
        }
        else if (playerMove.size() == 1)
        {
            if (tolower(playerMove[0]) == 'x')
                return player->dropPoisonPellet();
            else if (decodeDirection(playerMove[0], dir))
                return player->move(dir);
        }
        cout << "Player move must be nothing, or 1 character n/e/s/w/x." << endl;
    }
}

void Game::play()
{
    m_arena->display("");
    while ( ! m_arena->player()->isDead()  &&  m_arena->ratCount() > 0)
    {
        string msg = takePlayerTurn();
        Player* player = m_arena->player();
        if (player->isDead())
        {
            cout << msg << endl;
            break;
        }
        m_arena->moveRats();
        m_arena->display(msg);
    }
    if (m_arena->player()->isDead())
        cout << "You lose." << endl;
    else
        cout << "You win." << endl;
}

///////////////////////////////////////////////////////////////////////////
//  Auxiliary function implementation
///////////////////////////////////////////////////////////////////////////

// Return a uniformly distributed random int from min to max, inclusive
int randInt(int min, int max)
{
    if (max < min)
        swap(max, min);
    static random_device rd;
    static mt19937 generator(rd());
    uniform_int_distribution<> distro(min, max);
    return distro(generator);
}

bool decodeDirection(char ch, int& dir)
{
    switch (tolower(ch))
    {
        default:  return false;
        case 'n': dir = NORTH; break;
        case 'e': dir = EAST;  break;
        case 's': dir = SOUTH; break;
        case 'w': dir = WEST;  break;
    }
    return true;
}

// Return false without changing anything if moving one step from (r,c)
// in the indicated direction would run off the edge of the arena.
// Otherwise, update r and c to the position resulting from the move and
// return true.

//Did not use the following function in my program.
bool attemptMove(const Arena& a, int dir, int& r, int& c)
{
    //same premise as the move() function. Checks if the move is valid, and moves the player accordingly.
    switch(dir){
        case NORTH:
            if(r!=1){
                r--;
                return true;
            }
            break;
        case SOUTH:
            if(r!=a.rows()){
                r++;
                return true;
            }
            break;
        case WEST:
            if(c!=1){
                c--;
                return true;
            }
            break;
        case EAST:
            if(c!=a.cols()){
                c++;
                return true;
            }
            break;
    }
    return false;
    
}


// Recommend a move for a player at (r,c):  A false return means the
// recommendation is that the player should drop a poison pellet and not
// move; otherwise, this function sets bestDir to the recommended
// direction to move and returns true.

//HELPER FUNCTIONS FOR RECOMMEND MOVE:

bool validMove(const Arena& a, int dir, int r, int c);
int countRatsInRadiusUpOrDown(const Arena& a, int r, int c, bool isUp);
int countRatsInRadiusRightOrLeft(const Arena& a, int r, int c, bool isLeft);

/***********************************************************************************************
 ***********************************************************************************************
 
Explanation of recommendMove implementation (artificial intelligence algorithm)
 
 Checks if the player is next to a rat (i.e. if player does not move, there is a chance of being eaten by rat). If so, move the player to a position with the least probability of being consumed by a rat. To do this, the algorithm checks all four moves (NORTH, SOUTH, etc.) and sees how many rats are adjacent to the new position before moving. If there are no rats next to the player (player not in immediate danger), drop a poison pellet. If there are no rats next to the player and there is already a pellet on the ground, move to the least dangerous position in the hope of dropping a poison pellet there (populates board with pellets so rats die quicker).

 This algorithm is able to successfully win a game against:
    - 30 rats in a 10 x 10 grid in ~400 moves. Game(10,10,30)
    - 100 rats in a 20 x 20 grid in 1125 moves. Game(20,20,100)
    - 8 rats in a 5 x 5 grid in ~100 moves. Game(5,5,8)
 ***********************************************************************************************
 ***********************************************************************************************/

bool recommendMove(const Arena& a, int r, int c, int& bestDir)
{
    double distance = 25;  //initialize distance to greater than distance of corner point in arena to opposite corner in a 20 x 20 grid (max distance possible) -> minDistance will always be less.
    
    int countRatsInRadiusNorthSouth;  //variable showing the number of rats in the radius of the player after the better move (NORTH or SOUTH)
    int countRatsInRadiusEastWest;  //same as above, but for EAST and WEST
    
    int bestDirNorthSouth = NORTH;  //assigned to bestDir at the end
    int bestDirEastWest = EAST;
    
    double minDistance=distance;    //minDistance is updated whenever a rat is closer to a player
    for(int i=1; i<=a.rows(); i++)
    {
        for(int j=1; j<=a.cols(); j++)
        {
            if(a.numberOfRatsAt(i, j)>0)
                distance = sqrt(((r-i)*(r-i))+((c-j)*(c-j)));   //distance formula
            if(distance<minDistance)
            {
                minDistance=distance;
            }
        }
    }
    
    if(minDistance==1)      //if there is a rat next to a player, then the player should move
    {
        //following int values represent the number of rats adjacent to player after the respective move is made
        int countRatsInRadiusNorth = countRatsInRadiusUpOrDown(a, r, c, true);
        int countRatsInRadiusSouth = countRatsInRadiusUpOrDown(a, r, c, false);
        int countRatsInRadiusWest = countRatsInRadiusRightOrLeft(a, r, c, true);
        int countRatsInRadiusEast = countRatsInRadiusRightOrLeft(a, r, c, false);
        
        //if value is 4 (4 rats can never be adjacent to a player), then the move is either invalid, or moving would cause the player to move into a rat (this is how the helper functions were designed).
        
        if(countRatsInRadiusSouth == 4 && countRatsInRadiusNorth !=4){
            countRatsInRadiusNorthSouth = countRatsInRadiusNorth;   //NORTH is better option than SOUTH
            bestDirNorthSouth = NORTH;
        }
        else if(countRatsInRadiusNorth == 4 && countRatsInRadiusSouth != 4){
            countRatsInRadiusNorthSouth = countRatsInRadiusSouth;
            bestDirNorthSouth = SOUTH;
        }
        //if both moves return 4, then the value given to the better move variable is -1
        else if(countRatsInRadiusNorth == 4 && countRatsInRadiusSouth ==4)
            countRatsInRadiusNorthSouth = -1;
        
        //if both moves are identical, then randomly select a move and assign it to countRatsInRadiusNorthSouth
        else if(countRatsInRadiusNorth == countRatsInRadiusSouth){
            if(randInt(0, 1)==0){
                countRatsInRadiusNorthSouth = countRatsInRadiusNorth;
                bestDirNorthSouth = NORTH;
            }
            else{
                countRatsInRadiusNorthSouth = countRatsInRadiusSouth;
                bestDirNorthSouth = SOUTH;
            }
            
        }
        else{
            //if one is less than the other, then the lesser is better
            if(countRatsInRadiusNorth < countRatsInRadiusSouth){
                countRatsInRadiusNorthSouth = countRatsInRadiusNorth;
                bestDirNorthSouth = NORTH;
            }
            
            else{
                countRatsInRadiusNorthSouth = countRatsInRadiusSouth;
                bestDirNorthSouth = SOUTH;
            }
        }
        /**************************************************************/
        
        //same as above except for EAST vs. WEST
        if(countRatsInRadiusEast == 4 && countRatsInRadiusWest !=4){
            countRatsInRadiusEastWest = countRatsInRadiusWest;
            bestDirEastWest = WEST;
        }
        
        else if(countRatsInRadiusWest == 4 && countRatsInRadiusEast != 4){
            countRatsInRadiusEastWest = countRatsInRadiusEast;
            bestDirEastWest = EAST;
        }
        else if(countRatsInRadiusEast == 4 && countRatsInRadiusWest ==4)
            countRatsInRadiusEastWest = -1;
        
        else if(countRatsInRadiusEast == countRatsInRadiusWest){
            if(randInt(0, 1)==0){
                countRatsInRadiusEastWest = countRatsInRadiusEast;
                bestDirEastWest = EAST;
            }
            else{
                countRatsInRadiusEastWest = countRatsInRadiusWest;
                bestDirEastWest = WEST;
            }
            
        }
        
        else{
            if(countRatsInRadiusEast < countRatsInRadiusWest){
                countRatsInRadiusEastWest = countRatsInRadiusEast;
                bestDirEastWest = EAST;
            }
            else{
                countRatsInRadiusEastWest = countRatsInRadiusWest;
                bestDirEastWest = WEST;
            }
        }
        /**********************************************************/
        
        //if none of the moves are valid, (since both the variables are -1), then stay put.
        if(countRatsInRadiusNorthSouth== -1 && countRatsInRadiusEastWest== -1){
            return false;
        }
        //if EastWest returns -1 that means both EAST and WEST are invalid moves, meaning that NorthSouth is the better direction
        else if(countRatsInRadiusEastWest== -1)
            bestDir = bestDirNorthSouth;
        //same as above
        else if(countRatsInRadiusNorthSouth== -1)
            bestDir = bestDirEastWest;
        
        else{
            //if the better of NorthSouth has less rats in its radius than the better of EastWest, then move in that NorthSouth direction (NorthSouth assigned earlier)
            if(countRatsInRadiusNorthSouth < countRatsInRadiusEastWest)
                bestDir = bestDirNorthSouth;
            //if they are equal, then randomly select a direction
            else if(countRatsInRadiusNorthSouth == countRatsInRadiusEastWest){
                if(randInt(0, 1)==0){
                    bestDir = bestDirNorthSouth;
                }
                else
                    bestDir = bestDirEastWest;
            }
            else
                bestDir = bestDirEastWest;
        }
        
        return true;
    }
    
    //The following else if block checks that there are no adjacent rats, and if the current position already has a pellet, there is no use in dropping another pellet, so move to another position. Will only move if the position being moved to has 0 chance of being populated by rat. Essentially, don't take the risk of moving to a position where you could be eaten by a rat just for the sake of maybe putting a pellet.
    
    else if(minDistance!=1 && a.getCellStatus(r, c)==HAS_POISON){
        //using same variables, but in local scope
        int countRatsInRadiusNorth = countRatsInRadiusUpOrDown(a, r, c, true);
        int countRatsInRadiusSouth = countRatsInRadiusUpOrDown(a, r, c, false);
        int countRatsInRadiusWest = countRatsInRadiusRightOrLeft(a, r, c, true);
        int countRatsInRadiusEast = countRatsInRadiusRightOrLeft(a, r, c, false);
        bool isZero[4] = {false, false, false, false};  //array with bool values for whether the position has 0 rats around it
        int onlyZeros[4]={};    //array that holds these positions
        int counter=0;
        int noPelletsThere[4]={};   //array that holds the positions with 0 rats around them AND positions without pellets
        int counter2=0;
        
        if(countRatsInRadiusNorth==0)
            isZero[0] = true;           //bool is true if the position has 0 rats adjacent
        if(countRatsInRadiusEast==0)
            isZero[1] = true;
        if(countRatsInRadiusSouth==0)
            isZero[2] = true;
        if(countRatsInRadiusWest==0)
            isZero[3] = true;
        
        for(int i=0; i<4; i++){         //loop to go through the bool array
            if(isZero[i] == true){
                onlyZeros[counter] = i;   //if the ith value is true append the direction value to the onlyZeros array
                counter++;          //increment counter so next time it will append to next position
            }
        }
        if (counter==0)     //if none of the values are true, then don't move
            return false;
        
        else
        {
            for(int i=0; i<counter; i++){       //loop through the onlyZeros array
                switch(onlyZeros[i]){
                    case 0:
                        if(a.getCellStatus(r-1, c)==EMPTY){     //if the position has no pellets, it is even better because you can put a pellet there
                            noPelletsThere[counter2]=0; //append the value to the noPellets array
                            counter2++;
                        }
                        break;
                    case 1:
                        if(a.getCellStatus(r, c+1)==EMPTY){
                            noPelletsThere[counter2]=1;
                            counter2++;
                        }
                        break;
                    case 2:
                        if(a.getCellStatus(r+1, c)==EMPTY){
                            noPelletsThere[counter2]=2;
                            counter2++;
                        }
                        break;
                    case 3:
                        if(a.getCellStatus(r, c-1)==EMPTY){
                            noPelletsThere[counter2]=3;
                            counter2++;
                        }
                        break;
                }
            }
            int chooseRandomDirection = randInt(0, counter2-1);  //randomly select a value in the array
            for(int i=0; i<counter2; i++){
                if(i==chooseRandomDirection){       //loop through and find the random value
                    bestDir = noPelletsThere[i];    //assign bestDir to this value
                    break;
                }
            }
            if(counter2 == 0){     //if there are no positions with no pellets, then move to any of the positions in the onlyZeros array
                chooseRandomDirection = randInt(0, counter-1);
                for(int i=0; i<counter; i++){
                    if(i==chooseRandomDirection){
                        bestDir = onlyZeros[i];
                        break;
                    }
                        
                }
            }
            return true;
        }
        
    }
    //drop a pellet if you are in an empty position with no rats adjacent
    else
        return false;
    
}

bool validMove(const Arena& a, int dir, int r, int c)
{
    //same function as attempt move except for the fact that r and c are not passed by reference (not changed) and r and c not decremented/incremented. Checks if a move is valid or not
    
    switch(dir){
        case NORTH:
            if(r!=1){
                return true;
            }
            break;
        case SOUTH:
            if(r!=a.rows()){
                return true;
            }
            break;
        case WEST:
            if(c!=1){
                return true;
            }
            break;
        case EAST:
            if(c!=a.cols()){
                return true;
            }
            break;
    }
    return false;
    
}

//Helper function that tells the program how many rats are in the radius of the player after moving in the intended direction (NORTH or SOUTH), which is passed as the argument
int countRatsInRadiusUpOrDown(const Arena& a, int r, int c, bool isUp){
    int countRatsInRadius=4;    //initial value, which if unchanged, means that the move is invalid
    int flippedR = r;
    int CheckifRatUpOrDown;
    if(isUp)                //if the move is NORTH
        CheckifRatUpOrDown = -1;    //if NORTH, have to check one row up (r-1)
    else{
        CheckifRatUpOrDown = 1;     //if SOUTH have to check one row down (r+1)
        flippedR = a.rows() - r +1;     //flip the r value so that the program uses the same code to check whether SOUTH is a valid move
    }
    
    if(validMove(a, NORTH, flippedR, c) && a.numberOfRatsAt(r+CheckifRatUpOrDown, c)==0)
    {
        switch(flippedR)
        {
            //if moving NORTH and player is in row 2 or moving SOUTH and player in second last row, then don't check if rat above it, since that is undefined.
            case 2:
                //if in first column, don't check position to the left, since undefined
                if(c==1){
                    countRatsInRadius = a.numberOfRatsAt(r+CheckifRatUpOrDown, c+1);
                    break;
                }
                //if in last column, don;t check position to the right, cince undefined
                if(c==a.cols()){
                    countRatsInRadius = a.numberOfRatsAt(r+CheckifRatUpOrDown, c-1);
                    break;
                }
                //otherwise, check two positions adjacent to the newly moved position (say you are in position 2,2 (row, column) in a 4x4 grid, and moving north (move to 1,2), then check positions (1,3) and (1,1) for rats
                else{
                    countRatsInRadius =  a.numberOfRatsAt(r+CheckifRatUpOrDown, c+1)+ a.numberOfRatsAt(r+CheckifRatUpOrDown, c-1);
                    break;
                }
            //any other row, same logic of not checking undefined positions
            default:
                if(c==1){
                    countRatsInRadius = a.numberOfRatsAt(r+(2*CheckifRatUpOrDown), c)+ a.numberOfRatsAt(r+CheckifRatUpOrDown, c+1);
                    break;
                }
                if(c==a.cols()){
                    countRatsInRadius = a.numberOfRatsAt(r+(2*CheckifRatUpOrDown), c)+ a.numberOfRatsAt(r+CheckifRatUpOrDown, c-1);
                    break;
                }
                else{
                    countRatsInRadius = a.numberOfRatsAt(r+(2*CheckifRatUpOrDown), c)+ a.numberOfRatsAt(r+CheckifRatUpOrDown, c-1)+ a.numberOfRatsAt(r+CheckifRatUpOrDown, c+1);
                    break;
                }
        }
    }
    return countRatsInRadius;
}

//same logic as the above function, applied to EAST and WEST
int countRatsInRadiusRightOrLeft(const Arena& a, int r, int c, bool isLeft){
    int countRatsInRadius=4;
    int flippedC = c;
    int CheckRatsLeftOrRight;
    if(isLeft)
        CheckRatsLeftOrRight = -1;
    else{
        CheckRatsLeftOrRight = 1;
        flippedC = a.cols() - c +1;
    }
    
    if(validMove(a, WEST, r, flippedC) && a.numberOfRatsAt(r, c+CheckRatsLeftOrRight)==0)
    {
        switch(flippedC)
        {
            case 2:
                if(r==1){
                    countRatsInRadius = a.numberOfRatsAt(r+1, c+CheckRatsLeftOrRight);
                    break;
                }
                if(r==a.rows()){
                    countRatsInRadius = a.numberOfRatsAt(r-1, c+CheckRatsLeftOrRight);
                    break;
                }
                else{
                    countRatsInRadius =  a.numberOfRatsAt(r+1, c+CheckRatsLeftOrRight)+ a.numberOfRatsAt(r-1, c+CheckRatsLeftOrRight);
                    break;
                }
            default:
                if(r==1){
                    countRatsInRadius = a.numberOfRatsAt(r, c+(2*CheckRatsLeftOrRight))+ a.numberOfRatsAt(r+1, c+CheckRatsLeftOrRight);
                    break;
                }
                if(r==a.rows()){
                    countRatsInRadius = a.numberOfRatsAt(r, c+(2*CheckRatsLeftOrRight))+ a.numberOfRatsAt(r-1, c+CheckRatsLeftOrRight);
                    break;
                }
                else{
                    countRatsInRadius = a.numberOfRatsAt(r, c+(2*CheckRatsLeftOrRight))+ a.numberOfRatsAt(r-1, c+CheckRatsLeftOrRight)+ a.numberOfRatsAt(r+1, c+CheckRatsLeftOrRight);
                    break;
                }
        }
    }
    return countRatsInRadius;
}


#include <type_traits>
#include <cassert>

#define CHECKTYPE(c, f, r, a)  \
static_assert(std::is_same<decltype(&c::f), r (c::*)a>::value, \
"FAILED: You changed the type of " #c "::" #f);  \
[[gnu::unused]] r (c::* xxx##c##_##f) a = &c::f

void thisFunctionWillNeverBeCalled()
{
    // If the student deleted or changed the interfaces to the public
    // functions, this won't compile.  (This uses magic beyond the scope
    // of CS 31.)
    
    Rat r(static_cast<Arena*>(0), 1, 1);
    CHECKTYPE(Rat, row, int, () const);
    CHECKTYPE(Rat, col, int, () const);
    CHECKTYPE(Rat, isDead, bool, () const);
    CHECKTYPE(Rat, move, void, ());
    
    Player p(static_cast<Arena*>(0), 1, 1);
    CHECKTYPE(Player, row, int, () const);
    CHECKTYPE(Player, col, int, () const);
    CHECKTYPE(Player, isDead, bool, () const);
    CHECKTYPE(Player, dropPoisonPellet, string, ());
    CHECKTYPE(Player, move, string, (int));
    CHECKTYPE(Player, setDead, void, ());
    
    Arena a(1, 1);
    CHECKTYPE(Arena, rows, int, () const);
    CHECKTYPE(Arena, cols, int, () const);
    CHECKTYPE(Arena, player, Player*, () const);
    CHECKTYPE(Arena, ratCount, int, () const);
    CHECKTYPE(Arena, getCellStatus, int, (int,int) const);
    CHECKTYPE(Arena, numberOfRatsAt, int, (int,int) const);
    CHECKTYPE(Arena, display, void, (string) const);
    CHECKTYPE(Arena, setCellStatus, void, (int,int,int));
    CHECKTYPE(Arena, addRat, bool, (int,int));
    CHECKTYPE(Arena, addPlayer, bool, (int,int));
    CHECKTYPE(Arena, moveRats, void, ());
    
    Game g(1,1,1);
    CHECKTYPE(Game, play, void, ());
}

void findTheRat(const Arena& a, int& r, int& c)
{
    if      (a.numberOfRatsAt(r-1, c) == 1) r--;
    else if (a.numberOfRatsAt(r+1, c) == 1) r++;
    else if (a.numberOfRatsAt(r, c-1) == 1) c--;
    else if (a.numberOfRatsAt(r, c+1) == 1) c++;
    else assert(false);
}

void doBasicTests()
{
    {
        Arena a(10, 20);
        a.addPlayer(2, 5);
        Player* pp = a.player();
        assert(pp->row() == 2  &&  pp->col() == 5  && ! pp->isDead());
        assert(pp->move(NORTH) == "Player moved north.");
        assert(pp->row() == 1  &&  pp->col() == 5  && ! pp->isDead());
        assert(pp->move(NORTH) == "Player couldn't move; player stands.");
        assert(pp->row() == 1  &&  pp->col() == 5  && ! pp->isDead());
        pp->setDead();
        assert(pp->row() == 1  &&  pp->col() == 5  && pp->isDead());
    }
    {
        Arena a(10, 20);
        int r = 4;
        int c = 4;
        a.setCellStatus(r-1, c, HAS_POISON);
        a.setCellStatus(r+1, c, HAS_POISON);
        a.setCellStatus(r, c-1, HAS_POISON);
        a.setCellStatus(r, c+1, HAS_POISON);
        a.addRat(r, c);
        a.addPlayer(8, 18);
        assert(a.ratCount() == 1  &&  a.numberOfRatsAt(r, c) == 1);
        a.moveRats();
        assert(a.ratCount() == 1  &&  a.numberOfRatsAt(r, c) == 0);
        findTheRat(a, r, c);
        assert(a.getCellStatus(r, c) != HAS_POISON);
        a.moveRats();
        assert(a.ratCount() == 1  &&  a.numberOfRatsAt(r, c) == 1);
        a.moveRats();
        assert(a.ratCount() == 1  &&  a.numberOfRatsAt(r, c) == 0);
        findTheRat(a, r, c);
        a.moveRats();
        assert(a.ratCount() == 1  &&  a.numberOfRatsAt(r, c) == 1);
        a.setCellStatus(r-1, c, HAS_POISON);
        a.setCellStatus(r+1, c, HAS_POISON);
        a.setCellStatus(r, c-1, HAS_POISON);
        a.setCellStatus(r, c+1, HAS_POISON);
        a.moveRats();
        assert(a.ratCount() == 0  &&  a.numberOfRatsAt(r, c) == 0);
        assert(a.numberOfRatsAt(r-1, c) == 0);
        assert(a.numberOfRatsAt(r+1, c) == 0);
        assert(a.numberOfRatsAt(r, c-1) == 0);
        assert(a.numberOfRatsAt(r, c+1) == 0);
        
        for (int k = 0; k < MAXRATS/4; k++)
        {
            a.addRat(7, 18);
            a.addRat(9, 18);
            a.addRat(8, 17);
            a.addRat(8, 19);
        }
        assert(! a.player()->isDead());
        a.moveRats();
        assert(a.player()->isDead());
    }
    cout << "Passed all basic tests" << endl;
}

///////////////////////////////////////////////////////////////////////////
// main()
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  clearScreen implementation
///////////////////////////////////////////////////////////////////////////

// DO NOT MODIFY OR REMOVE ANY CODE BETWEEN HERE AND THE END OF THE FILE!!!
// THE CODE IS SUITABLE FOR VISUAL C++, XCODE, AND g++ UNDER LINUX.

// Note to Xcode users:  clearScreen() will just write a newline instead
// of clearing the window if you launch your program from within Xcode.
// That's acceptable.  (The Xcode output window doesn't have the capability
// of being cleared.)

#ifdef _MSC_VER  //  Microsoft Visual C++

#pragma warning(disable : 4005)
#include <windows.h>

void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    COORD upperLeft = { 0, 0 };
    DWORD dwCharsWritten;
    FillConsoleOutputCharacter(hConsole, TCHAR(' '), dwConSize, upperLeft,
                               &dwCharsWritten);
    SetConsoleCursorPosition(hConsole, upperLeft);
}

#else  // not Microsoft Visual C++, so assume UNIX interface

#include <iostream>
#include <cstring>
#include <cstdlib>

void clearScreen()  // will just write a newline in an Xcode output window
{
    static const char* term = getenv("TERM");
    if (term == nullptr  ||  strcmp(term, "dumb") == 0)
        cout << endl;
    else
    {
        static const char* ESC_SEQ = "\x1B[";  // ANSI Terminal esc seq:  ESC [
        cout << ESC_SEQ << "2J" << ESC_SEQ << "H" << flush;
    }
}

#endif

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <cassert>
using namespace std;

class streambuf_switcher
{
public:
    streambuf_switcher(ios& dest, ios& src)
    : dest_stream(dest), saved_streambuf(dest.rdbuf(src.rdbuf()))
    {}
    ~streambuf_switcher()
    {
        dest_stream.rdbuf(saved_streambuf);
    }
private:
    ios& dest_stream;
    streambuf* saved_streambuf;
};

set<void*> addrs;
bool recordaddrs = false;

void* operator new(size_t n)
{
    void* p = malloc(n);
    fill_n(static_cast<char*>(p), n, 0xca);
    if (recordaddrs  &&  (n == sizeof(Rat)  ||  n == sizeof(Player)))
    {
        recordaddrs = false;
        addrs.insert(p);
        recordaddrs = true;
    }
    return p;
}

void operator delete(void* p) noexcept
{
    if (recordaddrs)
    {
        recordaddrs = false;
        set<void*>::iterator it = addrs.find(p);
        if (it != addrs.end())
            addrs.erase(it);
            recordaddrs = true;
            }
    free(p);
}

bool recommendMove(const Arena& a, int r, int c, int& bestDir);

void testone(int n)
{
    streambuf_switcher sso(cout, cerr);
    
    switch (n)
    {
        default: {
            cout << "Bad argument" << endl;
        } break; case  1: {
            int k;
            for (k = 0; k < 300; k++)
            {
                Arena a(1, 20);
                a.addPlayer(1, 3);
                Rat r(&a, 1, 18);
                assert(r.row() == 1  &&  r.col() == 18);
                r.move(); assert(r.row() == 1);
                if (r.col() != 19)
                {
                    assert(r.col() == 17  ||  r.col() == 18);
                    continue;
                }
                r.move(); assert(r.row() == 1);
                if (r.col() == 20)
                    break;
                assert(r.col() == 18  ||  r.col() == 19);
            }
            assert(k < 300);
        } break; case  2: {
            int k;
            for (k = 0; k < 600; k++)
            {
                Arena a(1, 20);
                a.addPlayer(1, 3);
                Rat r(&a, 1, 19);
                assert(r.row() == 1  &&  r.col() == 19);
                r.move(); assert(r.row() == 1);
                if (r.col() != 19)
                {
                    assert(r.col() == 18  ||  r.col() == 20);
                    continue;
                }
                r.move(); assert(r.row() == 1);
                if (r.col() != 19)
                {
                    assert(r.col() == 18  ||  r.col() == 20);
                    continue;
                }
                r.move(); assert(r.row() == 1);
                if (r.col() != 19)
                {
                    assert(r.col() == 18  ||  r.col() == 20);
                    continue;
                }
                r.move(); assert(r.row() == 1);
                if (r.col() != 19)
                {
                    assert(r.col() == 18  ||  r.col() == 20);
                    continue;
                }
                r.move(); assert(r.row() == 1);
                if (r.col() == 18  ||  r.col() == 20)
                    break;
                assert(r.col() == 19);
            }
            assert(k < 600);
        } break; case  3: {
            int k;
            for (k = 0; k < 600; k++)
            {
                Arena a(20, 1);
                a.addPlayer(3, 1);
                Rat r(&a, 19, 1);
                assert(r.row() == 19  &&  r.col() == 1);
                r.move(); assert(r.col() == 1);
                if (r.row() != 19)
                {
                    assert(r.row() == 18  ||  r.row() == 20);
                    continue;
                }
                r.move(); assert(r.col() == 1);
                if (r.row() != 19)
                {
                    assert(r.row() == 18  ||  r.row() == 20);
                    continue;
                }
                r.move(); assert(r.col() == 1);
                if (r.row() != 19)
                {
                    assert(r.row() == 18  ||  r.row() == 20);
                    continue;
                }
                r.move(); assert(r.col() == 1);
                if (r.row() != 19)
                {
                    assert(r.row() == 18  ||  r.row() == 20);
                    continue;
                }
                r.move(); assert(r.col() == 1);
                if (r.row() == 18  ||  r.row() == 20)
                    break;
                assert(r.row() == 19);
            }
            assert(k < 600);
        } break; case  4: {
            Arena a(10,20);
            a.addPlayer(9, 19);
            for (int r = 5-2; r <= 5+2; r++)
                for (int c = 10-2; c <= 10+2; c++)
                    a.setCellStatus(r, c, HAS_POISON);
            Rat r(&a, 5, 10);
            r.move();
            assert((r.row() == 5  &&  abs(r.col() - 10) == 1)  ||
                   (r.col() == 10  &&  abs(r.row() - 5) == 1));
            int oldr = r.row();
            int oldc = r.col();
            r.move();
            assert(r.row() == oldr  &&  r.col() == oldc);
            r.move();
            assert((r.row() == oldr  &&  abs(r.col() - oldc) == 1)  ||
                   (r.col() == oldc  &&  abs(r.row() - oldr) == 1));
        } break; case  5: {
            Arena a(10,20);
            a.addPlayer(9, 19);
            for (int r = 5-2; r <= 5+2; r++)
                for (int c = 10-2; c <= 10+2; c++)
                    if (r != 5  ||  c != 10)
                        a.setCellStatus(r, c, HAS_POISON);
            Rat r(&a, 5, 10);
            assert(!r.isDead());
            r.move();
            assert((r.row() == 5  &&  abs(r.col() - 10) == 1)  ||
                   (r.col() == 10  &&  abs(r.row() - 5) == 1));
            int oldr = r.row();
            int oldc = r.col();
            assert(!r.isDead());
            a.setCellStatus(5, 10, HAS_POISON);
            r.move();
            assert(r.row() == oldr  &&  r.col() == oldc);
            assert(!r.isDead());
            r.move();
            assert((r.row() == oldr  &&  abs(r.col() - oldc) == 1)  ||
                   (r.col() == oldc  &&  abs(r.row() - oldr) == 1));
            assert(r.isDead());
        } break; case  6: {
            Arena a(10,20);
            a.addPlayer(9, 19);
            a.setCellStatus(4, 10, HAS_POISON);
            a.setCellStatus(6, 10, HAS_POISON);
            a.setCellStatus(5, 9, HAS_POISON);
            a.setCellStatus(5, 11, HAS_POISON);
            Rat r(&a, 5, 10);
            r.move();
            assert((r.row() == 5  &&  abs(r.col() - 10) == 1)  ||
                   (r.col() == 10  &&  abs(r.row() - 5) == 1));
            int oldr = r.row();
            int oldc = r.col();
            r.move();
            assert(r.row() == oldr  &&  r.col() == oldc);
            r.move();
            assert((r.row() == oldr  &&  abs(r.col() - oldc) == 1)  ||
                   (r.col() == oldc  &&  abs(r.row() - oldr) == 1));
            oldr = r.row();
            oldc = r.col();
            r.move();
            assert(r.row() == oldr  &&  r.col() == oldc);
            a.setCellStatus(oldr-1, oldc, EMPTY);
            a.setCellStatus(oldr+1, oldc, EMPTY);
            a.setCellStatus(oldr, oldc-1, EMPTY);
            a.setCellStatus(oldr, oldc+1, EMPTY);
            r.move();
            assert((r.row() == oldr  &&  abs(r.col() - oldc) == 1)  ||
                   (r.col() == oldc  &&  abs(r.row() - oldr) == 1));
        } break; case  7: {
            Arena a(1, 20);
            a.addPlayer(1, 3);
            Player* p = a.player();
            assert(p->row() == 1  &&  p->col() == 3);
            p->move(WEST); assert(p->row() == 1  &&  p->col() == 2);
            p->move(WEST); assert(p->row() == 1  &&  p->col() == 1);
        } break; case  8: {
            Arena a(1, 20);
            a.addPlayer(1, 3);
            Player* p = a.player();
            p->move(WEST); assert(p->row() == 1  &&  p->col() == 2);
            p->move(WEST); assert(p->row() == 1  &&  p->col() == 1);
            p->move(WEST); assert(p->row() == 1  &&  p->col() == 1);
            p->move(NORTH); assert(p->row() == 1  &&  p->col() == 1);
            p->move(SOUTH); assert(p->row() == 1  &&  p->col() == 1);
        } break; case  9: {
            Arena a(10, 20);
            a.addPlayer(3, 6);
            Player* p = a.player();
            assert( ! p->isDead());
            p->setDead();
            assert(p->isDead());
        } break; case 10: {
            Arena a(20, 1);
            a.addPlayer(1, 1);
            Player* p = a.player();
            assert(p->move(WEST).find(" stands") != string::npos);
            assert(p->move(EAST).find(" stands") != string::npos);
            assert(p->move(NORTH).find(" stands") != string::npos);
            assert(p->move(SOUTH).find(" south") != string::npos);
        } break; case 11: {
            Arena a(20, 1);
            a.addPlayer(1, 1);
            Player* p = a.player();
            a.addRat(2, 1);
            assert(p->move(SOUTH).find(" died") != string::npos);
        } break; case 12: {
            Arena a(20, 1);
            a.addPlayer(1, 1);
            Player* p = a.player();
            a.addRat(2, 1);
            assert(!p->isDead());
            p->move(SOUTH);
            assert(p->isDead());
        } break; case 13: {
            Arena a(6, 15);
            assert(a.rows() == 6  &&  a.cols() == 15);
        } break; case 14: {
            Arena a(10, 20);
            a.addPlayer(3, 6);
            a.addRat(7, 5);
            assert(a.numberOfRatsAt(7, 5) == 1  &&  a.ratCount() == 1);
        } break; case 15: {
            Arena a(10, 20);
            a.addPlayer(3, 6);
            a.addRat(7, 5);
            a.addRat(4, 7);
            a.addRat(7, 5);
            assert(a.numberOfRatsAt(7, 5) == 2  &&  a.ratCount() == 3);
        } break; case 16: {
            Arena a(1, 20);
            a.addPlayer(1, 8);
            a.addRat(1, 1);
            a.setCellStatus(1, 2, HAS_POISON);
            a.setCellStatus(1, 3, HAS_POISON);
            a.addRat(1, 16);
            a.setCellStatus(1, 14, HAS_POISON);
            a.setCellStatus(1, 15, HAS_POISON);
            a.setCellStatus(1, 17, HAS_POISON);
            a.setCellStatus(1, 18, HAS_POISON);
            assert(a.ratCount() == 2);
            int k;
            for (k = 0; k < 100; k++)
            {
                a.moveRats();
                int nb1 = (a.getCellStatus(1, 2) == HAS_POISON) +
                (a.getCellStatus(1, 3) == HAS_POISON);
                int nb2 = (a.getCellStatus(1, 14) == HAS_POISON) +
                (a.getCellStatus(1, 15) == HAS_POISON) +
                (a.getCellStatus(1, 17) == HAS_POISON) +
                (a.getCellStatus(1, 18) == HAS_POISON);
                assert(a.ratCount() == (nb1 > 0) + (nb2 > 2));
                if (a.ratCount() == 0)
                    break;
            }
            assert(k < 100);
        } break; case 17: {
            Arena a(1, 3);
            a.addPlayer(1, 1);
            Player* p = a.player();
            for (int j = 0; j < 10; j++)
                a.addRat(1, 3);
            assert(!p->isDead());
            a.moveRats();
            int k;
            for (k = 0; k < 100; k++)
            {
                assert(!p->isDead());
                a.moveRats();
                if (a.numberOfRatsAt(1, 1) > 0)
                {
                    assert(p->isDead());
                    break;
                }
            }
            assert(k < 100);
        } break; case 18: {
            ostringstream oss;
            streambuf_switcher sso2(cout, oss);
            Arena a(2, 3);
            a.addPlayer(2, 3);
            a.addRat(2, 1);
            a.addRat(2, 1);
            a.addRat(2, 2);
            a.display("");
            assert(oss.str().find("2R@") != string::npos);
        } break; case 19: {
            ostringstream oss;
            streambuf_switcher sso2(cout, oss);
            Arena a(2, 3);
            a.addPlayer(2, 3);
            for (int k = 1; k <= 20; k++)
                a.addRat(2, 1);
            a.display("");
            assert(oss.str().find("9.@") != string::npos);
        } break; case 20: {
            recordaddrs = true;
            int n = addrs.size();
            {
                Arena a(20, 20);
                for (int r = 1; r <= 5; r++)
                    for (int c = 11; c <= 20; c++)
                        a.setCellStatus(r, c, HAS_POISON);
                for (int r = 16; r <= 20; r++)
                    for (int c = 15; c <= 20; c++)
                        a.setCellStatus(r, c, HAS_POISON);
                a.addPlayer(19, 19);
                int n2 = addrs.size();
                a.setCellStatus(3, 13, EMPTY);
                a.setCellStatus(3, 18, EMPTY);
                a.addRat(3, 13);
                a.addRat(3, 18);
                for (int k = 0; k < 4; k++)
                {
                    a.addRat(1, 1);
                    a.addRat(1, 4);
                    a.addRat(4, 1);
                    a.addRat(4, 4);
                }
                assert(addrs.size() >= n2 + 18);
                a.moveRats();
                a.setCellStatus(3, 13, HAS_POISON);
                a.setCellStatus(3, 18, HAS_POISON);
                a.moveRats();
                a.moveRats();
                assert(a.ratCount() == 18-2);
                for (int k = a.ratCount(); k < MAXRATS; k++)
                    assert(a.addRat(1, 1));
                int j;
                for (j = 0; j < 1000  &&  a.ratCount() > 20; j++)
                {
                    for (int r = 1; r <= 20; r++)
                        for (int c = 1; c <= 20; c++)
                            if (a.numberOfRatsAt(r, c) == 0  &&  !(r == 19 && c == 19))
                                a.setCellStatus(r, c, HAS_POISON);
                    a.moveRats();
                }
                assert(j < 1000);
                for (int k = a.ratCount(); k < MAXRATS; k++)
                    assert(a.addRat(1, 1));
                assert(addrs.size() >= n2 + MAXRATS);
            }
            assert(addrs.size() == n);
            recordaddrs = false;
        } break; case 21: {
            Arena a(20, 20);
            a.addPlayer(19, 19);
            for (int r = 1; r < 19; r++)
                for (int c = 1; c < 19; c++)
                    if (r != 10  |  c != 10)
                        a.setCellStatus(r, c, HAS_POISON);
            for (int k = 0; k < 100; k++)
                a.addRat(10, 10);
            assert(a.ratCount() == 100  &&  a.numberOfRatsAt(10, 10) == 100);
            int nr[1+20][1+20];
            a.moveRats();
            int tot = 0;
            for (int r = 1; r < 19; r++)
            {
                for (int c = 1; c < 19; c++)
                {
                    nr[r][c] = a.numberOfRatsAt(r, c);
                    tot += nr[r][c];
                    assert((r == 10  &&  c >= 9 && c <= 11) ||
                           (c == 10  &&  r >= 9 && r <= 11) ||
                           nr[r][c] == 0);
                }
            }
            assert(nr[10][10] == 0  &&  tot == a.ratCount());
            assert(nr[9][10] == 0  ||  a.getCellStatus(9, 10) == EMPTY);
            assert(nr[11][10] == 0  ||  a.getCellStatus(11, 10) == EMPTY);
            assert(nr[10][9] == 0  ||  a.getCellStatus(10, 9) == EMPTY);
            assert(nr[10][11] == 0  ||  a.getCellStatus(10, 11) == EMPTY);
            a.setCellStatus(10, 10, HAS_POISON);
            a.moveRats();
            assert(a.numberOfRatsAt(9, 10) == (nr[9][10] == 0 ? 0 : 1));
            assert(a.numberOfRatsAt(11, 10) == (nr[11][10] == 0 ? 0 : 1));
            assert(a.numberOfRatsAt(10, 9) == (nr[10][9] == 0 ? 0 : 1));
            assert(a.numberOfRatsAt(10, 11) == (nr[10][11] == 0 ? 0 : 1));
            for (int k = 0; k < 17; k++)
            {
                for (int r = 1; r < 19; r++)
                    for (int c = 1; c < 19; c++)
                        if (a.numberOfRatsAt(r, c) == 0)
                            a.setCellStatus(r, c, HAS_POISON);
                a.moveRats();
            }
            tot = 0;
            for (int r = 1; r < 19; r++)
                for (int c = 1; c < 19; c++)
                    tot += a.numberOfRatsAt(r, c);
            assert(a.ratCount() == tot  &&  tot < 100);
        } break; case 22: {
            Arena a(4, 2);
            a.addPlayer(1, 1);
            Player* p = a.player();
            a.addRat(4, 2);
            for (int k = 0; k < 10000 &&  ! a.player()->isDead()  &&
                 a.ratCount() != 0; k++)
            {
                int dir;
                if (recommendMove(a, p->row(), p->col(), dir))
                    p->move(dir);
                else
                    p->dropPoisonPellet();
                a.moveRats();
            }
            assert(! a.player()->isDead()  &&  a.ratCount() == 0);
        } break; case 23: {
            Arena a(10, 10);
            a.addPlayer(6, 6);
            a.addRat(5, 6);
            a.addRat(7, 6);
            a.addRat(6, 7);
            int dir;
            assert(recommendMove(a, 6, 6, dir)  &&  dir == WEST);
        } break; case 24: {
            Arena a(10, 10);
            a.addPlayer(6, 6);
            a.addRat(4, 6);
            a.addRat(5, 7);
            a.addRat(6, 8);
            a.addRat(7, 7);
            a.addRat(8, 6);
            a.addRat(7, 5);
            a.addRat(6, 4);
            a.addRat(5, 5);
            int dir;
            assert(!recommendMove(a, 6, 6, dir));
        } break; case 25: {
            Arena a(2, 3);
            a.addPlayer(1, 2);
            a.addRat(1, 1);
            for (int k = 0; k < 10; k++)
                a.addRat(2, 3);
            int dir;
            assert(!recommendMove(a, 1, 2, dir));
        } break; case 26: {
            Arena a(3, 2);
            a.addPlayer(3, 1);
            a.addRat(1, 1);
            for (int k = 0; k < 10; k++)
                a.addRat(3, 2);
            int dir;
            assert(recommendMove(a, 3, 1, dir)  &&  dir == NORTH);
        } break;
    }
}

int main()
{
    cout << "Enter test number (1-26): ";
    int n;
    cin >> n;
    testone(n);
    cout << "Passed!" << endl;
}
