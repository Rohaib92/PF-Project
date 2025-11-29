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
    const int cell_size, const float suck_strength, int vacuumDirection)
{
    //this shows the maximum distance from which the enemy can be sucked
    const float vacuum_range = 200.0f;    
    //this tells the distance that from where the enemy should disappear after coming how much close to player    
    const float capture_distance_sq = 500.0f; 

    //this tells the player center 
    float player_center_x = player_x + PlayerWidth / 2.0f;
    float player_center_y = player_y + PlayerHeight / 2.0f;

    // Vacuum direction as unit vector
    float dir_x = 0, dir_y = 0;
    switch (vacuumDirection)
    {
        case 0://tells that the vacuum is facing left and looks for enemies on the left
        { 
          dir_x = -1;
          dir_y = 0;
          break;
         }
           
        case 1:// tells that the vacuum is faciing up and looks for enemies on up position
        {
          dir_x = 0;
          dir_y = -1;
          break;
         }  
         
        case 2://tell that the vacuum is facing rightwards and same lokks for enemies on right 
        {
         dir_x = 1;
         dir_y = 0;
         break;
         }   
        case 3://check for enemies in downward direction 
        {
         dir_x = 0;
         dir_y = 1;
         break;
         }   
    }

    //Ghost sucking mechnism
    for (int i = 0; i < ghosts; i++)
    {    
       //this check if ghost is currently active or alive in the game if not it continues and check the next ghost
        if (!ghost_active[i]) 
         {
           continue;
         } 
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
        if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f)
        {
           // next taking the squareroot to find the distance again this come from pythagerous theorum
            float distance = sqrt(distance_sq);

            //direction from player to ghost in unit vector form we need this becaoue later we need it to ensure ghost move in correct direction and in correct speed
            float to_ghost_x = dx / distance;
            float to_ghost_y = dy / distance;

            //this is the dot product formula it tells how much ghost is infornt of the camera so only ghost in vacuum direction are pulled 
            float dot = to_ghost_x * dir_x + to_ghost_y * dir_y;
   
             //checks if ghost is infront of the vacuum
            if (dot > 0.5f)
            {
                //if the ghost is too close  then capture the ghost means deactivate it
                if (distance_sq < capture_distance_sq)
                {
                    ghost_active[i] = false;
                    continue;//move to next ghost
                }

               // move ghost is not too close but still in vacuum range then pull it using suck_strngth
                ghost_x[i] -= to_ghost_x * suck_strength;
                ghost_y[i] -= to_ghost_y * suck_strength;
            }
        }
    }

    //Skeleton sucking mechanism same logic as ghost one
    for (int i = 0; i < skel; i++)
    {
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
        if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f)
        {
            //next compute actual distance we didnt find actual distance before just to make more efficiency in game
            float distance = sqrt(distance_sq);

            //next unit vertor direction to tell exactle which direction for skeleton to move
            float to_skel_x = dx / distance;
            float to_skel_y = dy / distance;
 
            //dot product between vacuum and direction to skeleton to tell how close the skeleton is 
            float dot = to_skel_x * dir_x + to_skel_y * dir_y;

            if (dot > 0.5f)
            {
            //if sskeleton is very close coming in the vacuum range then captur it and deactive it means that skeleton will dissapear
                if (distance_sq < capture_distance_sq)
                {
                    skel_active[i] = false;
                    continue;
                }
 
               //if skeleton if within the range of vacumm then pull it towards the player which is controlled by suck strength
                skel_x[i] -= to_skel_x * suck_strength;
                skel_y[i] -= to_skel_y * suck_strength;
            }
        }
    }
}


// Draws the level: background (already drawn externally) and blocks where lvl[][] == '#'
void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
    window.draw(bgSprite);

    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            if (lvl[i][j] == '#')
            {
                // position block sprite at tile coordinates
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
    char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
    char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

    if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
    {
        onGround = true;
    }
    else
    {
        // no ground -> apply new vertical offset
        player_y = offset_y;
        onGround = false;
    }

    if (!onGround)
    {
        velocityY += gravity; // accelerate downward
        if (velocityY >= terminal_Velocity)
        {
            velocityY = terminal_Velocity; // limit fall speed
        }
    }
    else
    {
        velocityY = 0; // on ground -> stop vertical velocity
    }
}

// LEFT COLLISION
// Attempt to move left; if a block exists on left-edge points, do not move
void player_left_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x;     // start from current
    offset_x -= speed;       // next predicted x after moving left

    // Check three points on the left side of the player (top, middle, bottom)
    char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
    char left_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];

    if (left_top == '#' || left_mid == '#' || left_bottom == '#')
    {
        offset_x = player_x ; // collision -> don't move
    }
    else
    {
        // safe -> commit movement
        player_x = offset_x;
    }
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

    if (right_top == '#' || right_mid == '#' || right_bottom == '#')
    {
        offset_x = player_x ; // don't move into block
    }
    else
    {
        player_x = offset_x; // commit movement
    }
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

    if (top_left == '#' || top_mid == '#' || top_right == '#')
    {
        velocityY = 0; // hit ceiling -> stop upward speed (start falling afterwards)
    }
    else
    {
        player_y = offset_y; // safe -> commit new Y
    }
}

int main()
{
    // Create window
    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    //level specifics
    const int cell_size = 64;  // tile dimension in pixels
    const int height = 14;     // tiles vertically
    const int width = 18;      // tiles horizontally
    char** lvl;                // 2D dynamic array representing map cells

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
    const float gravity = 1;        // gravity per frame
    bool onGround = false;

    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;

    float terminal_Velocity = 20;   // clamp falling speed

    int PlayerHeight = 102;
    int PlayerWidth = 96;

    Texture PlayerTexture;
    Sprite PlayerSprite;

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
    const int playerOptionsCount = 2;   // number of characters
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
    // Set positions and scale for menu display
    for(int i = 0; i < playerOptionsCount; i++)
    {
    bagOptionSprite[i].setTexture(bagOptions[i]);
    bagOptionSprite[i].setScale(2,2);
    bagOptionSprite[i].setPosition(300 + i*300+73, 300+41);
        playerOptionSprite[i].setTexture(playerOptions[i]);
        playerOptionSprite[i].setScale(3,3);
        playerOptionSprite[i].setPosition(300 + i*300, 300); // spacing
    }

    // Selection variables
    int selectedIndex = 0;
    bool playerChosen = false;
    menuMusic.play(); // play menu music

    // Simple menu loop (blocks until player chosen)
    while(!playerChosen && window.isOpen())
    {
        Event ev;
        while(window.pollEvent(ev))
        {
            if(ev.type == Event::Closed) window.close();

            if(ev.type == Event::KeyPressed)
            {
                if(ev.key.code == Keyboard::Right)
                {
                    selectedIndex = (selectedIndex + 1) % playerOptionsCount;
                }
                if(ev.key.code == Keyboard::Left)
                {
                    selectedIndex = (selectedIndex - 1 + playerOptionsCount) % playerOptionsCount;
                }
                if(ev.key.code == Keyboard::Enter)
                {
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

        window.clear();
        // highlight selected option
        for(int i = 0; i < playerOptionsCount; i++)
        {
            if(i == selectedIndex) playerOptionSprite[i].setColor(Color::Yellow);
            else 
            playerOptionSprite[i].setColor(Color::White);

window.draw(bagOptionSprite[i]);
            window.draw(playerOptionSprite[i]);
        }
        window.display();
    }

    bool isJumping = false;  // Track if jumping (unused now but available)

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
    vacuumLeftTex.loadFromFile("left.png");  // default player with vacuum left
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
    if(laserLoaded)
    {
        laserSprite.setTexture(laserTex);
    }

    //creating level array (dynamic 2D char array)
    lvl = new char* [height];
    for (int i = 0; i < height; i += 1)
    {
        lvl[i] = new char[width];
    }
    lvlMusic.play();
    lvlMusic.setLoop(true);

    // Initialize level cells to empty space ' '
    for(int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            lvl[i][j] = ' ';
        }
    }

    // ====== Hard-coded blocks (level layout) ======
    /////  BOTTOM BLOCKS
    for (int j = 0; j < width; ++j) lvl[8][j] = '#'; // you can replace with loop like this
    ////  BOTTOM UPPER PART
    lvl[5][6] = '#'; lvl[5][7] = '#'; lvl[5][8] = '#';
    lvl[5][9] = '#'; lvl[5][10] = '#'; lvl[5][11] = '#';

    ////  LEFT SIDE wall
    for(int i = 0; i <=7; ++i) lvl[i][0] = '#';

    ////  RIGHT PART wall
    for(int i = 0; i <=7; ++i) lvl[i][17] = '#';

    ////  RIGHT FORWARD PART (these were set originally, maybe misnamed)
    lvl[3][0] = '#'; lvl[3][1] = '#'; lvl[3][2] = '#'; lvl[3][3] = '#';

    ////  LEFT FORWARD PART
    lvl[3][14] = '#'; lvl[3][15] = '#'; lvl[3][16] = '#'; lvl[3][17] = '#';

    ///  UPPER PART (top ceiling)
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
    bool ghost_active[8];  // active flag per ghost
    Texture ghostTexture;
    ghostTexture.loadFromFile("gost.png");
    //spawning ghosts
    srand(time(0)); // seed RNG

    for(int i = 0; i < ghosts; i++)
    {
        // pick random tile that is empty and has ground below
        while(true)
        {
            int tx = rand() % width;
            int ty = rand() % (height - 1);

            if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
            {
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
        ghostSprite[i].setTexture(ghostTexture);
        ghostSprite[i].setScale(2, 2);   // increase ghost size

        ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
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
    skelTexture.loadFromFile("skeleton.png");
    //spawning skeletons
    srand(time(0)); // re-seed (fine but unnecessary)

    for(int i = 0; i < skel; i++)
    {
        // pick random platform tile (empty + ground below)
        while(true)
        {
            int tx = rand() % width;
            int ty = rand() % (height - 1);

            if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
            {
                skel_x[i] = tx * cell_size;
                skel_y[i] = ty * cell_size;
                break;
            }
        }

        skel_speed[i] = 1;
        skel_dir[i] = (rand() % 2 == 0) ? -1 : 1;
        skel_velocityY[i]=0;
        skel_onGround[i]=false;
        skel_active[i]=true;
        skelSprite[i].setTexture(skelTexture);
        skelSprite[i].setScale(2, 2);

        skelSprite[i].setPosition(skel_x[i], skel_y[i]);
    }

    Event ev;
    bool xKeyPressed=false;
    //main loop
    while (window.isOpen())
    {
        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
            {
                window.close();
            }

            if (ev.type == Event::KeyPressed)
            {
                // KeyPressed event remains for possible future logic
            }
        }

        // pressing escape to close
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        window.clear();

        // draw level and apply gravity to player
        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
        player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);

        ///*** // Moving the character left and right using arrow keys
        if(Keyboard::isKeyPressed(Keyboard::Key::Right ))
        {
            // attempt to move right with collision check
            player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            PlayerSprite.setScale(-3,3); // flip horizontally to face right
        }
        if(Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
            // attempt to move left
            player_left_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            PlayerSprite.setScale(3,3); // normal facing left
        }

        // Ceiling collision when moving up (velocityY < 0)
        if(velocityY<0)
        {
            player_ceiling_collision(lvl, offset_y, velocityY, player_x, player_y, cell_size, PlayerWidth);
        }

        // Jump with space if on ground
        if(Keyboard::isKeyPressed(Keyboard::Key::Space))
        {
            if(onGround)
            {
                velocityY=jumpStrength;
            }
        }

        // ============= VACUUM CONTROL WITH WASD =============
        if(Keyboard::isKeyPressed(Keyboard::Key::W))
        {
            vacuumActive = true;
            vacuumDirection = 1; // Up
        }
        else if(Keyboard::isKeyPressed(Keyboard::Key::A))
        {
            vacuumActive = true;
            vacuumDirection = 0; // Left
        }
        else if(Keyboard::isKeyPressed(Keyboard::Key::S))
        {
            vacuumActive = true;
            vacuumDirection = 3; // Down
        }
        else if(Keyboard::isKeyPressed(Keyboard::Key::D))
        {
            vacuumActive = true;
            vacuumDirection = 2; // Right
        }
        else
        {
            vacuumActive = false; // No vacuum key pressed
        }

        // ============= VACUUM SUCKING WITH X KEY =============
        xKeyPressed = Keyboard::isKeyPressed(Keyboard::Key::X);

        if(xKeyPressed && vacuumActive)
        {
            // call vacuum handler to pull enemies
            handle_vacuum_sucking(
                player_x, player_y, PlayerWidth, PlayerHeight,
                ghost_x, ghost_y, ghost_active, ghosts,
                skel_x, skel_y, skel_active, skel,
                cell_size, 4.0f, vacuumDirection
            );
        }

        // ============= DRAW PLAYER OR VACUUM =============
        if(vacuumActive)
        {
            // Draw vacuum sprite based on direction (placed at player's top-left)
            switch(vacuumDirection)
            {
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
        else
        {
            // Draw normal player sprite when vacuum is not active
            PlayerSprite.setPosition(player_x, player_y);
            
            window.draw(PlayerSprite);
        }

        // *** LASER BEAM DRAWING WHEN X IS PRESSED ***
        if(xKeyPressed && vacuumActive && laserLoaded)
        {
            float laser_x = player_x + PlayerWidth/2;
            float laser_y = player_y + PlayerHeight/2;

            // set laser rotation & scale to align with vacuum direction
            switch(vacuumDirection)
            {
                case 0: // Left
                    laserSprite.setRotation(180);
                    laserSprite.setScale(3, 2);
                    laserSprite.setPosition(laser_x, laser_y);
                    break;
                case 1: // Up
                    laserSprite.setRotation(270);
                    laserSprite.setScale(3, 2);
                    laserSprite.setPosition(laser_x, laser_y);
                    break;
                case 2: // Right
                    laserSprite.setRotation(0);
                    laserSprite.setScale(3, 2);
                    laserSprite.setPosition(laser_x, laser_y);
                    break;
                case 3: // Down
                    laserSprite.setRotation(90);
                    laserSprite.setScale(3, 2);
                    laserSprite.setPosition(laser_x, laser_y);
                    break;
            }
            window.draw(laserSprite);
        }

        // ====== GHOST MOVEMENT (NO FALLING - TURN AT EDGES) ======
        for(int i = 0; i < ghosts; i++)
        {
            if(!ghost_active[i]) continue; // skip inactive ghosts

            // Find tile coordinates for ground checks
            int bottomLeftX = ghost_x[i] / cell_size;
            int bottomRightX = (ghost_x[i] + 64) / cell_size;
            int bottomY = (ghost_y[i] + 64) / cell_size;

            // If ghost stands on a block, mark onGround true
            if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#')
            {
                ghost_onGround[i] = true;
            }

            // HORIZONTAL MOVEMENT - only if on ground
            if(ghost_onGround[i])
            {
                float nextX = ghost_x[i] + ghost_speed[i] * ghost_dir[i];

                // front tile X coordinate (depending on direction)
                int frontTileX = (nextX + (ghost_dir[i] == 1 ? 64 : 0)) / cell_size;
                int midTileY = (ghost_y[i] + 32) / cell_size;

                // edge check: if no ground ahead then turn
                int edgeCheckX = (nextX + (ghost_dir[i] == 1 ? 64 : 0)) / cell_size;
                int edgeCheckY = (ghost_y[i] + 64 + 1) / cell_size;

                // Turn around if there's a wall ahead OR no ground ahead (edge)
                if(lvl[midTileY][frontTileX] == '#' || lvl[edgeCheckY][edgeCheckX] != '#')
                {
                    ghost_dir[i] *= -1;
                }
                else
                {
                    ghost_x[i] = nextX;
                }
            }

            // update sprite position and draw
            ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
            window.draw(ghostSprite[i]);
        }

        // ====== SKELETON MOVEMENT WITH GRAVITY AND PLATFORM TELEPORT ======
        for(int j = 0; j < skel; j++)
        {
            if(!skel_active[j]) continue;  // skip inactive skeletons

            // GRAVITY - predict next vertical position
            float nextY = skel_y[j] + skel_velocityY[j];

            // Check ground collision (bottom of skeleton)
            int bottomLeftX = skel_x[j] / cell_size;
            int bottomRightX = (skel_x[j] + 64) / cell_size;
            int bottomY = (nextY + 64) / cell_size;

            if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#')
            {
                // landed on platform
                skel_onGround[j] = true;
                skel_velocityY[j] = 0;
            }
            else
            {
                // falling
                skel_onGround[j] = false;
                skel_y[j] = nextY;
                skel_velocityY[j] += gravity;
                if(skel_velocityY[j] > terminal_Velocity)
                {
                    skel_velocityY[j] = terminal_Velocity;
                }
            }

            // HORIZONTAL MOVEMENT - only move left/right if on ground
            if(skel_onGround[j])
            {
                // RANDOM TELEPORT TO PLATFORM - rare chance per frame
                if(rand() % 200 == 0)
                {
                    // Find a random valid platform position (attempts limit prevents infinite loop)
                    int attempts = 0;
                    while(attempts < 50)
                    {
                        int tx = rand() % width;
                        int ty = rand() % (height - 1);

                        // Check if it's a valid platform (empty space with ground below)
                        if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
                        {
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
                if(lvl[midTileY][frontskelX] == '#')
                {
                    skel_dir[j] *= -1;
                }
                else
                {
                    skel_x[j] = nextp;
                }
            }

            skelSprite[j].setPosition(skel_x[j], skel_y[j]);
            window.draw(skelSprite[j]);
        }

        // Present everything drawn this frame
        window.display();
    }

    //stopping music and deleting level array (cleanup)
    lvlMusic.stop();
    for (int i = 0; i < height; i++)
    {
        delete[] lvl[i];
    }
    delete[] lvl;

    return 0;
}


