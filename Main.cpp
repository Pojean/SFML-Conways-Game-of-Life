#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "random_mt.h"

//i'm making a namespace to contain often used variables across the functions that would bothersome to pass by value
//i'm not making them const unless i need to in case i want to make it a manipulable size later
namespace Var
{

    int windowHeight{ 1000 };
    int windowWidth{ 800 };
    int cellHeight{ 10 };

    sf::Vector2i screenDimensions(windowHeight, windowWidth); //screen dimensions
    sf::Vector2i cellSize(cellHeight, cellHeight); //cell size in pixels
    
  
    int rows = screenDimensions.y / cellSize.y; //rows is equal to screensize / cellsize (800/10)
    int columns = screenDimensions.x / cellSize.x; //columns is equal to screensize / cellsize (1000/10)

    int minNeighbours{2};
    int maxNeighbours{3};
    int exactNeighbours{ 3 };
}

void initializeGrid(std::vector<std::vector<sf::RectangleShape>>& grid)
{
    //this functions initialises the entire grid of rectangles, giving them a value through the passby reference parameter
    //it assigns a size, position, and colour to the individual grid coordinate
    for (int i = 0; i < Var::rows; ++i) {
        for (int j = 0; j < Var::columns; ++j) {
           
            grid[i][j].setSize(sf::Vector2f(Var::cellSize.x - 1, Var::cellSize.y - 1)); //assigns a size to each object in the grid
            grid[i][j].setPosition(j * Var::cellSize.x, i * Var::cellSize.y); //assigns position to object based on iteration through cells
        }
    }
}

void initialiseCells(std::vector<std::vector<int>>& cellStates)
{
    for (int i = 0; i < Var::rows; ++i) { 
        for (int j = 0; j < Var::columns; ++j) {
            cellStates[i][j] = 0;
        }
    }
}

void drawGrid(sf::RenderWindow& window , std::vector<std::vector<sf::RectangleShape>>& grid, std::vector<std::vector<int>>& cellStates)
{
    //this function draws the grid, it takes the window and grid as reference parameters and then draws the rectangles in each grid coordinate
    //the grids were initialised with a colour, position, and size in initialiseGrid();
    //first i have to initialise the grid, then i have to draw it seperately.
    for (int i = 0; i < Var::rows; ++i) {
        for (int j = 0; j < Var::columns; ++j) {
            window.draw(grid[i][j]);
            if (cellStates[i][j] == 0) { grid[i][j].setFillColor(sf::Color::White); }
            else { grid[i][j].setFillColor(sf::Color::Magenta); }
        }
    }
}

int countLiveNeighbours(const std::vector<std::vector<int>>& cellStates, int row, int col)
{
    //variables containing amount of live neighbours and number of rows/columns
    int liveNeighbours{ 0 };
    int numRows = static_cast<int>(cellStates.size()); //we static cast because vectors have size_t type
    int numCols = static_cast<int>(cellStates[0].size());

    //check the status of each of the sorrounding cells (there's 8)
    for (int i{ -1 }; i <= 1; ++i) {
        for (int j{ -1 }; j <= 1; ++j) {
            if (i == 0 && j == 0) continue; //skip the cell that is being checked

            int newRow = row + i;
            int newCol = col + j;

            //check if the neighbouring cell is within bounds and alive
            if (newRow >= 0 && newRow < numRows && newCol >= 0 && newCol < numCols) {
                liveNeighbours += cellStates[newRow][newCol];
            }

            //this loop is a bit wrinkly. i and j loops through -1 to 1
            // and can be represented like this:
            // 
            // (i-1, j-1), (i-1, j), (i-1, j+1)
            // (i, j-1),			   (i, j+1)
            // (i+1, j-1), (i+1, j), (i+1, j+1)
            //
            //As we can see, i increments upwards in the y axis (inverted though)
            //and j increments upwards in the x-axis (not inverted)
            //using if (i == 0 && j == 0) continue; we skip the loop that checks
            //the center cell. the loop counts through the cells, starting at
            //(-1,-1) it then puts that into the newRow and newCol
            //then the following if statement checks if newRow is larger or equal
            //to zero and if that is higher than the number of rows in the board
            //and the same for columns in this one if statement. 
            //if those are all true, then we access the value at that cell, and add it
            //to liveNeighbours, so if it's alive (1) it adds 1 and if it's dead it adds 0
            //so that we end up with a count of how many live neighbours there are.
        }
    }

    return liveNeighbours; //at the end we return liveNeighbours for use in updateBoard()
}

void updateCellStates(std::vector<std::vector<int>>& cellStates)
{
    //variables to initialize a new board which we will iterate over, and then
    //copy into original board and then repeat this in sequence over and over.
    //the syntax board[0].size(); is the same as board.size(); but for the inner vector
    int rows = cellStates.size();
    int cols = cellStates[0].size();

    std::vector<std::vector<int>> nextCellStates = cellStates; //create new board initialized as original board

    for (int i{ 0 }; i < rows; ++i) {
        for (int j{ 0 }; j < cols; ++j) {
            int liveNeighbours = countLiveNeighbours(cellStates, i, j);

            //now we implement the rules of the game of life, which is:
            //if a live cell has 2-3 neighbours, it stay alive
            //if a live cell has fewer than 2 or more than 3 live neighbours, it dies
            //a dead cell with exactly 3 live neighbours becomes alive
            //any other cell remains in its current state

            if (cellStates[i][j] == 1) { //if cell is alive
                if (liveNeighbours < Var::minNeighbours || liveNeighbours > Var::maxNeighbours) {
                    nextCellStates[i][j] = 0; // dies due to over or underpopulation
                }
            }
            else { //cell is dead
                if (liveNeighbours == Var::exactNeighbours) {
                    nextCellStates[i][j] = 1; // comes to life by reproduction
                }
            }
        }
    }

    //update the original board to the new state (we passed the board as reference)
    cellStates = nextCellStates;
}

void randomSpread(std::vector<std::vector<int>>& cellStates)
{
    //this function uses random_mh to get a random value between 0 and 1 and assign it to each cell
    for (int i = 0; i < Var::rows; ++i) {
        for (int j = 0; j < Var::columns; ++j) {
            cellStates[i][j] = Random::get(0, 1);
        }
    }

}

void paintColumn(sf::Vector2i& mousePos, std::vector<std::vector<int>>& cellStates)
{
    int col{ mousePos.x / Var::cellSize.x };

    if (col > 0 && col < Var::columns) {
        for (int i = 0; i < Var::rows; ++i) {

        cellStates[i][col] = 1;
        }
    }
}

void paintRow(sf::Vector2i& mousePos, std::vector<std::vector<int>>& cellStates)
{
    int row{ mousePos.y / Var::cellSize.y };

    if (row > 0 && row < Var::rows) {
        for (int i = 0; i < Var::columns; ++i) {

            cellStates[row][i] = 1;
        }
    }
}

int setFrameRateLimit()
{
    int input{};
    std::cout << "Enter new framerate limit: ";
    std::cin >> input;

    return input;
}

void adjustGridSize(std::vector<std::vector<sf::RectangleShape>>& grid, std::vector<std::vector<int>>& cellstates)
{
    int newCellSize{};
    std::cout << "Enter new square size in pixels (number divisible by 10): ";
    std::cin >> newCellSize;

    Var::cellSize = sf::Vector2i(newCellSize, newCellSize);
    
    Var::rows = Var::screenDimensions.y / Var::cellSize.y;
    Var::columns = Var::screenDimensions.x / Var::cellSize.x;
 
   
    grid.clear();
    cellstates.clear();
    grid.resize(Var::rows, std::vector<sf::RectangleShape>(Var::columns));
    cellstates.resize(Var::rows, std::vector<int>(Var::columns));

   
}

int main() {
    //this is the syntax to initiate a window object, it has be be RenderWindow if anything needs to be drawn in it
    //VideoMode determines the pixel size in x, y amounts where x is width and y is height
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Conway's Game of Life");

    //here i create my grid object, it is a 2d vector of type vector/rectangleshape which becomes grid[rows][columns]
    std::vector<std::vector<sf::RectangleShape>> grid(Var::rows, std::vector<sf::RectangleShape>(Var::columns));
    // outvector <| inner vector <type>|outvector> objectname
    //this creates a grid of rectangle shapes that i can draw inside the loop.

    //to manage cell states i make a grid of int values i can overlay on the grid of rectangles, this will control on/off behaviour of each cell
    std::vector<std::vector<int>> cellStates(Var::rows, std::vector<int>(Var::columns));
    //using the same rows and columns ensures there's the same amount of indices in the vectors for grid and cellStates

    initializeGrid(grid); // initialises the grid 
    initialiseCells(cellStates); // initialices the cellStates grid 
    bool isPaused{ true }; //used to store the pause state to be detected by event qeue
    window.setFramerateLimit(60);

    std::cout << "Keybinds: \n \n" <<
        "P: pause and unpause the simulation \n" <<
        "Z: change framerate limit \n" <<
        "X: resize squares \n" << 
        "Left click to turn on cell \n" <<
        "R: reset the grid \n" <<
        "F1: generate random noise pattern \n" <<
        "Hold right click to draw with cells \n" <<
        "At cursor: \n" <<
        "Press middle mouse button to pain column \n" <<
        "Press Shift+middle mouse button to paint row \n" <<
        "Press Alt+middle mouse button to paint column and row" <<
        '\n';

    std::cout << '\n' <<
        "Rules: If a cell has less than 2 or more than 3 neighbours, it dies. \n" <<
        "If it has exactly 3 neighbours and isn't alive, it comes alive. \n";
                 

    while (window.isOpen()) {
        //this is the program loop, it runs so long as the window is open, which means that it's a convenient way to run the program


        sf::Event event; // this is the Event object, it recieves events in a qeue (i think) or it recieves it via assignment

        while (window.pollEvent(event)) { //this loop runs every iteration, but only executes if there's an event waiting to happen

            if (event.type == sf::Event::Closed) // this event closes the window if we catch a Closed event
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
                isPaused = !isPaused;
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {   
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    paintRow(mousePos, cellStates);
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) {
                    paintRow(mousePos, cellStates);
                    paintColumn(mousePos, cellStates);
                }
                else { paintColumn(mousePos, cellStates); }

               
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X) { adjustGridSize(grid, cellStates);   
            initializeGrid(grid);
            initialiseCells(cellStates);
            }
           
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) { window.setFramerateLimit(setFrameRateLimit()); }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                //this checks if the event in the qeue is mouseButtonPressed and if that MouseButtonPressed is mouse left
                //now i get the mouse position and convert it to grid coordinates
                sf::Vector2i mousePos = sf::Mouse::getPosition(window); //gets mouse position in window in ints, stores in variable
                int col = mousePos.x / Var::cellSize.x;
                int row = mousePos.y / Var::cellSize.y;
                
                //this assigns the position in mousepos to a square by divinding the number with cellsize
                //now we ensure the click is within bounds
                if (row >= 0 && row < Var::rows && col >= 0 && col < Var::columns) {
                    //if it is within bounds, we change that cell to live (true)
                    if (cellStates[row][col] == 0) { cellStates[row][col] = 1; }
                    else { cellStates[row][col] = 0; }

                }
            }

            //clears the board if you press r during runtime (by reinitialising cellStates to 0)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                for (int i = 0; i < Var::rows; ++i) {
                    for (int j = 0; j < Var::columns; ++j) {
                        cellStates[i][j] = 0;
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1) {
                randomSpread(cellStates);
            }

            if (event.MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window); //gets mouse position in window in ints, stores in variable
                int col = mousePos.x / Var::cellSize.x;
                int row = mousePos.y / Var::cellSize.y;
                
                //this assigns the position in mousepos to a square by divinding the number with cellsize
                //now we ensure the click is within bounds
                if (row >= 0 && row < Var::rows && col >= 0 && col < Var::columns) {
                    //if it is within bounds, we change that cell to live (true)
                    cellStates[row][col] =1;
                    

                }

            }
        }

   

        
        

            window.clear(); // this clears the drawable window

            //all drawing code goes here
            drawGrid(window, grid, cellStates);
            if (!isPaused) { // makes it so that the drawing only happens if the simulation is not paused
                updateCellStates(cellStates);
            }

            window.display(); // this draws what was put in between this and window.clear
            //each loop the window clears, before drawing the next iteration, and then loops.

         
    }

    return 0;
}