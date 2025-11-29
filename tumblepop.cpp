#include <iostream>
#include <fstream>
#include <ctime>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

// Global screen dimensions
int screen_x = 1136;
int screen_y = 896;


// *** ADD THIS ENTIRE FUNCTION HERE ***
void handle_vacuum_sucking(
    float& player_x, float& player_y, int& PlayerWidth, int& PlayerHeight,
    float ghost_x[], float ghost_y[], bool ghost_active[], int ghosts,
    float skel_x[], float skel_y[], bool skel_active[], int skel,
    const int cell_size, const float suck_strength, int vacuumDirection)
{
    const float vacuum_range = 200.0f;
    const float capture_distance_sq = 500.0f;
    const float cone_angle = 60.0f;

    float player_center_x = player_x + PlayerWidth / 2.0f;
    float player_center_y = player_y + PlayerHeight / 2.0f;

    float dir_x = 0, dir_y = 0;
    switch (vacuumDirection)
    {
        case 0: dir_x = -1; dir_y = 0; break;  // Left
        case 1: dir_x = 0; dir_y = -1; break;  // Up
        case 2: dir_x = 1; dir_y = 0; break;   // Right
        case 3: dir_x = 0; dir_y = 1; break;   // Down
    }

    // === Ghost Sucking ===
    for (int i = 0; i < ghosts; i++)
    {
        if (!ghost_active[i])
        {
            continue;
        }

        float ghost_center_x = ghost_x[i] + cell_size / 2.0f;
        float ghost_center_y = ghost_y[i] + cell_size / 2.0f;

        float dx = ghost_center_x - player_center_x;
        float dy = ghost_center_y - player_center_y;

        float distance_sq = dx * dx + dy * dy;

        if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f)
        {
            float distance = sqrt(distance_sq);
            
            float to_ghost_x = dx / distance;
            float to_ghost_y = dy / distance;

            float dot = to_ghost_x * dir_x + to_ghost_y * dir_y;

            if (dot > 0.5f)
            {
                if (distance_sq < capture_distance_sq)
                {
                    ghost_active[i] = false;
                    continue;
                }

                ghost_x[i] -= to_ghost_x * suck_strength;
                ghost_y[i] -= to_ghost_y * suck_strength;
            }
        }
    }

    // === Skeleton Sucking ===
    for (int i = 0; i < skel; i++)
    {
        if (!skel_active[i])
        {
            continue;
        }

        float skel_center_x = skel_x[i] + cell_size / 2.0f;
        float skel_center_y = skel_y[i] + cell_size / 2.0f;

        float dx = skel_center_x - player_center_x;
        float dy = skel_center_y - player_center_y;

        float distance_sq = dx * dx + dy * dy;

        if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f)
        {
            float distance = sqrt(distance_sq);
            
            float to_skel_x = dx / distance;
            float to_skel_y = dy / distance;

            float dot = to_skel_x * dir_x + to_skel_y * dir_y;

            if (dot > 0.5f)
            {
                if (distance_sq < capture_distance_sq)
                {
                    skel_active[i] = false;
                    continue;
                }

                skel_x[i] -= to_skel_x * suck_strength;
                skel_y[i] -= to_skel_y * suck_strength;
            }
        }
    }
}
// *** END OF FUNCTION ***


void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
    window.draw(bgSprite);

    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {

            if (lvl[i][j] == '#')
            {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
        }
    }
}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
    offset_y = player_y;

    offset_y += velocityY;

    // Check three points below the player (left, center, right) for collision
    char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
    char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
    char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

    if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
    {
        onGround = true;
    }
    else
    {
        player_y = offset_y;
        onGround = false;
    }

    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY >= terminal_Velocity)
        {
            velocityY = terminal_Velocity;
        }
    }
    else
    {
        velocityY = 0;
    }
}

// LEFT COLLISION
// this function move the player left until the blacks comes
void player_left_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x; // current position is at 500
    offset_x -= speed;   // moves by 5 pixels like 500-5=295

    // Check three points on the left side of the player (top, middle, bottom)
    char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
    char left_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];

    // check if it hits any of the block if yes then
    if (left_top == '#' || left_mid == '#' || left_bottom == '#')
    {
        offset_x = player_x ; // stays at original position no moving
    }
    else
    {
        // can move left
        player_x = offset_x;
    }
}

// RIGHT COLLISION
void player_right_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x; // origianl position at 500
    offset_x += speed;  // next position +5

    // Check three points on the right side of the player (top, middle, bottom)
    char right_top = lvl[(int)(player_y) / cell_size][(int)(offset_x + Pwidth) / cell_size];
    char right_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x + Pwidth) / cell_size];
    char right_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x + Pwidth) / cell_size];

    // check if player hits the blocks
    if (right_top == '#' || right_mid == '#' || right_bottom == '#')
    {
        // if yes dont move stay on origional posiiton
        offset_x = player_x ;
    }
    else
    {
        // if no collision contionue moving
        player_x = offset_x;
    }
}

// CEILING COLLISION this prevents from going far beyound the upper part ceiling one
void player_ceiling_collision(char** lvl, float& offset_y, float& velocityY, float& player_x, float& player_y, const int cell_size, int& Pwidth)
{
    offset_y = player_y;
    offset_y += velocityY;  // velocity is neg when moving upward

    // Check three points on the top of the player (left, middle, right)
    char top_left = lvl[(int)(offset_y) / cell_size][(int)(player_x) / cell_size];
    char top_mid = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth/2) / cell_size];
    char top_right = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth) / cell_size];

    // if any of the three points matches means player head hits the celing
    if (top_left == '#' || top_mid == '#' || top_right == '#')
    {
        // if yes then stop the upward movement and now the player comes down
        velocityY = 0;
    }
    else
    {
        // if no collision then player can continue moving up
        player_y = offset_y;
    }
}

int main()
{

    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    //level specifics
    const int cell_size = 64;
    const int height = 14;
    const int width = 18;
    char** lvl;

    //level and background textures and sprites
    Texture bgTex;
    Sprite bgSprite;
    Texture blockTexture;
    Sprite blockSprite;

    bgTex.loadFromFile("bg.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);

    blockTexture.loadFromFile("block1.png");
    blockSprite.setTexture(blockTexture);



    //player data
    float player_x = 500;
    float player_y = 150;

    float speed = 5;

    const float jumpStrength = -15;
    const float gravity = 1;
    bool onGround = false;

    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;

    float terminal_Velocity = 20;

    int PlayerHeight = 102;
    int PlayerWidth = 96;

    Texture PlayerTexture;
    Sprite PlayerSprite;

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

    // Load textures for each character
    playerOptions[0].loadFromFile("player1.png");
    playerOptions[1].loadFromFile("player2.png");

    // Set positions and scale for menu display
    for(int i = 0; i < playerOptionsCount; i++)
    {
        playerOptionSprite[i].setTexture(playerOptions[i]);
        playerOptionSprite[i].setScale(3,3);
        playerOptionSprite[i].setPosition(300 + i*300, 300); // spacing
        
    }

    // Selection variables
    int selectedIndex = 0;
    bool playerChosen = false;
    menuMusic.play();

    while(!playerChosen && window.isOpen())
    {
        Event ev;
        while(window.pollEvent(ev))
        {
            if(ev.type == Event::Closed)
            {
                window.close();
            }
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
        
        PlayerTexture = playerOptions[selectedIndex];
        PlayerSprite.setTexture(PlayerTexture);
        PlayerSprite.setScale(3,3);
        PlayerSprite.setPosition(player_x, player_y);

        window.clear();
        for(int i = 0; i < playerOptionsCount; i++)
        {
            if(i == selectedIndex)
            {
                playerOptionSprite[i].setColor(Color::Yellow);
            }
            else
            {
                playerOptionSprite[i].setColor(Color::White);
            }
            window.draw(playerOptionSprite[i]);
        }
        window.display();
    }

    bool isJumping = false;  // Track if jumping

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

    // Setup vacuum sprites
    vacuumLeftSprite.setTexture(vacuumLeftTex);
    vacuumLeftSprite.setScale(3, 3);

    vacuumUpSprite.setTexture(vacuumUpTex);
    vacuumUpSprite.setScale(3, 3);

    vacuumRightSprite.setTexture(vacuumRightTex);
    vacuumRightSprite.setScale(3, 3);

    vacuumDownSprite.setTexture(vacuumDownTex);
    vacuumDownSprite.setScale(3, 3);

    // *** ADD THESE LINES HERE ***
    // LASER BEAM SETUP
    Texture laserTex;
    Sprite laserSprite;
    bool laserLoaded = laserTex.loadFromFile("lazer.png");
    if(laserLoaded)
    {
        laserSprite.setTexture(laserTex);
    }

    //creating level array
    lvl = new char* [height];
    for (int i = 0; i < height; i += 1)
    {
        lvl[i] = new char[width];
    }
    lvlMusic.play();
    lvlMusic.setLoop(true);
           //lvl can contain any garbage value so the collision deteection can become unpredictable
           // so we make  an aray and go thriugh rows and colum and set each of the part emprty
          for(int i = 0; i < height; i += 1)
           {
             for (int j = 0; j < width; j += 1)
               {
                 lvl[i][j] = ' ';  
               }
           }


    /////  BOTTOM BLOCKS
    lvl[8][0] = '#';
    lvl[8][1] = '#';
    lvl[8][2] = '#';
    lvl[8][3] = '#';
    lvl[8][4] = '#';
    lvl[8][5] = '#';
    lvl[8][6] = '#';
    lvl[8][7] = '#';
    lvl[8][8] = '#';
    lvl[8][9] = '#';
    lvl[8][10] = '#';
    lvl[8][11] = '#';
    lvl[8][12] = '#';
    lvl[8][13] = '#';
    lvl[8][14] = '#';
    lvl[8][15] = '#';
    lvl[8][16] = '#';
    lvl[8][17] = '#';

    ////  BOTTOM UPPER PART
    lvl[5][6] = '#';
    lvl[5][7] = '#';
    lvl[5][8] = '#';
    lvl[5][9] = '#';
    lvl[5][10] = '#';
    lvl[5][11] = '#';



    ////  LEFT SIDE
    lvl[0][0] = '#';
    lvl[1][0] = '#';
    lvl[2][0] = '#';
    lvl[3][0] = '#';
    lvl[4][0] = '#';
    lvl[5][0] = '#';
    lvl[6][0] = '#';
    lvl[7][0] = '#';

    ////  RIGHT PART
    lvl[0][17] = '#';
    lvl[1][17] = '#';
    lvl[2][17] = '#';
    lvl[3][17] = '#';
    lvl[4][17] = '#';
    lvl[5][17] = '#';
    lvl[6][17] = '#';
    lvl[7][17] = '#';


    ////  RIGHT FORWARD PART
    lvl[3][0] = '#'; // This section seems inconsistent with its name (RIGHT FORWARD PART starts at x=0)
    lvl[3][1] = '#';
    lvl[3][2] = '#';
    lvl[3][3] = '#';

    ////  LEFT FORWARD PART
    lvl[3][14] = '#';
    lvl[3][15] = '#';
    lvl[3][16] = '#';
    lvl[3][17] = '#'; // This section seems inconsistent with its name (LEFT FORWARD PART ends at x=17)

    ///  UPPER PART
    lvl[0][0] = '#';
    lvl[0][1] = '#';
    lvl[0][2] = '#';
    lvl[0][3] = '#';
    lvl[0][4] = '#';
    lvl[0][5] = '#';
    lvl[0][6] = '#';
    lvl[0][7] = '#';
    lvl[0][8] = '#';
    lvl[0][9] = '#';
    lvl[0][10] = '#';
    lvl[0][11] = '#';
    lvl[0][12] = '#';
    lvl[0][13] = '#';
    lvl[0][14] = '#';
    lvl[0][15] = '#';
    lvl[0][16] = '#';
    lvl[0][17] = '#';
    // ================= GHOST SETUP =================
    const int ghosts = 8;

    float ghost_x[8];
    float ghost_y[8];
    float ghost_speed[8];
    int ghost_dir[8];
    float ghost_velocityY[8];
    bool ghost_onGround[8];
    Sprite ghostSprite[8];
    bool ghost_active[8];  // *** ADD THIS LINE ***
    Texture ghostTexture;
    ghostTexture.loadFromFile("gost.png");
    //spawning ghostts
    srand(time(0));

    for(int i = 0; i < ghosts; i++)
    {
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
        ghost_dir[i] = (rand() % 2 == 0) ? -1 : 1;
        ghost_velocityY[i]=0;
        ghost_onGround[i]=false;
        ghost_active[i]=true;
        ghostSprite[i].setTexture(ghostTexture);
        ghostSprite[i].setScale(2, 2);   // increase ghost size

        ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
    }
    // skeletons
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
    //spawning skeleltom
    srand(time(0));

    for(int i = 0; i < skel; i++)
    {
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
        skelSprite[i].setScale(2, 2);   // increase ghost size

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
                // KeyPressed event is empty but remains for logic preservation
            }

        }

        //presing escape to close
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        window.clear();

        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
        player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);

        ///*** // Moving the character left and right using arrow keys

        if(Keyboard::isKeyPressed(Keyboard::Key::Right ))
        {
            player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            PlayerSprite.setScale(-3,3);
            // the player png pixels are 32x34 but game is 96x102 so we need to increase it by 3
            // player face on right as player is already facing left thats why we use neg sign to flip its face
        }
        if(Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
            player_left_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            PlayerSprite.setScale(3,3); // player faces left
        }
     
        // Check ceiling collision when moving upward
        // alo velocity is neg when moving upward
                      if(velocityY<0)
                       {
                           player_ceiling_collision(lvl, offset_y, velocityY, player_x, player_y, cell_size, PlayerWidth);
                        }
     
        ///***
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
            handle_vacuum_sucking(
                player_x, player_y, PlayerWidth, PlayerHeight,
                ghost_x, ghost_y, ghost_active, ghosts,
                skel_x, skel_y, skel_active, skel,
                cell_size, 4.0f, vacuumDirection
            );
        }
        // ====================================================
        // ========================================
                       
                     
        // ============= DRAW PLAYER OR VACUUM =============
        if(vacuumActive)
        {
            // Draw vacuum sprite based on direction
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


        // *** ADD LASER BEAM CODE HERE - RIGHT AFTER PLAYER/VACUUM DRAWING ***
        // ============= DRAW LASER BEAM WHEN X IS PRESSED =============
        if(xKeyPressed && vacuumActive && laserLoaded)
        {
            float laser_x = player_x + PlayerWidth/2;
            float laser_y = player_y + PlayerHeight/2;
            
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
        // =============================================================

        // ====== GHOST MOVEMENT (NO FALLING - TURN AT EDGES) ======

        for(int i = 0; i < ghosts; i++)
        {
            if(!ghost_active[i])
            {
                continue;
            }
            
            // Check if on ground
            int bottomLeftX = ghost_x[i] / cell_size;
            int bottomRightX = (ghost_x[i] + 64) / cell_size;
            int bottomY = (ghost_y[i] + 64) / cell_size;
            
            if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#')
            {
                ghost_onGround[i] = true;
            }
            
            // HORIZONTAL MOVEMENT - only if on ground
            if(ghost_onGround[i])
            {
                float nextX = ghost_x[i] + ghost_speed[i] * ghost_dir[i];
                
                // Check for wall ahead
                int frontTileX = (nextX + (ghost_dir[i] == 1 ? 64 : 0)) / cell_size;
                int midTileY = (ghost_y[i] + 32) / cell_size;
                
                // Check for edge ahead (no ground in front)
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
            
            ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
            window.draw(ghostSprite[i]);
        }

        // ====== SKELETON MOVEMENT WITH GRAVITY AND PLATFORM TELEPORT ======
        for(int j = 0; j < skel; j++)
        {
            if(!skel_active[j])
            {
                continue;  // FIXED: Changed from if(skel_active[j])
            }
            
            // GRAVITY - Apply falling
            float nextY = skel_y[j] + skel_velocityY[j];
            
            // Check ground collision (bottom of skeleton)
            int bottomLeftX = skel_x[j] / cell_size;
            int bottomRightX = (skel_x[j] + 64) / cell_size;
            int bottomY = (nextY + 64) / cell_size;
            
            if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#')
            {
                skel_onGround[j] = true;
                skel_velocityY[j] = 0;
            }
            else
            {
                skel_onGround[j] = false;
                skel_y[j] = nextY;
                skel_velocityY[j] += gravity;
                if(skel_velocityY[j] > terminal_Velocity)
                {
                    skel_velocityY[j] = terminal_Velocity;
                }
            }
            
            // HORIZONTAL MOVEMENT - Only move left/right if on ground
            if(skel_onGround[j])
            {
                // RANDOM TELEPORT TO PLATFORM - 0.5% chance per frame
                if(rand() % 200 == 0)
                {
                    // Find a random valid platform position
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

        // ================================================
        window.display();
    }

    //stopping music and deleting level array
    lvlMusic.stop();
    for (int i = 0; i < height; i++)
    {
        delete[] lvl[i];
    }
    delete[] lvl;

    return 0;
}

