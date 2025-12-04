#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;

//This function is used for sucking ghost and skeletons
//player_x/player_y represents position of player top orleft
//PlayerWidth/PlayerHeight shows player's sprite size
//ghost_x[], ghost_y[], ghost_active[], ghosts shows ghost positions + active flags + count
//skel_x[], skel_y[], skel_active[], skel is skeletons same as ghosts
//cell_size indicates tile size to compute centers
//suck_strength shows how fast the enemies are pulled
//vacuumDirection indiactes 0=left,1=up,2=right,3=down
void handle_vacuum_sucking(
    float& player_x, float& player_y, int& PlayerWidth, int& PlayerHeight,
    float ghost_x[], float ghost_y[], bool ghost_active[], int ghosts,
    float skel_x[], float skel_y[], bool skel_active[], int skel,
    const int cell_size, const float suck_strength, int vacuumDirection,
    int& enemiesSucked, const int maxCapacity)

{
    //this shows the maximum distance from which the enemy can be sucked
    const float vacuum_range = 200.0f;
    //this tells the distance that from where the enemy should disappear after coming how much close to player
    const float capture_distance_sq = 10000.0f;
    //this tells the player center
    float player_center_x = player_x + PlayerWidth / 2.0f;
    float player_center_y = player_y + PlayerHeight / 2.0f;
    // Vacuum direction as unit vector
    float dir_x = 0, dir_y = 0;
    switch (vacuumDirection) {
        case 0://tells that the vacuum is facing left and looks for enemies on the left
            dir_x = -1; dir_y = 0; break;
        case 1:// tells that the vacuum is faciing up and looks for enemies on up position
            dir_x = 0; dir_y = -1; break;
        case 2://tell that the vacuum is facing rightwards and same lokks for enemies on right
            dir_x = 1; dir_y = 0; break;
        case 3://check for enemies in downward direction
            dir_x = 0; dir_y = 1; break;
    }
    //Ghost sucking mechnism
    for (int i = 0; i < ghosts; i++) {
        //this check if ghost is currently active or alive in the game if not it continues and check the next ghost
        if (!ghost_active[i]) continue;
        //next we compute ghost sprite center because vacuum pulss ghost towards center
        float ghost_center_x = ghost_x[i] + cell_size / 2.0f;
        float ghost_center_y = ghost_y[i] + cell_size / 2.0f;
        //this shoss the distance between ghost and player on x-axis
        float dx = ghost_center_x - player_center_x;
        //this shows the distance between ghost and payer on y-axis
        float dy = ghost_center_y - player_center_y;
        //using pythagorous theorum to find the distance where dx=horizontal distance and dy=vertical distance
        float distance_sq = dx * dx + dy * dy;
        //this shows if ghost range is far then the vacuum range then its ignored and if ghost are on top of player they are also ignored
        if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f) {
            // next taking the squareroot to find the distance again this come from pythagerous theorum
            float distance = sqrt(distance_sq);
            //direction from player to ghost in unit vector form we need this becaoue later we need it to ensure ghost move in correct direction and in correct speed
            float to_ghost_x = dx / distance;
            float to_ghost_y = dy / distance;
            //this is the dot product formula it tells how much ghost is infornt of the camera so only ghost in vacuum direction are pulled
            float dot = to_ghost_x * dir_x + to_ghost_y * dir_y;
            //checks if ghost is infront of the vacuum
            if (dot > 0.5f) {
                //if the ghost is too close then capture the ghost means deactivate it
                if (distance_sq < capture_distance_sq) {
                    ghost_active[i] = false;
                    ghost_x[i]= -1000;
                    ghost_y[i]= -1000;
                    continue;//move to next ghost
                }
                // move ghost is not too close but still in vacuum range then pull it using suck_strngth
                ghost_x[i] -= to_ghost_x * suck_strength;
                ghost_y[i] -= to_ghost_y * suck_strength;
            }
        }
    }
    //Skeleton sucking mechanism same logic as ghost one
    for (int i = 0; i < skel; i++) {
        //skips skeleton that are already captured
        if (!skel_active[i]) continue;
        //next compute the center of skeleton sprite to make sure the sucking is directed toward sthe middle of the sprite
        float skel_center_x = skel_x[i] + cell_size / 2.0f;
        float skel_center_y = skel_y[i] + cell_size / 2.0f;
        //find horizontal and vertocal distance from player to the skelton
        float dx = skel_center_x - player_center_x;
        float dy = skel_center_y - player_center_y;
        //find the distance using pythagerous theorum
        float distance_sq = dx * dx + dy * dy;
        //check if the skeleton is within the vacuum range and not above the player
        if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f) {
            //next compute actual distance we didnt find actual distance before just to make more efficiency in game
            float distance = sqrt(distance_sq);
            //next unit vertor direction to tell exactle which direction for skeleton to move
            float to_skel_x = dx / distance;
            float to_skel_y = dy / distance;
            //dot product between vacuum and direction to skeleton to tell how close the skeleton is
            float dot = to_skel_x * dir_x + to_skel_y * dir_y;
            if (dot > 0.5f) {
                //if sskeleton is very close coming in the vacuum range then captur it and deactive it means that skeleton will dissapear
                if (distance_sq < capture_distance_sq) {
                    skel_active[i] = false;
                    skel_x[i]=-1000;
                    skel_y[i]=-1000;
                    continue;
                }
                //if skeleton if within the range of vacumm then pull it towards the player which is controlled by suck strength
                skel_x[i] -= to_skel_x * suck_strength;
                skel_y[i] -= to_skel_y * suck_strength;
            }
        }
    }
}

// NEW FUNCTION: Check collision between player and enemies
bool check_player_enemy_collision(
    float player_x, float player_y, int PlayerWidth, int PlayerHeight,
    float enemy_x, float enemy_y, int enemy_size)
{
    return (player_x < enemy_x + enemy_size &&
        player_x + PlayerWidth > enemy_x &&
        player_y < enemy_y + enemy_size &&
        player_y + PlayerHeight > enemy_y);
}

// NEW FUNCTION: Respawn player from top
void respawn_player(float& player_x, float& player_y, float& velocityY, bool& playerDead)
{
    player_x = 500;
    player_y = 150;
    velocityY = 0;
    playerDead = false;
}

// Initialize level layout
void init_level(char** lvl, int height, int width, int levelNumber)
{
    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) lvl[i][j] = ' ';
    }
    if(levelNumber == 1) {
        // LEVEL 1 LAYOUT (your existing layout)
        for (int j = 0; j < width; ++j) lvl[8][j] = '#';
        lvl[5][6] = '#'; lvl[5][7] = '#'; lvl[5][8] = '#';
        lvl[5][9] = '#'; lvl[5][10] = '#'; lvl[5][11] = '#';
        for(int i = 0; i <=7; ++i) lvl[i][0] = '#';
        for(int i = 0; i <=7; ++i) lvl[i][17] = '#';
        lvl[3][0] = '#'; lvl[3][1] = '#'; lvl[3][2] = '#'; lvl[3][3] = '#';
        lvl[3][14] = '#'; lvl[3][15] = '#'; lvl[3][16] = '#'; lvl[3][17] = '#';
        for (int j = 0; j < width; ++j) lvl[0][j] = '#';
    }
    else if(levelNumber == 2) {
        // LEVEL 2 LAYOUT - More platforms
        for (int j = 0; j < width; ++j) lvl[8][j] = '#';
        for(int j = 3; j <= 7; j++) lvl[6][j] = '#';
        for(int j = 10; j <= 14; j++) lvl[6][j] = '#';
        for(int j = 5; j <= 12; j++) lvl[4][j] = '#';
        for(int i = 0; i <=7; ++i) lvl[i][0] = '#';
        for(int i = 0; i <=7; ++i) lvl[i][17] = '#';
        for (int j = 0; j < width; ++j) lvl[0][j] = '#';
    }
}

// Spawn enemies for current level
void spawn_enemies_for_level(
    float ghost_x[], float ghost_y[], float ghost_speed[], int ghost_dir[],
    float ghost_velocityY[], bool ghost_onGround[], bool ghost_active[],
    int ghostCurrentFrame[], int ghostFrameCounter[],
    float skel_x[], float skel_y[], float skel_speed[], int skel_dir[],
    float skel_velocityY[], bool skel_onGround[], bool skel_active[],
    char** lvl, int height, int width, int cell_size,
    int ghosts, int skel, int levelNumber)
{
    srand(time(0) + levelNumber);
    for(int i = 0; i < ghosts; i++) {
        while(true) {
            int tx = rand() % width;
            int ty = rand() % (height - 1);
            bool onPlayerPlatform = (ty == 4 && tx >= 6 && tx <= 11);
            if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#' && !onPlayerPlatform) {
                ghost_x[i] = tx * cell_size;
                ghost_y[i] = ty * cell_size;
                break;
            }
        }
        ghost_speed[i] = 1;
        ghost_dir[i] = (rand() % 2 == 0) ? -1 : 1;
        ghost_velocityY[i] = 0;
        ghost_onGround[i] = false;
        ghost_active[i] = true;
        ghostCurrentFrame[i] = 0;
        ghostFrameCounter[i] = 0;
    }
    for(int i = 0; i < skel; i++) {
        while(true) {
            int tx = rand() % width;
            int ty = rand() % (height - 1);
            bool onPlayerPlatform = (ty == 4 && tx >= 6 && tx <= 11);
            if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#' && !onPlayerPlatform) {
                skel_x[i] = tx * cell_size;
                skel_y[i] = ty * cell_size;
                break;
            }
        }
        skel_speed[i] = 1;
        skel_dir[i] = (rand() % 2 == 0) ? -1 : 1;
        skel_velocityY[i] = 0;
        skel_onGround[i] = false;
        skel_active[i] = true;
    }
}

// Draws the level: background (already drawn externally) and blocks where lvl[][] == '#'
void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
    window.draw(bgSprite);
    for (int i = 0; i < height; i += 1) {
        for (int j = 0; j < width; j += 1) {
            if (lvl[i][j] == '#') {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
        }
    }
}

// Applies gravity to player, checks if player is on ground, updates velocity/position
void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
    offset_y = player_y;
    offset_y += velocityY; // predict new Y
    // Check three points below the player (left, center, right) for collision with blocks
    char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
    char bottom_right_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
    char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];
    if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#') onGround = true;
    else {
        player_y = offset_y;
        onGround = false;
    }
    if (!onGround) {
        velocityY += gravity; // accelerate downward
        if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity; // limit fall speed
    }
    else velocityY = 0; // on ground -> stop vertical velocity
}

// LEFT COLLISION
// Attempt to move left; if a block exists on left-edge points, do not move
void player_left_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x; // start from current
    offset_x -= speed; // next predicted x after moving left
    // Check three points on the left side of the player (top, middle, bottom)
    char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
    char left_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];
    if (left_top == '#' || left_mid == '#' || left_bottom == '#') offset_x = player_x ; // collision -> don't move
    else player_x = offset_x; // safe -> commit movement
}

// RIGHT COLLISION - same as left but to the right
void player_right_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x;
    offset_x += speed;
    // Check three points on the right side of the player (top, middle, bottom)
    char right_top = lvl[(int)(player_y) / cell_size][(int)(offset_x + Pwidth) / cell_size];
    char right_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x + Pwidth) / cell_size];
    char right_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x + Pwidth) / cell_size];
    if (right_top == '#' || right_mid == '#' || right_bottom == '#') offset_x = player_x ; // don't move into block
    else player_x = offset_x; // commit movement
}

// CEILING COLLISION - prevents moving up into blocks
void player_ceiling_collision(char** lvl, float& offset_y, float& velocityY, float& player_x, float& player_y, const int cell_size, int& Pwidth)
{
    offset_y = player_y;
    offset_y += velocityY; // predicted top position (velocityY negative when jumping)
    // Check three points on the top of the player (left, middle, right)
    char top_left = lvl[(int)(offset_y) / cell_size][(int)(player_x) / cell_size];
    char top_mid = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth/2) / cell_size];
    char top_right = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth) / cell_size];
    if (top_left == '#' || top_mid == '#' || top_right == '#') velocityY = 0; // hit ceiling -> stop upward speed (start falling afterwards)
    else player_y = offset_y; // safe -> commit new Y
}

int main()
{
    // Create window
    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);
    //level specifics
    const int cell_size = 64; // tile dimension in pixels
    const int height = 14; // tiles vertically
    const int width = 18; // tiles horizontally
    char** lvl; // 2D dynamic array representing map cells
    //level and background textures and sprites
    Texture bgTex;
    Sprite bgSprite;
    Texture blockTexture;
    Sprite blockSprite;
    // load background & block textures (files must exist)
    bgTex.loadFromFile("bg.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);
    blockTexture.loadFromFile("block1.png");
    blockSprite.setTexture(blockTexture);
    //player data (position, speed, size etc.)
    float player_x = 500;
    float player_y = 150;
    float speed = 5;
    const float jumpStrength = -15; // negative -> upward
    const float gravity = 1; // gravity per frame
    bool onGround = false;
    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;
    float terminal_Velocity = 20; // clamp falling speed
    int PlayerHeight = 102;
    int PlayerWidth = 96;
    Font scoreFont;
    Text scoreText;
    Text comboText;
    // ===== LEVEL SYSTEM =====
    int currentLevel = 1;
    bool levelTransition = false;
    Clock transitionClock;
    Text levelText;
    levelText.setFont(scoreFont);
    levelText.setCharacterSize(50);
    levelText.setFillColor(Color::Green);
    levelText.setPosition(400, 400);
    // ===== SCORING SYSTEM =====
    int score = 0;
    int combo = 0; // Track consecutive kills without taking damage
    float comboMultiplier = 1.0f;
    bool levelComplete = false;
    bool tookDamage = false; // Track if player took any damage in the level
    Clock levelTimer; // Timer for level completion speed
    if(!scoreFont.loadFromFile("Roboto-Regular.ttf")) cout << "Error loading font!" << endl;
    scoreText.setFont(scoreFont);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(20, 10);
    comboText.setFont(scoreFont);
    comboText.setCharacterSize(25);
    comboText.setFillColor(Color::Yellow);
    comboText.setPosition(20, 50);
    levelTimer.restart(); // Start timing the level
    // ===== PLAYER LIVES SYSTEM =====
    int playerLives = 3;
    bool playerDead = false;
    Clock deathClock;
    const float respawnDelay = 2.0f;
    Texture PlayerTexture;
    Sprite PlayerSprite;
    Texture bagTexture;
    Sprite bagSprite;
    bool facingRight = false;
    // Animation variables
<<<<<<< HEAD
    const int animationFrames = 4;
    Texture walkTextures[4];
    int currentFrame = 0;
    int frameCounter = 0;
    const int frameDelay = 10; // (lower = faster)
    bool isWalking = false;
=======
const int animationFrames = 4;  
const int bagFrames = 4;
Texture walkTextures[4]; 
Texture bagTextures[4];       
int currentFrame = 0;  
int currentBagFrame =0; 
int frameCounter = 0;  
int bagFrameCounter = 0;         
const int frameDelay = 10;      // (lower = faster)
bool isWalking = false;         

>>>>>>> c5504a66be96ace49bddf6f0c5805879b3d4f5e4
    // Music objects and settings
    Music menuMusic;
    Music lvlMusic;
    menuMusic.openFromFile("bgmus.ogg");
    menuMusic.setLoop(true);
    menuMusic.setVolume(40);
    lvlMusic.openFromFile("mus.ogg");
    lvlMusic.setLoop(true);
    lvlMusic.setVolume(40);
    // ===== PLAYER SELECTION MENU =====
    const int playerOptionsCount = 2; // number of characters
    Texture playerOptions[playerOptionsCount];
    Sprite playerOptionSprite[playerOptionsCount];
    // Load textures for each character (must exist)
    playerOptions[0].loadFromFile("player1.png");
    playerOptions[1].loadFromFile("player2.png");
    const int bagOptionsCount = 2;
    Texture bagOptions[bagOptionsCount];
    Sprite bagOptionSprite[bagOptionsCount];
    bagOptions[0].loadFromFile("bag1.png");
    bagOptions[1].loadFromFile("bag2.png");
    /// Set positions and scale for menu display
    for(int i = 0; i < playerOptionsCount; i++) {
        bagOptionSprite[i].setTexture(bagOptions[i]);
        bagOptionSprite[i].setScale(2,2);
        bagOptionSprite[i].setPosition(300 + i*300+73, 200+41); // Changed from 300+41 to 200+41
        playerOptionSprite[i].setTexture(playerOptions[i]);
        playerOptionSprite[i].setScale(3,3);
        playerOptionSprite[i].setPosition(300 + i*300, 200); // Changed from 300 to 200
    }
    // ===== DESCRIPTION IMAGES =====
    Texture descTextures[2];
    Sprite descSprites[2];
    descTextures[0].loadFromFile("greendesc.png");
    descTextures[1].loadFromFile("yellowdesc.png");
    descSprites[0].setTexture(descTextures[0]);
    descSprites[0].setPosition(200, 300); // Moved right (280→320) and up (500→400)
    descSprites[1].setTexture(descTextures[1]);
    descSprites[1].setPosition(500, 300); // Moved right (580→620) and up (500→400)
    // Selection variables
    int selectedIndex = 0;
    bool playerChosen = false;
    menuMusic.play(); // play menu music
    // Simple menu loop (blocks until player chosen)
    while(!playerChosen && window.isOpen()) {
        Event ev;
        while(window.pollEvent(ev)) {
            if(ev.type == Event::Closed) window.close();
            if(ev.type == Event::KeyPressed) {
                if(ev.key.code == Keyboard::Right) selectedIndex = (selectedIndex + 1) % playerOptionsCount;
                if(ev.key.code == Keyboard::Left) selectedIndex = (selectedIndex - 1 + playerOptionsCount) % playerOptionsCount;
                if(ev.key.code == Keyboard::Enter) {
                    playerChosen = true;
                    menuMusic.stop();
                }
            }
        }
        // apply chosen texture to PlayerSprite for preview
        PlayerTexture = playerOptions[selectedIndex];
        PlayerSprite.setTexture(PlayerTexture);
        PlayerSprite.setScale(3,3);
        PlayerSprite.setPosition(player_x, player_y);
        // Load walking animation frames based on selected player
<<<<<<< HEAD
        if(selectedIndex == 0) {
            walkTextures[0].loadFromFile("player1walk1.png");
            walkTextures[1].loadFromFile("player1walk2.png");
            walkTextures[2].loadFromFile("player1walk3.png");
            walkTextures[3].loadFromFile("player1walk4.png");
        }
        else if(selectedIndex == 1) {
            walkTextures[0].loadFromFile("player2walk1.png");
            walkTextures[1].loadFromFile("player2walk2.png");
            walkTextures[2].loadFromFile("player2walk3.png");
            walkTextures[3].loadFromFile("player2walk4.png");
        }
=======
if(selectedIndex == 0)
{
    walkTextures[0].loadFromFile("player1walk1.png");
    walkTextures[1].loadFromFile("player1walk2.png");
    walkTextures[2].loadFromFile("player1walk3.png");
    walkTextures[3].loadFromFile("player1walk4.png");
    bagTextures[0].loadFromFile("bagpone1.png");
    bagTextures[1].loadFromFile("bagpone2.png");
    bagTextures[2].loadFromFile("bagpone3.png");
     bagTextures[3].loadFromFile("bagpone4.png");
    
}
else if(selectedIndex == 1)
{
    walkTextures[0].loadFromFile("player2walk1.png");
    walkTextures[1].loadFromFile("player2walk2.png");
    walkTextures[2].loadFromFile("player2walk3.png");
    walkTextures[3].loadFromFile("player2walk4.png");
    bagTextures[0].loadFromFile("bagptwo1.png");
    bagTextures[1].loadFromFile("bagptwo2.png");
    bagTextures[2].loadFromFile("bagptwo3.png");
     bagTextures[3].loadFromFile("bagptwo4.png");
}
>>>>>>> c5504a66be96ace49bddf6f0c5805879b3d4f5e4
        bagTexture = bagOptions[selectedIndex];
        bagSprite.setTexture(bagTexture);
        bagSprite.setScale(2, 2);
        window.clear();
        // highlight selected option
        for(int i = 0; i < playerOptionsCount; i++) {
            if(i == selectedIndex) {
                playerOptionSprite[i].setColor(Color::Yellow);
                bagOptionSprite[i].setColor(Color::Yellow);
            }
            else {
                playerOptionSprite[i].setColor(Color::White);
                bagOptionSprite[i].setColor(Color::White);
            }
            window.draw(bagOptionSprite[i]);
            window.draw(playerOptionSprite[i]);
            window.draw(descSprites[i]); // This draws the description images
        }
        window.display();
    }
    bool isJumping = false; // Track if jumping (unused now but available)
    // collision flags and char placeholders (many used locally later)
    bool up_collide = false;
    bool left_collide = false;
    bool right_collide = false;
    bool up_button = false;
    char top_left = '\0';
    char top_right = '\0';
    char top_mid = '\0';
    char left_mid = '\0';
    char right_mid = '\0';
    char bottom_left = '\0';
    char bottom_right = '\0';
    char bottom_mid = '\0';
    char bottom_left_down = '\0';
    char bottom_right_down = '\0';
    char bottom_mid_down = '\0';
    char top_right_up = '\0';
    char top_mid_up = '\0';
    char top_left_up = '\0';
    // Vacuum system
    bool vacuumActive = false;
    int vacuumDirection = 0; // 0=left, 1=up, 2=right, 3=down
    // Vacuum textures and sprites for each direction
    Texture vacuumLeftTex, vacuumUpTex, vacuumRightTex, vacuumDownTex;
    Sprite vacuumLeftSprite, vacuumUpSprite, vacuumRightSprite, vacuumDownSprite;
    // Load vacuum textures (add this after loading PlayerTexture)
    vacuumLeftTex.loadFromFile("left.png"); // default player with vacuum left
    vacuumUpTex.loadFromFile("up.png");
    vacuumRightTex.loadFromFile("right.png");
    vacuumDownTex.loadFromFile("down.png");
    // Setup vacuum sprites and scale them
    vacuumLeftSprite.setTexture(vacuumLeftTex);
    vacuumLeftSprite.setScale(3, 3);
    vacuumUpSprite.setTexture(vacuumUpTex);
    vacuumUpSprite.setScale(3, 3);
    vacuumRightSprite.setTexture(vacuumRightTex);
    vacuumRightSprite.setScale(3, 3);
    vacuumDownSprite.setTexture(vacuumDownTex);
    vacuumDownSprite.setScale(3, 3);
    // LASER BEAM SETUP
    Texture laserTex;
    Sprite laserSprite;
    bool laserLoaded = laserTex.loadFromFile("lazer.png"); // try to load laser image
    if(laserLoaded) laserSprite.setTexture(laserTex);
    //creating level array (dynamic 2D char array)
    lvl = new char* [height];
    for (int i = 0; i < height; i += 1) lvl[i] = new char[width];
    lvlMusic.play();
    lvlMusic.setLoop(true);
    // Initialize level cells to empty space ' '
    for(int i = 0; i < height; i += 1) for (int j = 0; j < width; j += 1) lvl[i][j] = ' ';
    // ====== Hard-coded blocks (level layout) ======
    ///// BOTTOM BLOCKS
    for (int j = 0; j < width; ++j) lvl[8][j] = '#'; // you can replace with loop like this
    //// BOTTOM UPPER PART
    lvl[5][6] = '#'; lvl[5][7] = '#'; lvl[5][8] = '#';
    lvl[5][9] = '#'; lvl[5][10] = '#'; lvl[5][11] = '#';
    //// LEFT SIDE wall
    for(int i = 0; i <=7; ++i) lvl[i][0] = '#';
    //// RIGHT PART wall
    for(int i = 0; i <=7; ++i) lvl[i][17] = '#';
    //// RIGHT FORWARD PART (these were set originally, maybe misnamed)
    lvl[3][0] = '#'; lvl[3][1] = '#'; lvl[3][2] = '#'; lvl[3][3] = '#';
    //// LEFT FORWARD PART
    lvl[3][14] = '#'; lvl[3][15] = '#'; lvl[3][16] = '#'; lvl[3][17] = '#';
    /// UPPER PART (top ceiling)
    for (int j = 0; j < width; ++j) lvl[0][j] = '#';
    // ================= GHOST SETUP =================
    const int ghosts = 8;
    float ghost_x[8];
    float ghost_y[8];
    float ghost_speed[8];
    int ghost_dir[8];
    float ghost_velocityY[8];
    bool ghost_onGround[8];
    Sprite ghostSprite[8];
    bool ghost_active[8]; // active flag per ghost
    const int ghostAnimationFrames = 4;
    Texture ghostWalkTextures[4];
    int ghostCurrentFrame[8]; // cuuurrent frame for each ghost
    int ghostFrameCounter[8]; // Frame counter for each ghost
    const int ghostFrameDelay = 10; // aniimation speed
    // Load ghost walking animation frames
    ghostWalkTextures[0].loadFromFile("ghost1.png");
    ghostWalkTextures[1].loadFromFile("ghost2.png");
    ghostWalkTextures[2].loadFromFile("ghost3.png");
    ghostWalkTextures[3].loadFromFile("ghost4.png");
    //spawning ghosts
    srand(time(0)); // seed RNG
    for(int i = 0; i < ghosts; i++) {
        // pick random tile that is empty and has ground below
        while(true) {
            int tx = rand() % width;
            int ty = rand() % (height - 1);
            // Player spawns at row 4, platform is at row 5, columns 6-11
            // Exclude spawning on the player's starting platform
            bool onPlayerPlatform = (ty == 4 && tx >= 6 && tx <= 11);
            if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#' && !onPlayerPlatform) {
                ghost_x[i] = tx * cell_size;
                ghost_y[i] = ty * cell_size;
                break;
            }
        }
        ghost_speed[i] = 1;
        ghost_dir[i] = (rand() % 2 == 0) ? -1 : 1; // left or right
        ghost_velocityY[i]=0;
        ghost_onGround[i]=false;
        ghost_active[i]=true;
        ghostSprite[i].setTexture(ghostWalkTextures[0]);
        ghostSprite[i].setScale(2, 2); // increase ghost size
        // Flip sprite if starting direction is right
        if(ghost_dir[i] == 1) ghostSprite[i].setScale(-2, 2);
        ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
        ghostCurrentFrame[i] = 0;
        ghostFrameCounter[i] = 0;
    }
    // SKELETONS - similar setup
    const int skel = 4;
    float skel_x[4];
    float skel_y[4];
    float skel_speed[4];
    int skel_dir[4];
    float skel_velocityY[4];
    bool skel_onGround[4];
    Sprite skelSprite[4];
    bool skel_active[4];
    Texture skelTexture;
<<<<<<< HEAD
    skelTexture.loadFromFile("skeleton.png");
    //spawning skeletons
    srand(time(0));
    for(int i = 0; i < skel; i++) {
        // pick random platform tile (empty + ground below)
        while(true) {
            int tx = rand() % width;
            int ty = rand() % (height - 1);
            // Exclude spawning on the player's starting platform
            bool onPlayerPlatform = (ty == 4 && tx >= 6 && tx <= 11);
            if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#' && !onPlayerPlatform) {
                skel_x[i] = tx * cell_size;
                skel_y[i] = ty * cell_size;
                break;
            }
=======
    const int skelAnimationFrames = 4;
     Texture skelWalkTextures[4];
     int skelCurrentFrame[8];      // cuuurrent frame for each skel
int skelFrameCounter[8];      // Frame counter for each skel
const int skelFrameDelay = 10; // aniimation speed 

    
    
    
    skelWalkTextures[0].loadFromFile("skel1.png");
skelWalkTextures[1].loadFromFile("skel2.png");
skelWalkTextures[2].loadFromFile("skel3.png");
skelWalkTextures[3].loadFromFile("skel4.png");


  //spawning skeletons
srand(time(0));

for(int i = 0; i < skel; i++)
{
    // pick random platform tile (empty + ground below)
    while(true)
    {
        int tx = rand() % width;
        int ty = rand() % (height - 1);

        // Exclude spawning on the player's starting platform
        bool onPlayerPlatform = (ty == 4 && tx >= 6 && tx <= 11);
       
        if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#' && !onPlayerPlatform)
        {
            skel_x[i] = tx * cell_size;
            skel_y[i] = ty * cell_size;
            break;
>>>>>>> c5504a66be96ace49bddf6f0c5805879b3d4f5e4
        }
        skel_speed[i] = 1;
        skel_dir[i] = (rand() % 2 == 0) ? -1 : 1;
        skel_velocityY[i]=0;
        skel_onGround[i]=false;
        skel_active[i]=true;
<<<<<<< HEAD
        skelSprite[i].setTexture(skelTexture);
        skelSprite[i].setScale(2, 2);
=======
        skelSprite[i].setTexture(skelWalkTextures[0]);
        skelSprite[i].setScale(2, 2);   // increase ghost size
// Flip sprite if starting direction is right
if(skel_dir[i] == 1)
{
    ghostSprite[i].setScale(-2, 2);
}
        skelSprite[i].setPosition(skel_x[i], skel_y[i]);
        skelCurrentFrame[i] = 0;
    skelFrameCounter[i] = 0;
    

>>>>>>> c5504a66be96ace49bddf6f0c5805879b3d4f5e4
        skelSprite[i].setPosition(skel_x[i], skel_y[i]);
    }
    const int MAX_PROJECTILES = 20;
    bool proj_active[MAX_PROJECTILES];
    // ===== CAPTURED ENEMY STACK (LIFO) =====
    const int MAX_CAPTURED = 12;
    int captured_enemy_index[MAX_CAPTURED];
    bool captured_is_ghost[MAX_CAPTURED];
    int captured_count = 0;
    // ===== ROLLING BALL PROJECTILES =====
    const int MAX_BALLS = 12;
    float ball_x[MAX_BALLS];
    float ball_y[MAX_BALLS];
    float ball_vx[MAX_BALLS];
    float ball_vy[MAX_BALLS];
    bool ball_active[MAX_BALLS];
    CircleShape ballShape[MAX_BALLS];
    const int maxBalls = MAX_BALLS; // Add this alias
    int ball_power[MAX_BALLS]; // Track how many enemies in each ball
    Sprite ballSprite[MAX_BALLS]; // Sprite for each ball
    Texture ballTexture; // Texture for balls
    const float ball_release_speed = 12.0f;
    const float ball_gravity = 0.8f;
    const float ball_bounce_factor = 0.7f;
    for(int i = 0; i < MAX_BALLS; i++) {
        ball_active[i] = false;
        ball_power[i] = 0; // ADD THIS
        ballShape[i].setRadius(20);
        ballShape[i].setOrigin(20, 20);
        ballShape[i].setFillColor(Color(255, 100, 100, 200));
        ballShape[i].setOutlineThickness(2);
        ballShape[i].setOutlineColor(Color::White);
        ballSprite[i].setTexture(ballTexture); // ADD THIS
        ballSprite[i].setScale(1.5f, 1.5f); // ADD THIS
    }
    // Initialize projectiles
    for(int i = 0; i < MAX_PROJECTILES; i++) proj_active[i] = false;
    // ===== END OF NEW CODE =====
    Event ev;
    bool xKeyPressed=false;
    bool rKeyPressed=false;
    bool rKeyPreviouslyPressed = false; // Track R key state
    // ===== TRACKING PREVIOUS FRAME DEFEATS =====
    int lastFrameGhostsSucked = 0;
    int lastFrameSkeletonsSucked = 0;
    // ===== SUCKING CAPACITY =====
    int enemiesSucked = 0; // Current count of sucked enemies
    const int maxCapacity = 3; // Maximum enemies that can be sucked at once
    //main loop
    while (window.isOpen()) {
        while (window.pollEvent(ev)) {
            if (ev.type == Event::Closed) window.close();
            if (ev.type == Event::KeyPressed) { } // KeyPressed event remains for possible future logic
        }
        // pressing escape to close
        if (Keyboard::isKeyPressed(Keyboard::Escape)) window.close();
        window.clear();
        // draw level and apply gravity to player
        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
        player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
        ///*** // Moving the character left and right using arrow keys'
        if(!playerDead) { // Only allow controls if alive
            isWalking = false;
            if(Keyboard::isKeyPressed(Keyboard::Key::Right )) {
                // attempt to move right with collision check
                player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
                bagSprite.setScale(-2,2);
                PlayerSprite.setScale(-3,3);
                facingRight = true;// flip horizontally to face right
                isWalking=true;
            }
            if(Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                // attempt to move left
                player_left_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
                bagSprite.setScale(2,2);
                PlayerSprite.setScale(3,3);
                facingRight=false; // normal facing left
                isWalking = true;
            }
            if(isWalking) {
                frameCounter++;
                if(frameCounter>=frameDelay) {
                    frameCounter=0;
                    currentFrame = (currentFrame+1) % animationFrames;
                    PlayerSprite.setTexture(walkTextures[currentFrame]);
                }
            }
            else {
                currentFrame = 0;
                frameCounter = 0;
                PlayerSprite.setTexture(PlayerTexture);
            }
            // Ceiling collision when moving up (velocityY < 0)
            if(velocityY<0) player_ceiling_collision(lvl, offset_y, velocityY, player_x, player_y, cell_size, PlayerWidth);
            // Jump with up if on ground
            if(Keyboard::isKeyPressed(Keyboard::Key::Up)) if(onGround) velocityY=jumpStrength;
            // ============= VACUUM CONTROL WITH WASD =============
            if(Keyboard::isKeyPressed(Keyboard::Key::W)) { vacuumActive = true; vacuumDirection = 1; } // Up
            else if(Keyboard::isKeyPressed(Keyboard::Key::A)) { vacuumActive = true; vacuumDirection = 0; } // Left
            else if(Keyboard::isKeyPressed(Keyboard::Key::S)) { vacuumActive = true; vacuumDirection = 3; } // Down
            else if(Keyboard::isKeyPressed(Keyboard::Key::D)) { vacuumActive = true; vacuumDirection = 2; } // Right
            else vacuumActive = false; // No vacuum key pressed
        } //end of player controls if alive
        // ============= VACUUM SUCKING WITH Space KEY =============
        xKeyPressed = Keyboard::isKeyPressed(Keyboard::Key::Space);
        rKeyPressed = Keyboard::isKeyPressed(Keyboard::Key::R);
        const int MAX_VACUUM_CAPACITY = 3;
        if(xKeyPressed && vacuumActive && captured_count < MAX_CAPTURED) {
            const float vacuum_range = 200.0f;
            const float capture_distance_sq = 1600.0f; // Reduced capture distance
            float player_center_x = player_x + PlayerWidth / 2.0f;
            float player_center_y = player_y + PlayerHeight / 2.0f;
            // Vacuum direction as unit vector
            float dir_x = 0, dir_y = 0;
            switch (vacuumDirection) {
                case 0: dir_x = -1; dir_y = 0; break; // Left
                case 1: dir_x = 0; dir_y = -1; break; // Up
                case 2: dir_x = 1; dir_y = 0; break; // Right
                case 3: dir_x = 0; dir_y = 1; break; // Down
            }
            // Suck ghosts
            for (int i = 0; i < ghosts; i++) {
                if (!ghost_active[i]) continue;
                float ghost_center_x = ghost_x[i] + cell_size / 2.0f;
                float ghost_center_y = ghost_y[i] + cell_size / 2.0f;
                float dx = ghost_center_x - player_center_x;
                float dy = ghost_center_y - player_center_y;
                float distance_sq = dx * dx + dy * dy;
                if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f) {
                    float distance = sqrt(distance_sq);
                    float to_ghost_x = dx / distance;
                    float to_ghost_y = dy / distance;
                    float dot = to_ghost_x * dir_x + to_ghost_y * dir_y;
                    if (dot > 0.5f) {
                        // Check if captured
                        if (distance_sq < capture_distance_sq) {
                        if(captured_count>MAX_VACUUM_CAPACITY) continue;
                            ghost_active[i] = false;
                            ghost_x[i] = -1000;
                            ghost_y[i] = -1000;
                            // Add to captured stack
                            captured_enemy_index[captured_count] = i;
                            captured_is_ghost[captured_count] = true;
                            captured_count++;
                            continue;
                        }
                        // Pull ghost toward player
                        ghost_x[i] -= to_ghost_x * 4.0f;
                        ghost_y[i] -= to_ghost_y * 4.0f;
                    }
                }
            }
            // Suck skeletons (same logic)
            for (int i = 0; i < skel; i++) {
                if (!skel_active[i]) continue;
                float skel_center_x = skel_x[i] + cell_size / 2.0f;
                float skel_center_y = skel_y[i] + cell_size / 2.0f;
                float dx = skel_center_x - player_center_x;
                float dy = skel_center_y - player_center_y;
                float distance_sq = dx * dx + dy * dy;
                if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f) {
                    float distance = sqrt(distance_sq);
                    float to_skel_x = dx / distance;
                    float to_skel_y = dy / distance;
                    float dot = to_skel_x * dir_x + to_skel_y * dir_y;
                    if (dot > 0.5f) {
                        if (distance_sq < capture_distance_sq) {
                        if(captured_count>MAX_VACUUM_CAPACITY) continue;
                            skel_active[i] = false;
                            skel_x[i] = -1000;
                            skel_y[i] = -1000;
                            // Add to captured stack
                            captured_enemy_index[captured_count] = i;
                            captured_is_ghost[captured_count] = false;
                            captured_count++;
                            continue;
                        }
                        skel_x[i] -= to_skel_x * 4.0f;
                        skel_y[i] -= to_skel_y * 4.0f;
                    }
                }
            }
        }
        // ===== RELEASE CAPTURED ENEMIES WITH R KEY =====
        bool rKeyPressed = Keyboard::isKeyPressed(Keyboard::Key::R);
        static bool rKeyPreviouslyPressed = false;
       
        if(rKeyPressed && !rKeyPreviouslyPressed && enemiesSucked > 0)
        {
            // Find and release ONE enemy (the last one sucked)
            bool released = false;
           
            // Try to release a skeleton first (check backwards for last sucked)
            for(int i = skel - 1; i >= 0 && !released; i--)
            {
                if(!skel_active[i] && skel_x[i] == -1000 && skel_y[i] == -1000)
                {
                    // Mark skeleton as permanently gone (released)
                    skel_x[i] = -5000;
                    skel_y[i] = -5000;
                    score += 75;  // Bonus for releasing skeleton
                    released = true;
                }
            }
           
            // If no skeleton found, release a ghost
            for(int i = ghosts - 1; i >= 0 && !released; i--)
            {
                if(!ghost_active[i] && ghost_x[i] == -1000 && ghost_y[i] == -1000)
                {
                    // Mark ghost as permanently gone (released)
                    ghost_x[i] = -5000;
                    ghost_y[i] = -5000;
                    score += 50;  // Bonus for releasing ghost
                    released = true;
                }
            }
        }
       
        rKeyPreviouslyPressed = rKeyPressed;

        // ====== COUNT SUCKED ENEMIES (ONLY THOSE IN VACUUM AT -1000) ======
enemiesSucked = 0;
int currentGhostsSucked = 0;
int currentSkeletonsSucked = 0;
int totalGhostsDefeated = 0;
int totalSkeletonsDefeated = 0;

// Count ONLY enemies currently in vacuum (at position -1000)
for(int i = 0; i < ghosts; i++)
{
    if(!ghost_active[i])
    {
        totalGhostsDefeated++;  // Total defeated (including released)
       
        // Count only if in vacuum (not released)
        if(ghost_x[i] == -1000 && ghost_y[i] == -1000)
        {
            currentGhostsSucked++;
            enemiesSucked++;
        }
    }
}
for(int i = 0; i < skel; i++)
{
    if(!skel_active[i])
    {
        totalSkeletonsDefeated++;  // Total defeated (including released)
       
        // Count only if in vacuum (not released)
        if(skel_x[i] == -1000 && skel_y[i] == -1000)
        {
            currentSkeletonsSucked++;
            enemiesSucked++;
        }
    }
}


        // ====== CHECK IF ALL ENEMIES ARE DEFEATED ======
        int totalActiveEnemies = 0;
        for(int i = 0; i < ghosts; i++) if(ghost_active[i]) totalActiveEnemies++;
        for(int i = 0; i < skel; i++) if(skel_active[i]) totalActiveEnemies++;
        // If all enemies defeated and not already transitioning
        if(totalActiveEnemies == 0 && !levelTransition && !playerDead) {
            levelTransition = true;
            transitionClock.restart();
            if(currentLevel == 1) levelText.setString("LEVEL 1 COMPLETE!\nLoading Level 2...");
            else levelText.setString("YOU WIN!\nAll Levels Complete!");
        }
        // ====== CONVERT SUCKED ENEMIES TO BALLS AUTOMATICALLY ======
        for(int i = 0; i < ghosts; i++) {
            if(!ghost_active[i] && ghost_x[i] == -1000) { // Just sucked this frame
                // Find empty ball slot
                for(int b = 0; b < maxBalls; b++) {
                    if(!ball_active[b]) {
                        ball_active[b] = true;
                        ball_x[b] = player_x + PlayerWidth / 2.0f;
                        ball_y[b] = player_y + PlayerHeight / 2.0f;
                        ball_vx[b] = 0; // Held by player initially
                        ball_power[b] = 1;
                        ballSprite[b].setTexture(ballTexture);
                        ballSprite[b].setScale(1.5f, 1.5f);
                        ghost_x[i] = -2000; // Mark as already converted (not -1000)
                        break;
                    }
                }
            }
        }
        // Same for skeletons
        for(int i = 0; i < skel; i++) {
            if(!skel_active[i] && skel_x[i] == -1000) { // Just sucked this frame
                for(int b = 0; b < maxBalls; b++) {
                    if(!ball_active[b]) {
                        ball_active[b] = true;
                        ball_x[b] = player_x + PlayerWidth / 2.0f;
                        ball_y[b] = player_y + PlayerHeight / 2.0f;
                        ball_vx[b] = 0;
                        ball_power[b] = 1;
                        ballSprite[b].setTexture(ballTexture);
                        ballSprite[b].setScale(1.5f, 1.5f);
                        skel_x[i] = -2000; // Mark as converted
                        break;
                    }
                }
            }
        }
        for(int i = 0; i < skel; i++) if(!skel_active[i]) { currentSkeletonsSucked++; enemiesSucked++; }
        // Calculate NEW defeats this frame (compare to last frame)
int newGhosts = totalGhostsDefeated - lastFrameGhostsSucked;
int newSkeletons = totalSkeletonsDefeated - lastFrameSkeletonsSucked;
int newKills = newGhosts + newSkeletons;
<<<<<<< HEAD
        if(newKills > 0) {
            int basePoints = 0;
            // ===== BASE POINTS PER ENEMY TYPE =====
            basePoints += newGhosts * 50; // Ghosts: 50 points each
            basePoints += newSkeletons * 75; // Skeletons: 75 points each
            // ===== MULTI-KILL BONUSES (only for 2+ enemies killed at same time) =====
            if(newKills == 2) basePoints += 200; // Multi-Kill bonus: exactly 2 enemies
            else if(newKills >= 3) basePoints += 500; // Multi-Kill bonus: 3 or more enemies
            // ===== AERIAL DEFEAT BONUS (defeat enemies while in air) =====
            if(!onGround) basePoints += 150 * newKills; // 150 bonus per enemy defeated mid-air
            // ===== COMBO STREAK SYSTEM (only builds on multi-kills) =====
            // Combo only increases for multi-kills (2+ enemies at once)
            if(newKills >= 2) combo += newKills;
            else combo += 1; // Single kill doesn't build combo, but doesn't break it either
            // Calculate combo multiplier based on streak
            if(combo >= 8) comboMultiplier = 3.0f; // 8+ streak: 3x multiplier
            else if(combo >= 5) comboMultiplier = 2.0f; // 5-7 streak: 2x multiplier
            else if(combo >= 3) comboMultiplier = 1.5f; // 3-4 streak: 1.5x multiplier
            else comboMultiplier = 1.0f; // 1-2 kills: No multiplier
            // Apply combo multiplier to final score
            int finalPoints = (int)(basePoints * comboMultiplier);
            score += finalPoints;
        }
       // Update trackers for next frame
lastFrameGhostsSucked = totalGhostsDefeated;
lastFrameSkeletonsSucked = totalSkeletonsDefeated;
        // ====== LAUNCH BALLS (R KEY) ======
        if(rKeyPressed && !rKeyPreviouslyPressed) {
            // Launch all held balls in vacuum direction
            for(int b = 0; b < maxBalls; b++) {
                if(ball_active[b] && ball_vx[b] == 0) { // Ball is held by player
                    float speed = 12.0f;
                    switch(vacuumDirection) {
                        case 0: ball_vx[b] = -speed; break; // Left
                        case 2: ball_vx[b] = speed; break; // Right
                        case 1: // Up - default to right
                        case 3: // Down - default to right
                            ball_vx[b] = (facingRight ? speed : -speed);
                            break;
                    }
                    // Keep ball at player's vertical center
                    ball_y[b] = player_y + PlayerHeight / 2.0f;
                }
            }
        }
        rKeyPreviouslyPressed = rKeyPressed;
        // ====== UPDATE AND DRAW BALLS ======
        for(int b = 0; b < maxBalls; b++) {
            if(ball_active[b]) {
                // Only move horizontally - NO GRAVITY!
                ball_x[b] += ball_vx[b];
                // Bounce off screen walls
                if(ball_x[b] <= cell_size || ball_x[b] >= (width - 1) * cell_size) {
                    ball_vx[b] = -ball_vx[b]; // Reverse horizontal direction
                    // Keep in bounds
                    if(ball_x[b] <= cell_size) ball_x[b] = cell_size + 1;
                    if(ball_x[b] >= (width - 1) * cell_size) ball_x[b] = (width - 2) * cell_size;
                }
                // Check collision with active ghosts
                for(int i = 0; i < ghosts; i++) {
                    if(ghost_active[i]) {
                        float dx = (ghost_x[i] + 32) - ball_x[b];
                        float dy = (ghost_y[i] + 32) - ball_y[b];
                        float dist = sqrt(dx*dx + dy*dy);
                        if(dist < 50) { // Hit ghost
                            ghost_active[i] = false;
                            ghost_x[i] = -3000; // Move far away (never respawn)
                            ghost_y[i] = -3000;
                            ball_power[b]--; // Lose one enemy from ball
                            score += 100;
                            combo++;
                            // Ball disappears when empty
                            if(ball_power[b] <= 0) ball_active[b] = false;
                            break;
                        }
                    }
                }
                // Check collision with active skeletons (only if ball still active)
                if(ball_active[b]) {
                    for(int i = 0; i < skel; i++) {
                        if(skel_active[i]) {
                            float dx = (skel_x[i] + 32) - ball_x[b];
                            float dy = (skel_y[i] + 32) - ball_y[b];
                            float dist = sqrt(dx*dx + dy*dy);
                            if(dist < 50) { // Hit skeleton
                                skel_active[i] = false;
                                skel_x[i] = -3000; // Move far away (never respawn)
                                skel_y[i] = -3000;
                                ball_power[b]--;
                                score += 125;
                                combo++;
                                if(ball_power[b] <= 0) ball_active[b] = false;
                                break;
                            }
                        }
                    }
                }
                // Draw ball if still active
                if(ball_active[b]) {
                    ballSprite[b].setPosition(ball_x[b], ball_y[b]);
                    window.draw(ballSprite[b]);
                }
            }
        }
=======





     if(newKills > 0)
{
    int basePoints = 0;
   
    // ===== BASE POINTS PER ENEMY TYPE =====
    basePoints += newGhosts * 50;        // Ghosts: 50 points each
    basePoints += newSkeletons * 75;     // Skeletons: 75 points each
   
    // ===== MULTI-KILL BONUSES (only for 2+ enemies killed at same time) =====
    if(newKills == 2)
    {
        basePoints += 200;  // Multi-Kill bonus: exactly 2 enemies
    }
    else if(newKills >= 3)
    {
        basePoints += 500;  // Multi-Kill bonus: 3 or more enemies
    }
   
    // ===== AERIAL DEFEAT BONUS (defeat enemies while in air) =====
    if(!onGround)
    {
        basePoints += 150 * newKills;  // 150 bonus per enemy defeated mid-air
    }
   
    // ===== COMBO STREAK SYSTEM (only builds on multi-kills) =====
    // Combo only increases for multi-kills (2+ enemies at once)
    if(newKills >= 2)
    {
        combo += newKills;
    }
    else
    {
        // Single kill doesn't build combo, but doesn't break it either
        combo += 1;
    }
   
    // Calculate combo multiplier based on streak
    if(combo >= 8)
    {
        comboMultiplier = 3.0f;  // 8+ streak: 3x multiplier
    }
    else if(combo >= 5)
    {
        comboMultiplier = 2.0f;  // 5-7 streak: 2x multiplier
    }
    else if(combo >= 3)
    {
        comboMultiplier = 1.5f;  // 3-4 streak: 1.5x multiplier
    }
    else
    {
        comboMultiplier = 1.0f;  // 1-2 kills: No multiplier
    }
   
    // Apply combo multiplier to final score
    int finalPoints = (int)(basePoints * comboMultiplier);
    score += finalPoints;     }
// Update trackers for next frame
lastFrameGhostsSucked = currentGhostsSucked;
lastFrameSkeletonsSucked = currentSkeletonsSucked;
int bagFrame = 0;
      if (enemiesSucked >= 3) bagFrame = 2;
      else if (enemiesSucked > 0) bagFrame = enemiesSucked;

      bagSprite.setTexture(bagTextures[bagFrame]);



>>>>>>> c5504a66be96ace49bddf6f0c5805879b3d4f5e4
        // ============= DRAW PLAYER OR VACUUM =============
        if(!playerDead) {
            if(vacuumActive) {
                // Draw vacuum sprite based on direction (placed at player's top-left)
                switch(vacuumDirection) {
                    case 0: // Left
                        vacuumLeftSprite.setPosition(player_x, player_y);
                        window.draw(vacuumLeftSprite);
                        break;
                    case 1: // Up
                        vacuumUpSprite.setPosition(player_x, player_y);
                        window.draw(vacuumUpSprite);
                        break;
                    case 2: // Right
                        vacuumRightSprite.setPosition(player_x, player_y);
                        window.draw(vacuumRightSprite);
                        break;
                    case 3: // Down
                        vacuumDownSprite.setPosition(player_x, player_y);
                        window.draw(vacuumDownSprite);
                        break;
                }
            }
            else {
                // Draw normal player sprite when vacuum is not active
                if(facingRight) {
                    bagSprite.setPosition(player_x-70+100, player_y+41);
                    PlayerSprite.setPosition(player_x+100, player_y);
                }
                else {
                    bagSprite.setPosition(player_x+73, player_y+41);
                    PlayerSprite.setPosition(player_x, player_y);
                }
                window.draw(bagSprite);
                window.draw(PlayerSprite);
            }
        }
        // *** LASER BEAM DRAWING WHEN Space IS PRESSED ***
        if(xKeyPressed && vacuumActive && laserLoaded) {
            float laser_x = player_x + PlayerWidth/2;
            float laser_y = player_y + PlayerHeight/2;
            // Position laser in front of vacuum based on direction (no rotation needed)
            switch(vacuumDirection) {
                case 0: // Left
                    laserSprite.setScale(3, 2);
                    laserSprite.setPosition(laser_x - 190, laser_y - 20);
                    break;
                case 1: // Up
                    laserSprite.setScale(2, 3);
                    laserSprite.setPosition(laser_x - 20, laser_y - 150);
                    break;
                case 2: // Right
                    laserSprite.setScale(-3, 2);
                    laserSprite.setPosition(laser_x + 190, laser_y - 20);
                    break;
                case 3: // Down
                    laserSprite.setScale(2, 3);
                    laserSprite.setPosition(laser_x - 20, laser_y + 50);
                    break;
            }
            window.draw(laserSprite);
        }
        // ====== GHOST MOVEMENT
        for(int i = 0; i < ghosts; i++) {
            if(!ghost_active[i]) continue; // skip inactive ghosts
            // Find tile coordinates for ground checks
            int bottomLeftX = ghost_x[i] / cell_size;
            int bottomRightX = (ghost_x[i] + 64) / cell_size;
            int bottomY = (ghost_y[i] + 64) / cell_size;
            // If ghost stands on a block, mark onGround true
            if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#') ghost_onGround[i] = true;
            // HORIZONTAL MOVEMENT - only if on ground
            if(ghost_onGround[i]) {
                float nextX = ghost_x[i] + ghost_speed[i] * ghost_dir[i];
                // front tile X coordinate (depending on direction)
                int frontTileX = (nextX + (ghost_dir[i] == 1 ? 64 : 0)) / cell_size;
                int midTileY = (ghost_y[i] + 32) / cell_size;
                // edge check: if no ground ahead then turn
                int edgeCheckX = (nextX + (ghost_dir[i] == 1 ? 64 : 0)) / cell_size;
                int edgeCheckY = (ghost_y[i] + 64 + 1) / cell_size;
                // Turn around if there's a wall ahead OR no ground ahead (edge)
                if(lvl[midTileY][frontTileX] == '#' || lvl[edgeCheckY][edgeCheckX] != '#') {
                    ghost_dir[i] *= -1;
                    if(ghost_dir[i] == 1) ghostSprite[i].setScale(-2, 2); // Now moving right
                    else ghostSprite[i].setScale(2, 2); // Now moving left
                }
                else ghost_x[i] = nextX;
                // ANIMATE GHOST WHILE MOVING
                ghostFrameCounter[i]++;
                if(ghostFrameCounter[i] >= ghostFrameDelay) {
                    ghostFrameCounter[i] = 0;
                    ghostCurrentFrame[i] = (ghostCurrentFrame[i] + 1) % ghostAnimationFrames;
                    ghostSprite[i].setTexture(ghostWalkTextures[ghostCurrentFrame[i]]);
                }
            }
            // update sprite position and draw
            ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
            window.draw(ghostSprite[i]);
        }
<<<<<<< HEAD
        // ====== SKELETON MOVEMENT WITH GRAVITY AND PLATFORM TELEPORT ======
        for(int j = 0; j < skel; j++) {
            if(!skel_active[j]) continue; // skip inactive skeletons
            // GRAVITY - predict next vertical position
            float nextY = skel_y[j] + skel_velocityY[j];
            // Check ground collision (bottom of skeleton)
            int bottomLeftX = skel_x[j] / cell_size;
            int bottomRightX = (skel_x[j] + 64) / cell_size;
            int bottomY = (nextY + 64) / cell_size;
            if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#') {
                // landed on platform
                skel_onGround[j] = true;
                skel_velocityY[j] = 0;
            }
            else {
                // falling
                skel_onGround[j] = false;
                skel_y[j] = nextY;
                skel_velocityY[j] += gravity;
                if(skel_velocityY[j] > terminal_Velocity) skel_velocityY[j] = terminal_Velocity;
            }
            // HORIZONTAL MOVEMENT - only move left/right if on ground
            if(skel_onGround[j]) {
                // RANDOM TELEPORT TO PLATFORM - rare chance per frame
                if(rand() % 200 == 0) {
                    // Find a random valid platform position (attempts limit prevents infinite loop)
                    int attempts = 0;
                    while(attempts < 50) {
                        int tx = rand() % width;
                        int ty = rand() % (height - 1);
                        // Check if it's a valid platform (empty space with ground below)
                        if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#') {
                            skel_x[j] = tx * cell_size;
                            skel_y[j] = ty * cell_size;
                            skel_velocityY[j] = 0;
                            skel_dir[j] = (rand() % 2 == 0) ? -1 : 1;
                            break;
                        }
                        attempts++;
                    }
                }
                float nextp = skel_x[j] + skel_speed[j] * skel_dir[j];
                // Check the tile directly in front of the skeleton (left or right)
                int frontskelX = (nextp + (skel_dir[j] == 1 ? 64 : 0)) / cell_size;
                int midTileY = (skel_y[j] + 32) / cell_size;
                // Turn around if there's a wall ahead
                if(lvl[midTileY][frontskelX] == '#') skel_dir[j] *= -1;
                else skel_x[j] = nextp;
            }
            skelSprite[j].setPosition(skel_x[j], skel_y[j]);
            window.draw(skelSprite[j]);
        }
        // ====== CHECK PLAYER-ENEMY COLLISIONS ======
=======
        
        // ANIMATE GHOST WHILE MOVING
        ghostFrameCounter[i]++;
        if(ghostFrameCounter[i] >= ghostFrameDelay)
        {
            ghostFrameCounter[i] = 0;
            ghostCurrentFrame[i] = (ghostCurrentFrame[i] + 1) % ghostAnimationFrames;
            ghostSprite[i].setTexture(ghostWalkTextures[ghostCurrentFrame[i]]);
        }
    }

    // update sprite position and draw
    ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
    window.draw(ghostSprite[i]);
}




        for(int i = 0; i < skel; i++)
{
    if(!skel_active[i]) continue; // skip inactive ghosts

    // Find tile coordinates for ground checks
    int bottomLeftX = skel_x[i] / cell_size;
    int bottomRightX = (skel_x[i] + 64) / cell_size;
    int bottomY = (skel_y[i] + 64) / cell_size;

    // If ghost stands on a block, mark onGround true
    if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#')
    {
        skel_onGround[i] = true;
    }

    // HORIZONTAL MOVEMENT - only if on ground
    if(skel_onGround[i])
    {
        float nextX = skel_x[i] + skel_speed[i] * skel_dir[i];

        // front tile X coordinate (depending on direction)
        int frontTileX = (nextX + (skel_dir[i] == 1 ? 64 : 0)) / cell_size;
        int midTileY = (skel_y[i] + 32) / cell_size;

        // edge check: if no ground ahead then turn
        int edgeCheckX = (nextX + (skel_dir[i] == 1 ? 64 : 0)) / cell_size;
        int edgeCheckY = (skel_y[i] + 64 + 1) / cell_size;

        // Turn around if there's a wall ahead OR no ground ahead (edge)
        if(lvl[midTileY][frontTileX] == '#' || lvl[edgeCheckY][edgeCheckX] != '#')
        {
            skel_dir[i] *= -1;
        if(skel_dir[i] == 1) // Now moving right
    {
        skelSprite[i].setScale(-2, 2);
        
    }
    
    
    else // Now moving left
    {
        skelSprite[i].setScale(2, 2);
                

    }
    }
    else
       
        {
            skel_x[i] = nextX;
        }
        
        // ANIMATE GHOST WHILE MOVING
        skelFrameCounter[i]++;
        if(skelFrameCounter[i] >= skelFrameDelay)
        {
            skelFrameCounter[i] = 0;
            skelCurrentFrame[i] = (skelCurrentFrame[i] + 1) % skelAnimationFrames;
            skelSprite[i].setTexture(skelWalkTextures[skelCurrentFrame[i]]);
        }
    }

    // update sprite position and draw
    skelSprite[i].setPosition(skel_x[i], skel_y[i]);
    window.draw(skelSprite[i]);
}
  
  // ====== CHECK PLAYER-ENEMY COLLISIONS ======
>>>>>>> c5504a66be96ace49bddf6f0c5805879b3d4f5e4
if(!playerDead)
{
    // Check ghost collisions
    for(int i = 0; i < ghosts; i++)
    {
        if(ghost_active[i])
        {
            // Calculate distance from player center to ghost center
            float player_center_x = player_x + PlayerWidth / 2.0f;
            float player_center_y = player_y + PlayerHeight / 2.0f;
            float ghost_center_x = ghost_x[i] + 32;
            float ghost_center_y = ghost_y[i] + 32;
            float dx = ghost_center_x - player_center_x;
            float dy = ghost_center_y - player_center_y;
            float distance_sq = dx * dx + dy * dy;
           
            // Skip collision if enemy is being actively sucked (within vacuum range AND vacuum is active)
            bool beingSucked = (xKeyPressed && vacuumActive && distance_sq < 200.0f * 200.0f);
           
            if(!beingSucked && check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                               ghost_x[i], ghost_y[i], 64))
{
    playerDead = true;
    deathClock.restart();
    score -= 50;  // PENALTY: Take Damage
    if(score < 0) score = 0;
    combo = 0;  // Reset combo on damage
    comboMultiplier = 1.0f;
    break;
}
        }
    }
   
    // Check skeleton collisions
    if(!playerDead)
    {
        for(int i = 0; i < skel; i++)
        {
            if(skel_active[i])
            {
                // Calculate distance from player center to skeleton center
                float player_center_x = player_x + PlayerWidth / 2.0f;
                float player_center_y = player_y + PlayerHeight / 2.0f;
                float skel_center_x = skel_x[i] + 32;
                float skel_center_y = skel_y[i] + 32;
                float dx = skel_center_x - player_center_x;
                float dy = skel_center_y - player_center_y;
                float distance_sq = dx * dx + dy * dy;
               
                // Skip collision if enemy is being actively sucked
                bool beingSucked = (xKeyPressed && vacuumActive && distance_sq < 200.0f * 200.0f);
               
               if(!beingSucked && check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                               skel_x[i], skel_y[i], 64))
{
    playerDead = true;
    deathClock.restart();
    score -= 50;  // PENALTY: Take Damage
    if(score < 0) score = 0;
    combo = 0;  // Reset combo on damage
    comboMultiplier = 1.0f;
    break;
}
            }
        }
    }
}

        // ====== HANDLE PLAYER DEATH AND RESPAWN ======
        if(playerDead) {
            float elapsedTime = deathClock.getElapsedTime().asSeconds();
            // Make player blink during death
            if((int)(elapsedTime * 4) % 2 == 0) {
                PlayerSprite.setColor(Color(255, 255, 255, 100));
                bagSprite.setColor(Color(255, 255, 255, 100));
            }
            else {
                PlayerSprite.setColor(Color(255, 255, 255, 255));
                bagSprite.setColor(Color(255, 255, 255, 255));
            }
            if(elapsedTime >= respawnDelay) {
                playerLives--;
                if(playerLives > 0) {
                    respawn_player(player_x, player_y, velocityY, playerDead);
                    PlayerSprite.setColor(Color(255, 255, 255, 255));
                    bagSprite.setColor(Color(255, 255, 255, 255));
                    vacuumActive = false;
                    enemiesSucked = 0;
                }
                else {
                    // PENALTY: Death (all 3 lives lost)
                    score -= 200;
                    if(score < 0) score = 0;
                    // Game Over - close window
                    window.close();
                }
            }
        }
        // Update and draw score
        scoreText.setString("Score: " + to_string(score) + " Lives: " + to_string(playerLives));
        window.draw(scoreText);
        // Draw combo if active
        if(combo > 0) {
            comboText.setString("Combo: " + to_string(combo) + "x");
            window.draw(comboText);
        }
        // ====== HANDLE LEVEL TRANSITION ======
        if(levelTransition) {
            window.draw(levelText);
            float transitionTime = transitionClock.getElapsedTime().asSeconds();
            if(transitionTime >= 3.0f) { // Wait 3 seconds
                if(currentLevel == 1) {
                    // Move to Level 2
                    currentLevel = 2;
                    levelTransition = false;
                    // Respawn enemies for level 2
                    spawn_enemies_for_level(
                        ghost_x, ghost_y, ghost_speed, ghost_dir,
                        ghost_velocityY, ghost_onGround, ghost_active,
                        ghostCurrentFrame, ghostFrameCounter,
                        skel_x, skel_y, skel_speed, skel_dir,
                        skel_velocityY, skel_onGround, skel_active,
                        lvl, height, width, cell_size,
                        ghosts, skel, currentLevel);
                    // Rebuild level layout
                    init_level(lvl, height, width, currentLevel);
                    // Reset player
                    player_x = 500;
                    player_y = 150;
                    velocityY = 0;
                    captured_count = 0;
                }
                else {
                    // Game complete - close after 2 more seconds
                    if(transitionTime >= 5.0f) window.close();
                }
            }
        }
        // Present everything drawn this frame
        window.display();
    }
    //stopping music and deleting level array (cleanup)
    lvlMusic.stop();
    for (int i = 0; i < height; i++) delete[] lvl[i];
    delete[] lvl;
    return 0;
}

	


