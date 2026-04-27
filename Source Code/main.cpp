#include <SFML/Graphics.hpp>  //Sfml main library for setting up textures and creating window.
#include <SFML/Audio.hpp>     //SFML Library for importing sound to your project and using them.
#include <iostream>           //C++ Library to get user input and show output.
#include <vector>             //C++ Library for setting up array dynamically.
#include <ctime>              //C++ Library to get system time.
#include <cmath>              //C++ Library for using math functions.

enum Difficulty { EASY, NORMAL, HARD }; // a datatype that only force user to select from fixed value
                                        // here we use it for applying game difficulties Easy, Medium, Hard.

const int TILE_SIZE = 32;  //Applying a fixed size for texture
bool gameOver = false;     //Checking Gameover Statues
bool gameStarted = false;  //Checking Gamestart Status

struct Tile {                   //Creating a structure Tile the check if the unrevealed tile is
    bool isMine = false;        //A mine
    bool isRevealed = false;    //Is revealed or not
    bool isFlagged = false;     //Did user Flag it or not
    int adjacentMines = 0;      //And assinging 0 to adjacent mines by default
};

std::vector<std::vector<Tile>> board;   //Creating a Dynamic Board that changes according to difficulty
sf::Texture texture;                    //Creating a texture variable
sf::Texture timeTexture;                //Creating a texture of name timeTexture that stores the font and image for time
sf::Texture menuTexture;                //Creating a texture of name menuTexture that stores the image for time Start Options and Exit
sf::Texture difficultyTexture;          //Creating a texture of name difficultyTexture that store the image for Begineer, Intermediate, Expert
sf::Texture backgroundTexture;          //Creating a texture of name backgroundTexture that display a specific tile loaded from image in the game background
sf::Music menuMusic;                    //Use to store the menu music
sf::Music gameMusic;                    //Use to store the game music
sf::SoundBuffer clickBuffer;            //Use to store a click sound made when user selects and option
sf::Sound clickSound;                   //Use to make the click sound stored in clickbuffer
sf::SoundBuffer gameOverBuffer;         //Use to store gameover sound made when user hits a mine
sf::Sound gameOverSound;                //Use to play the gameover sound stored in buffer
sf::Clock gameClock;                    // Clock to measure time elapsed
sf::Text timerText;                     // Text to display the timer
int gridSize = 10;
int numMines = 10;
Difficulty currentDifficulty = EASY;

void initializeBoard();                     //Creating a function that create the game board
void revealTile(int x, int y);              //Function that reveal tile when user selects its
void revealAllTiles();                      //Function that reveal all tiles when user hit a mine
void drawBoard(sf::RenderWindow &window);   //Function that prints the game board that is initialized and sf renderwindow allocates a specific window size
void resetGame();                           //Resets the Game
void setDifficulty(Difficulty diff);        //Set the difficulty of game according to user choice

bool inMenu = true;             //Checking inMenu status
bool inDifficultyMenu = false;  //Checking DifficultyMenu status
bool inOptionsMenu = false;     //Checking OptionMenu Status
bool soundmute = false;         //Checking sound on or off status
int selectedItemIndex = 0;      //assigning 0 to selected item index (takes the control to a specific texture when user switch b/w menus)
sf::RenderWindow window(sf::VideoMode(350, 380), "Minesweeper");    //Creates a default window of size 350,380 that shows Minesweeper on application name
sf::Font font;                  //A texture to store font
std::vector<sf::Text> menuItems;        //Used to store collection of menuItems vector to make it dynamic so user can increase or decrease textures/sprite in the menu
std::vector<sf::Text> difficultyItems;  //used to store collection of difficulty items
std::vector<sf::Sprite> menuSprites;    //used to store collection of menuSprites these contain the the specific texture user want
std::vector<sf::Sprite> difficultySprites;  //used to store collection of difficultySprites
std::vector<sf::Sprite> timeSprites;    //used to store collection of timeSprites

void initializeBoard() {            //Function that initializes the board
    srand(time(0));                 //Used to take system time and avoid repetition of mines placement location
    board.clear();                  //clearing the board after user selects difficulty
    board.resize(gridSize, std::vector<Tile>(gridSize));        //Sets a gridsize according to difficulty selected

    int minesPlaced = 0;                    //assigning 0 t0 minesplaces variable
    while (minesPlaced < numMines) {        //Loops until total required mines are places
        int x = rand() % gridSize;          //Randomly takes a number and take remainder with grid size so that it does not exceed the total grid size
        int y = rand() % gridSize;          
        if (!board[x][y].isMine) {          //if the location does not already contain a mines it increases the MinePlaced size 
            board[x][y].isMine = true;      
            minesPlaced++;                  
        }
    }
}

int countAdjacentMines(int x, int y) {      //Function that calculates total mines a tile has in its surrounding
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {          //looping through rows
        for (int dy = -1; dy <= 1; dy++) {      //looping through column
            int nx = x + dx, ny = y + dy;       //Checks the surrounding tile
            if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize && board[nx][ny].isMine)
                count++;                        //If there is a mine in surrounding tile it increases the count
        }
    }
    return count;
}

void revealTile(int x, int y) {         //Function that reveal the tile user clicks on or reveals all the tile if user clicks on mines
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize || board[x][y].isRevealed || board[x][y].isFlagged)  //Checks if a tile has been revealed or not
        return;
                     
                board[x][y].isRevealed = true;          //if tile is revealed
                                                        //and it is a mine it call the reveal all tile function and ends the game
                if (board[x][y].isMine) {
                    gameOver = true;
                    revealAllTiles();
                    return;
                }
            
                board[x][y].adjacentMines = countAdjacentMines(x, y);  //counts the surrounding mine of the revealed tile and places a number = to total mines in surrounding
            
                if (board[x][y].adjacentMines == 0) {       // if there is no mines on the reveal tile it continues the game
                for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                   revealTile(x + dx, y + dy);
            }
        }
    }
}

void revealAllTiles() {             //Function that stops game execution after mines is hit and displays all the mines in the board
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            board[i][j].isRevealed = true;
        }
    }
}

void drawBoard(sf::RenderWindow &window) {              //Function that drawsa a window according to user given size
    sf::Sprite sprite(texture);                 //Creating a sprite to store texture
    if(gridSize==7){                    // if user selects easy difficulty then all mines are started from 5th row and 3rd column 
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                int tileIndex = board[i][j].isRevealed ? (board[i][j].isMine ? 9 : board[i][j].adjacentMines) // assigns the tile texture
                    : (board[i][j].isFlagged ? 12 : 11); // Set to 12 for flagged and 11 for unrevealed
                sprite.setTextureRect(sf::IntRect(sf::Vector2i(tileIndex * TILE_SIZE, 0), sf::Vector2i(TILE_SIZE, TILE_SIZE)));
                sprite.setPosition(sf::Vector2f((i+2) * TILE_SIZE, (j+4) * TILE_SIZE));  // sets location where to print the sprite for tile
            window.draw(sprite);        //Draws the sprite
        }
    }
    } else{                                     //Does the Same thing but starts printing tile from 3rd row and 2nd Column
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            int tileIndex = board[i][j].isRevealed ? (board[i][j].isMine ? 9 : board[i][j].adjacentMines)
                : (board[i][j].isFlagged ? 12 : 11); // Set to 12 for flagged and 11 for unrevealed
            sprite.setTextureRect(sf::IntRect(sf::Vector2i(tileIndex * TILE_SIZE, 0), sf::Vector2i(TILE_SIZE, TILE_SIZE)));
            sprite.setPosition(sf::Vector2f((i+1) * TILE_SIZE, (j+2) * TILE_SIZE));
            window.draw(sprite);
        }
    }
}
}

void resetGame() {      //Resets game after user hits a mine and presses enter
    gameOver = false;
    gameStarted = false;
    initializeBoard();
}

void setDifficulty(Difficulty diff) {               //Function that assigns grid size and total mines according to difficulty level
    currentDifficulty = diff;
    switch (diff) {
        case EASY:
            gridSize = 7;
            numMines = 10;
            break;
        case NORMAL:
            gridSize = 9;
            numMines = 15;
            break;
        case HARD:
            gridSize = 9;
            numMines = 30;
            break;
    }
    resetGame();            //resets game clock to 0:00
    gameClock.restart();
}

void stopAllMusic() {       //Stops all music when user disable from setting
    if (menuMusic.getStatus() == sf::Music::Playing) {
        menuMusic.stop();
    }
    if (gameMusic.getStatus() == sf::Music::Playing) {
        gameMusic.stop();
    }
}

void playGameMusic() {         //Plays music again after user enable from setting
    if (!soundmute && gameMusic.getStatus() != sf::Music::Playing) {
        gameMusic.play();
    }
}

void playMenuMusic() {      //Plays the assigned menu music
    if (!soundmute && menuMusic.getStatus() != sf::Music::Playing) {
        menuMusic.play();
    }
}

void playClickSound() {         //Makes a click sound loaded from files
    if (!soundmute) {
        clickSound.play();
    }
}

void playGameOverSound() {      //plays game over sund when mine is hit
    if (!soundmute) {
        gameOverSound.play();
    }
}

void toggleMute() {             //Function that handles the enabling and disabling of sound and music
    soundmute = !soundmute;
    if (soundmute) {
        // Stop all sounds when muted
        stopAllMusic();
        
    } else {
        // Play the music or sounds based on current state
        if (inMenu) {
            playMenuMusic();
        } else if (gameStarted) {
            playGameMusic();
        }
        playClickSound();
    }
}


int main() {

    if (!texture.loadFromFile("Assets\\images\\tile.png")) {   //Loads the tile.png in computer to !texture
        std::cerr << "Error loading tileset!" << std::endl;
        return -1;
    }

    if (!menuTexture.loadFromFile("Assets\\images\\optionbuttons.png")) { // Load menu texture
        std::cerr << "Error loading menu texture!" << std::endl;
        return -1;
    }

    if (!timeTexture.loadFromFile("Assets\\images\\optionbuttons.png")) { // Load menu texture
        std::cerr << "Error loading menu texture!" << std::endl;
        return -1;
    }

    if (!difficultyTexture.loadFromFile("Assets\\images\\optionbuttons.png")) { // Load difficulty texture
        std::cerr << "Error loading difficulty texture!" << std::endl;
        return -1;
    }

     if (!backgroundTexture.loadFromFile("Assets\\images\\tile.png")) { //Load Background texture
        std::cerr << "Error loading tileset!" << std::endl;
        return -1;
    }

    if (!font.loadFromFile("Assets\\fonts\\font.ttf")) {    //Loads Font will only be using to display time
        std::cerr << "Error loading font!" << std::endl;
        return -1;
    }

    sf::Texture logoTexture;
    if (!logoTexture.loadFromFile("Assets\\images\\logo.png")) {    //Loads the logo that is displayed when user starts the game
    std::cerr << "Error loading logo texture!" << std::endl;
    return -1;
    }

    if (!menuMusic.openFromFile("Assets\\sounds\\mainmenu.ogg")) { // Load main menu music
        std::cerr << "Error loading main menu music!" << std::endl;
        return -1;
    }

    if (!gameMusic.openFromFile("Assets\\sounds\\music.ogg")) { // Load game music
        std::cerr << "Error loading game music!" << std::endl;
        return -1;
    }

    if (!clickBuffer.loadFromFile("Assets\\sounds\\click.wav")) { // Load click sound
        std::cerr << "Error loading click sound!" << std::endl;
        return -1;
    }
    clickSound.setBuffer(clickBuffer);

    if (!gameOverBuffer.loadFromFile("Assets\\sounds\\gameOver.wav")) { // Load game over sound
        std::cerr << "Error loading game over sound!" << std::endl;
        return -1;
    }
    gameOverSound.setBuffer(gameOverBuffer);

    timerText.setFont(font);          //using font for text
    timerText.setCharacterSize(24);  // Set the character size
    timerText.setFillColor(sf::Color::White);   //Setting the color to white

    sf::Sprite logoSprite(logoTexture); //loads logoTexture to sprite
    sf::Clock clock;        //using clock to give an effect to logo
    float scaleFactor = 1.0f; //changing the logo effect speed
    bool increasing = true;
    logoSprite.setPosition(125.f, 30.f); //setting a position for logo

    sf::Sprite backButton(menuTexture); //loading menutexture into backbutton
    backButton.setTextureRect(sf::IntRect(335, 96, TILE_SIZE * 4, TILE_SIZE)); //telling which texture to load from menuTexture
    backButton.setPosition(128.f, 224.f);   //setting a fixed location for it to display

    sf::Sprite credit(menuTexture);     //setting texture for credit
    credit.setTextureRect(sf::IntRect(1130, 96, TILE_SIZE * 9, TILE_SIZE));
    credit.setPosition(31.f, 288.f);

    //Sound Mute
    sf::Sprite muteButton(menuTexture); // Mute button on
    muteButton.setTextureRect(sf::IntRect(816,0,TILE_SIZE,TILE_SIZE));
    muteButton.setPosition(128.f, 160.f);

    sf::Sprite muteButtonoff(menuTexture); // Mute button off
    muteButtonoff.setTextureRect(sf::IntRect(880,0,TILE_SIZE,TILE_SIZE));
    muteButtonoff.setPosition(128.f, 160.f);

    // Menu sprites (Play, Options, Exit And Time)
    sf::Sprite playSprite(menuTexture);  //Loads texture for play
    playSprite.setTextureRect(sf::IntRect(1120, 64, TILE_SIZE * 5, TILE_SIZE));
    playSprite.setPosition(125.f, 160.f);
    menuSprites.push_back(playSprite);

    sf::Sprite Time(timeTexture); //load texture for time
    Time.setTextureRect(sf::IntRect(1120,128,32*5,32));
    Time.setPosition(200.f, 32.f);
    timeSprites.push_back(Time);

    sf::Sprite optionsSprite(menuTexture); //load texture for option
    optionsSprite.setTextureRect(sf::IntRect(1120, 0, TILE_SIZE * 1, TILE_SIZE));
    optionsSprite.setPosition(350.f, 1.f);
    menuSprites.push_back(optionsSprite);

    sf::Sprite exitSprite(menuTexture);     //load texture for exit
    exitSprite.setTextureRect(sf::IntRect(800, 128, TILE_SIZE * 5, TILE_SIZE));
    exitSprite.setPosition(125.f, 255.f);
    menuSprites.push_back(exitSprite);

    // Difficulty sprites (Beginner, Intermediate, Expert)
    sf::Sprite beginnerSprite(difficultyTexture);
    beginnerSprite.setTextureRect(sf::IntRect(160, 32, TILE_SIZE * 5, TILE_SIZE));
    beginnerSprite.setPosition(125.f, 160);
    difficultySprites.push_back(beginnerSprite);

    sf::Sprite intermediateSprite(difficultyTexture);
    intermediateSprite.setTextureRect(sf::IntRect(480, 32, TILE_SIZE * 5, TILE_SIZE));
    intermediateSprite.setPosition(125.f, 225);
    difficultySprites.push_back(intermediateSprite);

    sf::Sprite expertSprite(difficultyTexture);
    expertSprite.setTextureRect(sf::IntRect(800, 32, TILE_SIZE * 5, TILE_SIZE));
    expertSprite.setPosition(125.f, 288);
    difficultySprites.push_back(expertSprite);

    // Creating a background sprite
    sf::Sprite backgroundSprite(backgroundTexture);
    sf::Vector2u textureSize = backgroundTexture.getSize();

    while (window.isOpen()) {   //Setting up loop for game


    float time = clock.getElapsedTime().asSeconds(); //getting time for logo
    scaleFactor = 1.0f + 0.1f * sin(time * 1.0f);  //using sin from ctime and telling it how much logo size changes by time
    logoSprite.setScale(scaleFactor, scaleFactor);
    sf::FloatRect logoBounds = logoSprite.getLocalBounds();
    logoSprite.setOrigin(logoBounds.width / 2, logoBounds.height / 2);
    logoSprite.setPosition(170.f, 80.f);  //setting a location for Logo


        sf::Event event;  //Game Loop
        while (window.pollEvent(event)) {  //Setting events for Keys
            if (event.type == sf::Event::Closed) { //if event is closed
                window.close();                    //closes the Window/Game
                } else if (event.type == sf::Event::KeyPressed) {  // If User presses a key then checks following condition


                    if (inMenu) {   //Firsts check where it is currently inMenu , Difficulty or option
                    playMenuMusic(); //starts playing the menu music
                    if (event.key.code == sf::Keyboard::Up)  //if user press up arrow key it switches highlighted texture by 1
                        selectedItemIndex = (selectedItemIndex - 1 + menuSprites.size()) % menuSprites.size();
                        else if (event.key.code == sf::Keyboard::Down) //if user press down arrow key it switches highlighted texture by 1
                            selectedItemIndex = (selectedItemIndex + 1) % menuSprites.size();
                                else if (event.key.code == sf::Keyboard::Enter) {  //If user presses enter then checks following condition

                                    if (selectedItemIndex == 0) { //If play is selected then
                                        playClickSound();         //Makes a click sound
                                        inMenu = false;           //Changes inMenu to False
                                        selectedItemIndex = 0;
                                        inDifficultyMenu = true;  //Changes inDifficultyMenu to true taking user to difficulty screen
                                        selectedItemIndex = 0;

                                            } else if (selectedItemIndex == 1) { // Options selected
                                                playClickSound(); //Makes click sound
                                                inMenu=false;
                                                inOptionsMenu=true; //Takes user to option screen

                                                    } else if (selectedItemIndex == 2){ //If exit is selected closes the gmae
                                                        window.close();
                                                    }
                                                } 

                                            }else if (inOptionsMenu) {                          // if user is in option menu
                                                if (event.key.code == sf::Keyboard::Escape){    // and presses escape it takes user to previous menu
                                                    inOptionsMenu = false;
                                                    inMenu = true; // Go back to the main menu  
                                                    selectedItemIndex = 0;
                                                }

                                                else if (event.key.code == sf::Keyboard::Enter){ // if user presses enter it mutes or unmutes the game sounds
                                                    soundmute = !soundmute;
                                                        if (soundmute) {
                                                            stopAllMusic();
                                                        } else {
                                                            if (inMenu) {
                                                            playMenuMusic();
                                                        } else if (gameStarted) {
                                                        playGameMusic();
                                                        }
                                                    playClickSound();
                                                }
                                            }

                                        }else if (inDifficultyMenu) {  //Does the same thing as menu selection but user selects difficulty here
                                            if (event.key.code == sf::Keyboard::Up)
                                                selectedItemIndex = (selectedItemIndex - 1 + difficultySprites.size()) % difficultySprites.size();
                                                    else if (event.key.code == sf::Keyboard::Down)
                                                        selectedItemIndex = (selectedItemIndex + 1) % difficultySprites.size();

                                                            else if (event.key.code == sf::Keyboard::Enter) { // creates a window and plays music after difficulty is selected
                                                                playClickSound();
                                                                setDifficulty(static_cast<Difficulty>(selectedItemIndex));
                                                                window.create(sf::VideoMode(350, 380), "Minesweeper");
                                                                inDifficultyMenu = false;
                                                                gameStarted = true;
                                                                stopAllMusic();
                                                                playGameMusic();
                                                            }

                                                    else if(event.key.code == sf::Keyboard::Escape){ // if user presses ESC take him back to previous screen
                                                            inDifficultyMenu = false;
                                                            inMenu = true;
                                                            selectedItemIndex = 0;
                                                        }

                                                    } else if (gameStarted && gameOver) {  //when user hits a mines and presses enter it takes him back to main menu 
                                                        if (event.key.code == sf::Keyboard::Enter) {
                                                            resetGame();
                                                            playClickSound();
                                                            window.create(sf::VideoMode(350, 380), "Minesweeper");
                                                            inMenu = true;
                                                            stopAllMusic();
                                                            playMenuMusic();
                                                        }
                                                    }       

                                            } else if (event.type == sf::Event::MouseButtonPressed && gameStarted && !gameOver) { //if mine is not hit then it reveals the tile
                                                if (event.mouseButton.button == sf::Mouse::Left) {
                                                    int x,y;
                                                    if(gridSize== 7){
                                                        x = (event.mouseButton.x - 2 * TILE_SIZE) / TILE_SIZE;
                                                        y = (event.mouseButton.y - 4 * TILE_SIZE) / TILE_SIZE;
                                                    }
                                                    else {
                                                        x = (event.mouseButton.x - TILE_SIZE) / TILE_SIZE;
                                                        y = (event.mouseButton.y - 2 * TILE_SIZE) / TILE_SIZE;
                                                    }
                                
                                                    if (x >= 0 && x < gridSize && y >= 0 && y < gridSize) { 
                                                        revealTile(x, y); // reveals the tile by calling the function

                                                         if (gameOver) { // when user hits a mine it stops all music plays game oversound and takes user back to main menu
                                                            stopAllMusic();
                                                            playGameOverSound();
                                                            window.display();
                                
                                                         }
                                                    }
                                                }

                                        else if (event.type == sf::Event::MouseButtonPressed && gameStarted && !gameOver) {  //Does the same thing but for Intermediate and Expert Difficulty
                                            if (event.mouseButton.button == sf::Mouse::Left) {                               //Made different for Easy difficulty because of change in Grid Size
                                                } else if (event.mouseButton.button == sf::Mouse::Right) {

                                                int x, y;
                                                if (gridSize == 7) {
                                                    x = (event.mouseButton.x - 2 * TILE_SIZE) / TILE_SIZE;
                                                    y = (event.mouseButton.y - 4 * TILE_SIZE) / TILE_SIZE;
                                                } else {
                                                    x = (event.mouseButton.x - TILE_SIZE) / TILE_SIZE;
                                                    y = (event.mouseButton.y - 2 * TILE_SIZE) / TILE_SIZE;
                                                }
                                                if (x >= 0 && x < gridSize && y >= 0 && y < gridSize) {
                                                    // Flag the tile
                                                    board[x][y].isFlagged = !board[x][y].isFlagged;
                                                }
                                            }
                                        }
                                    }
                                }


        window.clear(sf::Color::Black); //Sets a black Background
        
        sf::IntRect backgroundRect(512, 0, TILE_SIZE, TILE_SIZE); //after black background is set this overlaps the background with a texture of user choice
        backgroundSprite.setTextureRect(backgroundRect);

        for (unsigned int x = 0; x < window.getSize().x; x += backgroundRect.width) {  //Used to cover the whole background according to window size
        for (unsigned int y = 0; y < window.getSize().y; y += backgroundRect.height) {
        backgroundSprite.setPosition(static_cast<float>(x), static_cast<float>(y));
        window.draw(backgroundSprite); //Draws the background Texture
        }
     }
 
 if (gameStarted && !gameOver) {
     float elapsedTime = gameClock.getElapsedTime().asSeconds();  // Get elapsed time in seconds
 
     // Convert elapsed time to minutes and seconds
     int minutes = static_cast<int>(elapsedTime) / 60;
     int seconds = static_cast<int>(elapsedTime) % 60;
 
     // Format the timer text (e.g., "Time: 01:23")
     timerText.setString("" + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds));
     
     // Set the position and draw the timer
     timerText.setFont(font);
     timerText.setCharacterSize(24);
     timerText.setFillColor(sf::Color::White); //Sets the color
     timerText.setPosition(280.f, 32.f);  // Position at the top left corner
     window.draw(Time);
     window.draw(timerText);  // Draw the timer on the window
 }
 
 if (inMenu) {  // A loop that changes a sprite texture when it is highlighted
     window.draw(logoSprite);
     for (size_t i = 0; i < menuSprites.size(); i++) {
         window.draw(menuSprites[i]);
         // Reset to default texture first
         if (i == 0) {                          //For Start Texture
             menuSprites[i].setTexture(menuTexture);
             menuSprites[i].setTextureRect(sf::IntRect(1120, 64, TILE_SIZE * 5, TILE_SIZE));
             menuSprites[i].setPosition(95.f, 160.f);
         } else if (i == 2) {           //For Exit Texture
             menuSprites[i].setTexture(menuTexture);
             menuSprites[i].setTextureRect(sf::IntRect(1120, 0, TILE_SIZE * 1, TILE_SIZE));
             menuSprites[i].setPosition(318.f, 1.f);
         } else if (i == 1) {           //For Option Texture
             menuSprites[i].setTexture(menuTexture);
             menuSprites[i].setTextureRect(sf::IntRect(800, 128, TILE_SIZE * 5, TILE_SIZE));
             menuSprites[i].setPosition(95.f, 257.f);
         }
 
         // Apply highlight if selected
         if (i == selectedItemIndex) {  //Changes Texture after it is highlighted
             if (i == 0) {
                 menuSprites[i].setTexture(menuTexture);
                 menuSprites[i].setTextureRect(sf::IntRect(1280, 64, TILE_SIZE * 5, TILE_SIZE));
             } else if (i == 2) {
                 menuSprites[i].setTexture(menuTexture);
                 menuSprites[i].setTextureRect(sf::IntRect(1152, 0, TILE_SIZE * 1, TILE_SIZE));
             } else if (i == 1) {
                 menuSprites[i].setTexture(menuTexture);
                 menuSprites[i].setTextureRect(sf::IntRect(960, 128, TILE_SIZE * 5, TILE_SIZE));
             }
         }
         
     }
 }
 
 else if (inOptionsMenu) {  //loads the texture when in option menu
     window.draw(backButton);
     window.draw(credit);
     window.draw(logoSprite);
     if (soundmute) {
         window.draw(muteButtonoff);  // Show muted state
     } else {
         window.draw(muteButton);     // Show unmuted state
     }
 }
 
  else if (inDifficultyMenu) {          //Loads the difficulty texture and changes them when highlighted
     window.draw(logoSprite);
     for (size_t i = 0; i < difficultySprites.size(); i++) {
         window.draw(difficultySprites[i]);
         if (i == 0) {
             difficultySprites[i].setTexture(difficultyTexture);
             difficultySprites[i].setTextureRect(sf::IntRect(160, 32, TILE_SIZE * 5, TILE_SIZE));
             difficultySprites[i].setPosition(95.f, 160.f);
         } else if (i == 1) {
             difficultySprites[i].setTexture(difficultyTexture);
             difficultySprites[i].setTextureRect(sf::IntRect(480, 32, TILE_SIZE * 5, TILE_SIZE));
             difficultySprites[i].setPosition(95.f, 225.f);
         } else if (i == 2) {
             difficultySprites[i].setTexture(difficultyTexture);
             difficultySprites[i].setTextureRect(sf::IntRect(800, 32, TILE_SIZE * 5, TILE_SIZE));
             difficultySprites[i].setPosition(95.f, 288.f);
         }
 
         if (i == selectedItemIndex) {  //The highlighted texture are loaded from here
             if (i == 0) {
                 difficultySprites[i].setTexture(difficultyTexture);
                 difficultySprites[i].setTextureRect(sf::IntRect(320, 32, TILE_SIZE * 5, TILE_SIZE));
                 difficultySprites[i].setPosition(95.f, 160.f);
             } else if (i == 1) {
                 difficultySprites[i].setTexture(difficultyTexture);
                 difficultySprites[i].setTextureRect(sf::IntRect(640, 32, TILE_SIZE * 5, TILE_SIZE));
                 difficultySprites[i].setPosition(95.f, 225.f);
             } else if (i == 2) {
                 difficultySprites[i].setTexture(difficultyTexture);
                 difficultySprites[i].setTextureRect(sf::IntRect(960, 32, TILE_SIZE * 5, TILE_SIZE));
                 difficultySprites[i].setPosition(95.f, 288.f);
             }
         }
                 window.draw(difficultySprites[i]);
     }
 }
 else {             //Draws the next board selected
     drawBoard(window);
 }
 window.display();  //Displays the window
     }
    return 0;
}