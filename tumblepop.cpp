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

// Get character-specific speed multiplier
// characterIndex: 0 = green (1.5x speed), 1 = yellow (1.2x speed)
// baseSpeed: the normal movement speed
// Returns: modified speed based on character
float get_character_speed(int characterIndex, float baseSpeed)
{
    if(characterIndex == 0)
    {
        return baseSpeed * 1.5f;  // Green character - faster
    }
    else if(characterIndex == 1)
    {
        return baseSpeed * 1.2f;  // Yellow character - medium speed
    }
    return baseSpeed;  // Default speed
}

// Vacuum and Shooting

void update_vacuum(float player_x, float player_y, int vacuumDirection, float vacuum_range, float suck_strength,
    float ghost_x[], float ghost_y[], bool ghost_active[], bool ghost_stunned[], float ghost_stun_timer[], int ghosts, float skel_x[], float skel_y[], bool skel_active[], bool skel_stunned[], float skel_stun_timer[], int skel, float invis_x[], float invis_y[], bool invis_active[], bool invis_stunned[], float invis_stun_timer[], int invis_count, float chelnov_x[], float chelnov_y[], bool chelnov_active[], bool chelnov_stunned[], float chelnov_stun_timer[], int chelnov_count,
bool chelnov_is_shooting[], const int cell_size, bool vacuum_on, int captured[], int &cap_count, int max_capacity, int &score);


    void generate_random_slanted_platform(char** lvl, int height, int width, bool clearOld);
    
    
void shoot_single_enemy(float player_x, float player_y, int vacuum_dir, int captured[], int& cap_count, float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], int shot_enemy_type[], bool shot_is_active[], int& shot_count);


void shoot_burst_mode(float player_x, float player_y, int vacuum_dir, int captured[], int& cap_count, float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], int shot_enemy_type[], bool shot_is_active[], int& shot_count);


void update_projectiles(float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], bool shot_is_active[], float shot_lifetime[], float deltaTime, float max_lifetime, int shot_count, char** lvl, int cell_size, int height);


bool check_projectile_hits(float shot_enemy_x[], float shot_enemy_y[], bool shot_is_active[], int shot_count,float ghost_x[], float ghost_y[], bool ghost_active[], int total_ghosts,float skel_x[], float skel_y[], bool skel_active[], int total_skels,float invis_x[], float invis_y[], bool invis_active[], int total_invis, float chelnov_x[], float chelnov_y[], bool chelnov_active[], int total_chelnov, int& hit_projectile, int& hit_enemy_index, int& hit_enemy_type);

   
// Level 2 helper functions
bool is_platform_reachable(char** lvl, int x, int y, int width, int height);


// Check if all enemies are defeated then level  completed
// here ghost_active[] and skel_active[] means alivee
bool check_level_complete(bool ghost_active[], int total_ghosts,bool skel_active[], int total_skels,bool invis_active[], int total_invis, bool chelnov_active[], int total_chelnov)
{
    for(int i = 0; i < total_ghosts; i++)
    {
        if(ghost_active[i]) return false;
    }
   
    for(int i = 0; i < total_skels; i++)
    {
        if(skel_active[i]) return false;
    }
   
    for(int i = 0; i < total_invis; i++)
    {
        if(invis_active[i]) return false;
    }
   
    for(int i = 0; i < total_chelnov; i++)
    {
        if(chelnov_active[i]) return false;
    }
   
    return true;
}


bool is_platform_reachable(char** lvl, int x, int y, int width, int height)
{
    if (y <= 0 || lvl[y-1][x] == '#') return false;
   
    bool has_neighbor = false;
    if (x > 0 && lvl[y][x-1] == '#') has_neighbor = true;
    if (x < width-1 && lvl[y][x+1] == '#') has_neighbor = true;
    if (y < height-1 && lvl[y+1][x] == '#') has_neighbor = true;
   
    return has_neighbor || y == height-2;
    }


// Next move to level 2
// char** lvl represents 2D aray of map
// int height, int width are screen height and width

void change_to_level2(char** lvl, int height, int width)
{ // Clear entire level
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            lvl[i][j] = ' ';
        }
    }
   
    // Bottom floor (row 8)
    for(int j = 0; j < width; ++j)
        lvl[height-6][j] = '#';
   
    // Left wall
    for(int i = 0; i < height-6; ++i)
        lvl[i][0] = '#';
   
    // Right wall
    for(int i = 0; i < height-6; ++i)
        lvl[i][width-1] = '#';
   
    // Top ceiling (row 0)
    for(int j = 0; j < width; ++j)
        lvl[0][j] = '#';
   
   
    generate_random_slanted_platform(lvl, height, width, false);
   
  
}
void generate_random_slanted_platform(char** lvl, int height, int width, bool clearOld)
{
    if(clearOld)
    {
        for(int i = 2; i < height-6; i++)
        {
            for(int j = 2; j < width-2; j++)
            {
                if(lvl[i][j] == '#' || lvl[i][j] == '/' || lvl[i][j] == '\\')
                {
                    lvl[i][j] = ' ';
                }
            }
        }
    }
   
    // STEP 2: DEFINE RAMP PROPERTIES
    int length = 5;
    int attempts = 0;
    bool placed = false;
   
    // STEP 3 & 4: TRY TO PLACE AND VALIDATE
    while(attempts < 100 && !placed)
    {
        int direction = rand() % 2;
        int start_x, start_y;
       
        start_y = 6 + (rand() % 2);
       
        if (direction == 0) { // / Ramp (Up-Right)
            start_x = 4 + rand() % 3;
        } else { // \ Ramp (Up-Left)
            start_x = 10 + rand() % 3;
        }
       
        bool valid = true;
       
        for(int i = 0; i < length; i++)
        {
            int y = start_y - i;
           
         
            int x_slope = (direction == 0) ? start_x + i : start_x - i;
            int x_block = (direction == 0) ? x_slope + 1 : x_slope - 1;  // SWAPPED!
           
            // Bounds Check
            if(x_slope <= 0 || x_slope >= width-1 || x_block <= 0 || x_block >= width-1 || y <= 0 || y >= height-6)
            {
                valid = false;
                break;
            }
            // Check if both tiles are empty
            if (lvl[y][x_slope] != ' ' || lvl[y][x_block] != ' ')
            {
                valid = false;
                break;
            }
        }
       
      
        if(valid)
        {
            char slope_char = (direction == 0) ? '/' : '\\';
           
            for(int i = 0; i < length; i++)
            {
                int y = start_y - i;
               
               
               
                int x_slope = (direction == 0) ? start_x + i : start_x - i;
                int x_block = (direction == 0) ? x_slope + 1 : x_slope - 1;  // SWAPPED!
               
               
                lvl[y][x_slope] = slope_char;
                lvl[y][x_block] = '#';
            }
           
            placed = true;
            
        }
       
        attempts++;
    }
   
    // ===== STEP 6: BACKUP PLATFORM IF FAILED =====
    if(!placed)
    {
       
        int backup_y = 4;
        for(int j = 7; j <= 10; ++j) {
            lvl[backup_y][j] = '#';
        }
    }
}


// Next display level 2 window
// window this displays SFML window
// height and width are screen dmensions and cell_size is size of screen in pixels
void display_level(RenderWindow& window, char**lvl, Texture& bgTex, Sprite& bgSprite,
                   Texture& blockTexture, Sprite& blockSprite,
                   const int height, const int width, const int cell_size)
{
    window.draw(bgSprite);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Draw normal solid blocks
            if (lvl[i][j] == '#')
            {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
            // Draw UP-RIGHT slope (/) using stair-step blocks
            else if (lvl[i][j] == '/')
            {
                // Use your existing blockSprite, just scaled smaller!
                int mini_size = cell_size / 4;  // Each mini-block is 16x16 pixels
               
                blockSprite.setScale(0.25f, 0.25f);  // Make it 1/4 size
               
                // Bottom row (4 blocks)
                for(int k = 0; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 48);
                    window.draw(blockSprite);
                }
               
                // Third row (3 blocks)
                for(int k = 1; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 32);
                    window.draw(blockSprite);
                }
               
                // Second row (2 blocks)
                for(int k = 2; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 16);
                    window.draw(blockSprite);
                }
               
                // Top row (1 block)
                blockSprite.setPosition(j * cell_size + 48, i * cell_size);
                window.draw(blockSprite);
               
                // Reset scale back to normal for regular blocks
                blockSprite.setScale(1.0f, 1.0f);
            }
            // Draw UP-LEFT slope (\) using stair-step blocks
            else if (lvl[i][j] == '\\')
            {
                int mini_size = cell_size / 4;  // Each mini-block is 16x16 pixels
               
                blockSprite.setScale(0.25f, 0.25f);  // Make it 1/4 size
               
                // Bottom row (4 blocks)
                for(int k = 0; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 48);
                    window.draw(blockSprite);
                }
               
                // Third row (3 blocks - left side)
                for(int k = 0; k < 3; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 32);
                    window.draw(blockSprite);
                }
               
                // Second row (2 blocks - left side)
                for(int k = 0; k < 2; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 16);
                    window.draw(blockSprite);
                }
               
                // Top row (1 block - left corner)
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
               
                // Reset scale back to normal
                blockSprite.setScale(1.0f, 1.0f);
            }
        }
    }
}

// This functioin is to check if player is not standing on the blocks then it must fall
// char** lvl is a 2D array for map
// player_x and player_y are the current horizontal and vertical position of player
// Pheight and Pwidth are players height and wdth in pixels of players collision with block
// offset_y is the prediction of players vertical position
// speed = players constant speed
// terminal_Velocity is the maximum downward speed that the player can achieve
// onGround checks if player is on the ground
void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
    int col_mid = (player_x + Pwidth / 2) / cell_size;
int row_feet = (player_y + Pheight) / cell_size;

// 2. Define the tiles around the player's feet
char tile_center = lvl[row_feet][col_mid];
char tile_left = lvl[row_feet][col_mid - 1];
char tile_right = lvl[row_feet][col_mid + 1];

// 3. Check for SLIDING condition (override if feet touch a sliding surface OR are next to one)
if (row_feet >= 0 && row_feet < Pheight && col_mid >= 1 && col_mid < Pwidth - 1)
{
    if (tile_center == '/' || tile_left == '/' || tile_right == '/')
    {
        // Detected Up-Right slope (/) anywhere near feet. Must slide LEFT.
        player_x -= 2.5f; // FIX: Slide Left (Downhill)
        velocityY = 1.5f;
        onGround = false;
        player_y += velocityY;
        return;
    }
    else if (tile_center == '\\' || tile_left == '\\' || tile_right == '\\')
    {
        // Detected Up-Left slope (\) anywhere near feet. Must slide RIGHT.
        player_x += 2.5f; // FIX: Slide Right (Downhill)
        velocityY = 1.5f;
        onGround = false;
        player_y += velocityY;
        return;
    }
}
    // Apply gravity first (increase downward velocity)
    if (!onGround)
    {
        // increse velocity so player falls faster
        velocityY += gravity;
        // Prevent falling jut too fast by putting it eual to terminal velocity
        if (velocityY >= terminal_Velocity)
        {
            velocityY = terminal_Velocity;
        }
    }
   
    // Copying current player position to offset_y
    offset_y = player_y;
    // Predicts new vertical velocity after applyong the current vertical velocity
    offset_y += velocityY;
   
    // ===== ONLY CHECK TOP BOUNDARY (ROW 0) - Player can jump through all other blocks =====
    if (velocityY < 0)  // Moving upward (jumping)
    {
        int top_y = (int)(offset_y) / cell_size;
       
        // Only check if trying to go into or above row 0 (top ceiling)
        if(top_y <= 0)
        {
            int left_x = (int)(player_x) / cell_size;
            int mid_x = (int)(player_x + Pwidth / 2) / cell_size;
            int right_x = (int)(player_x + Pwidth) / cell_size;
           
            // Check if hitting the TOP ROW blocks
            if(lvl[0][left_x] == '#' || lvl[0][mid_x] == '#' || lvl[0][right_x] == '#')
            {
                // Hit top ceiling - stop upward movement
                velocityY = 0;
                player_y = cell_size;  // Position just below row 0
                onGround = false;
                return;
            }
        }
       
        // If not hitting top row, move freely upward through other blocks
        player_y = offset_y;
        onGround = false;
        return;
    }
   
    // CRITICAL FIX: Only check GROUND collision when FALLING DOWN (velocityY > 0)
    // This allows player to jump UP through platforms without getting stuck
    if (velocityY > 0)
    {
        // Check three points below the player (left, center, right) for collision with blocks
        // for bottom collision check the predicted top value + height of player gives players bottom posiiton
        // next dividing it by cell_size to convert to pixels and finally convert to integer to discard decimal parts
        char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
        // Right sid checks bootom and right side of the player
        char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
        // for middle part check bootom and center of players width by dividing Players wdith by 2
        char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

        if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
        {
            // Hit ground - stop on platform
            onGround = true;
            velocityY = 0;
            // Don't update player_y - stay on top of block
        }
        else
        {
            // if player is not on ground then update its vertical position to move down
            player_y = offset_y;
            onGround = false;
        }
    }
    else
    {
        // When jumping UP (velocityY <= 0), move freely through blocks
        player_y = offset_y;
        onGround = false;
    }
}
















// Check player collison with left wall
// char** lvl is a 2D array for map
// player_x and player_y are the current horizontal and vertical position of player
// Pheight and Pwidth are players height and wdth in pixels of players collision with block
// offset_x is the prediction of players horizontal position
void player_left_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    // moving left if there is no wall
    offset_x = player_x;    
    offset_x -= speed;      

    // Check three points on the left side of the player (top, middle, bottom)
    char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
    char left_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];

    if (left_top == '#' || left_mid == '#' || left_bottom == '#')
    {
       // collision occurs so dont move and stay still
        offset_x = player_x ;
    }
    else
    {
        // no collisions ocuur player is safe to move left
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
        offset_x = player_x ;
    }
    else
    {
        player_x = offset_x;
    }
}

// CEILING COLLISION  this prevents moving up into blocks
void player_ceiling_collision(char** lvl, float& offset_y, float& velocityY, float& player_x, float& player_y, const int cell_size, int& Pwidth)
{
    // predicting next position
    offset_y = player_y;
    offset_y += velocityY;

    // Check three points on the top of the player (left, middle, right)
    char top_left = lvl[(int)(offset_y) / cell_size][(int)(player_x) / cell_size];
    char top_mid = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth/2) / cell_size];
    char top_right = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth) / cell_size];

    if (top_left == '#' || top_mid == '#' || top_right == '#')
    {
        velocityY = 0;
    }
    else
    {
        player_y = offset_y;
    }
}

//Check collision between player and they enemies
//PlayerWidth and PlayerHeight are players height and width
//whre enemy idth and height = enemy_size
//player_x and player_y are players top left points
//enemy_x and enemy_y are enemies top and left points
bool check_player_enemy_collision(
    float player_x, float player_y, int PlayerWidth, int PlayerHeight,
    float enemy_x, float enemy_y, int enemy_size)
{
   // checking players collision with enemy by horizontal and vertical overlapping
    return (player_x < enemy_x + enemy_size &&
            player_x + PlayerWidth > enemy_x &&
            player_y < enemy_y + enemy_size &&
            player_y + PlayerHeight > enemy_y);
}

// Respawn player from top
// player_x = players horizontal position and player_y is players vertical position
// velocityY = vertical speed meanng howfast is player movng down
// playerDead check if player is dead
void respawn_player(float& player_x, float& player_y, float& velocityY, bool& playerDead)
{
    player_x = 500;       // resets players poistion
    player_y = 150;
    velocityY = 0;       // stop falling
    playerDead = false;  // player is alive again
}
// Check power-up collision with player
bool check_powerup_collision(float player_x, float player_y, int PlayerWidth, int PlayerHeight,
                            float powerup_x, float powerup_y, int powerup_size)
{
    return (player_x < powerup_x + powerup_size &&
            player_x + PlayerWidth > powerup_x &&
            player_y < powerup_y + powerup_size &&
            player_y + PlayerHeight > powerup_y);
}

// Power-up spawning function (0=EXTRA_LIFE, 1=POWER, 2=RANGE, 3=SPEED)
void spawn_powerup(float& powerup_x, float& powerup_y, bool& powerup_active,
                   int& powerup_type, Sprite& powerup_sprite, Clock& spawn_timer,
                   bool& has_spawned, char** lvl, int width, int height,
                   int cell_size, int type, Texture& texture)
{
    if(has_spawned) return;  // Already spawned in this level
   
    powerup_type = type;
    powerup_sprite.setTexture(texture);
    powerup_sprite.setScale(2, 2);
   
    // Find random valid spawn position
    int attempts = 0;
    while(attempts < 50)
    {
        int tx = 2 + rand() % (width - 4);
        int ty = 2 + rand() % (height - 3);
       
        // Must be empty space with ground below
        if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
        {
            powerup_x = tx * cell_size;
            powerup_y = ty * cell_size;
            powerup_active = true;
            has_spawned = true;
            spawn_timer.restart();
            return;
        }
        attempts++;
    }
   
    // Fallback spawn
    powerup_x = 400;
    powerup_y = 300;
    powerup_active = true;
    has_spawned = true;
    spawn_timer.restart();
}

// ============================================================================
// FUNCTION DEFINITIONS (Vacuum and Shooting)
// ============================================================================

//This function is used for sucking ghost and skeletons
//player_x/player_y represents position of player top orleft
//PlayerWidth/PlayerHeight shows player's sprite size
//ghost_x[], ghost_y[], ghost_active[], ghosts shows ghost positions + active flags + count
//skel_x[], skel_y[], skel_active[], skel is skeletons same as ghosts
//cell_size indicates tile size to compute centers
//suck_strength shows how fast the enemies are pulled
//vacuumDirection indiactes 0=left,1=up,2=right,3=down

void update_vacuum(
    float player_x, float player_y, int vacuumDirection,
    float vacuum_range, float suck_strength,
    float ghost_x[], float ghost_y[], bool ghost_active[], bool ghost_stunned[], float ghost_stun_timer[], int ghosts,
    float skel_x[], float skel_y[], bool skel_active[], bool skel_stunned[], float skel_stun_timer[], int skel,
    float invis_x[], float invis_y[], bool invis_active[], bool invis_stunned[], float invis_stun_timer[], int invis_count,
    float chelnov_x[], float chelnov_y[], bool chelnov_active[], bool chelnov_stunned[], float chelnov_stun_timer[], int chelnov_count,
    bool chelnov_is_shooting[],  // <--- ADD THIS PARAMETER
    const int cell_size, bool vacuum_on,
    int captured[], int &cap_count, int max_capacity, int &score)
{
    if (!vacuum_on)
    {
        return;
    }

    float player_center_x = player_x + 96 / 2.0f;
    float player_center_y = player_y + 102 / 2.0f;

    float dir_x = 0, dir_y = 0;
    switch (vacuumDirection)
    {
        case 0: dir_x = -1; dir_y = 0; break;
        case 1: dir_x = 0; dir_y = -1; break;
        case 2: dir_x = 1; dir_y = 0; break;
        case 3: dir_x = 0; dir_y = 1; break;
    }

    const float capture_distance = 50.0f;

    // Process ghosts (type 0)
    for (int i = 0; i < ghosts; i++)
    {
        if (!ghost_active[i]) continue;

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
                ghost_stunned[i] = true;
                ghost_stun_timer[i] = 1.5f;

                if (distance < capture_distance && cap_count < max_capacity)
                {
                    captured[cap_count] = 0;
                    cap_count = cap_count + 1;
                    ghost_active[i] = false;
                    ghost_stunned[i] = false;
                    ghost_x[i] = -1000;
                    ghost_y[i] = -1000;
                    score = score + 50;
                     
                }
                else
                {
                    ghost_x[i] -= to_ghost_x * suck_strength;
                    ghost_y[i] -= to_ghost_y * suck_strength;
                }
            }
        }
    }

    // Process skeletons (type 1)
    for (int i = 0; i < skel; i++)
    {
        if (!skel_active[i]) continue;

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
                skel_stunned[i] = true;
                skel_stun_timer[i] = 1.5f;

                if (distance < capture_distance && cap_count < max_capacity)
                {
                    captured[cap_count] = 1;
                    cap_count = cap_count + 1;
                    skel_active[i] = false;
                    skel_stunned[i] = false;
                    skel_x[i] = -1000;
                    skel_y[i] = -1000;
                    score = score + 75;
                    
                }
                else
                {
                    skel_x[i] -= to_skel_x * suck_strength;
                    skel_y[i] -= to_skel_y * suck_strength;
                }
            }
        }
    }

    // Process Invisible Men (type 2) - NEW
    for (int i = 0; i < invis_count; i++)
    {
        if (!invis_active[i]) continue;

        float invis_center_x = invis_x[i] + cell_size / 2.0f;
        float invis_center_y = invis_y[i] + cell_size / 2.0f;

        float dx = invis_center_x - player_center_x;
        float dy = invis_center_y - player_center_y;
        float distance_sq = dx * dx + dy * dy;

        if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f)
        {
            float distance = sqrt(distance_sq);
            float to_invis_x = dx / distance;
            float to_invis_y = dy / distance;
            float dot = to_invis_x * dir_x + to_invis_y * dir_y;

            if (dot > 0.5f)
            {
                invis_stunned[i] = true;
                invis_stun_timer[i] = 1.5f;

                if (distance < capture_distance && cap_count < max_capacity)
                {
                    captured[cap_count] = 2;  // Invisible Man type
                    cap_count = cap_count + 1;
                    invis_active[i] = false;
                    invis_stunned[i] = false;
                    invis_x[i] = -1000;
                    invis_y[i] = -1000;
                    score = score + 100;
                     
                }
                else
                {
                    invis_x[i] -= to_invis_x * suck_strength;
                    invis_y[i] -= to_invis_y * suck_strength;
                }
            }
        }
    }

// Process Chelnovs (type 3) - CANNOT BE CAPTURED WHILE SHOOTING
for (int i = 0; i < chelnov_count; i++)
{
    if (!chelnov_active[i]) continue;
   
    // CRITICAL: Cannot capture during shooting phase
    if (chelnov_is_shooting[i])
    {
        continue;  // Skip this Chelnov - invincible while shooting
    }

    float chelnov_center_x = chelnov_x[i] + cell_size / 2.0f;
    float chelnov_center_y = chelnov_y[i] + cell_size / 2.0f;

    float dx = chelnov_center_x - player_center_x;
    float dy = chelnov_center_y - player_center_y;
    float distance_sq = dx * dx + dy * dy;

    if (distance_sq < vacuum_range * vacuum_range && distance_sq > 1.0f)
    {
        float distance = sqrt(distance_sq);
        float to_chelnov_x = dx / distance;
        float to_chelnov_y = dy / distance;
        float dot = to_chelnov_x * dir_x + to_chelnov_y * dir_y;

        if (dot > 0.5f)
        {
            chelnov_stunned[i] = true;
            chelnov_stun_timer[i] = 1.5f;

            if (distance < capture_distance && cap_count < max_capacity)
            {
                captured[cap_count] = 3;
                cap_count = cap_count + 1;
                chelnov_active[i] = false;
                chelnov_stunned[i] = false;
                chelnov_x[i] = -1000;
                chelnov_y[i] = -1000;
                score = score + 125;
               
            }
            else
            {
                chelnov_x[i] -= to_chelnov_x * suck_strength;
                chelnov_y[i] -= to_chelnov_y * suck_strength;
            }
        }
    }
}}

void shoot_single_enemy(float player_x, float player_y, int vacuum_dir,
                       int captured[], int& cap_count,
                       float shot_enemy_x[], float shot_enemy_y[],
                       float shot_velocity_x[], float shot_velocity_y[],
                       int shot_enemy_type[], bool shot_is_active[],
                       float shot_lifetime[],  // NEW: Add lifetime parameter
                       int& shot_count)
{
    if (cap_count <= 0)
    {
        return;
    }

    // LIFO: Pop last captured enemy
    cap_count = cap_count - 1;
    int shot_type = captured[cap_count];
   
    // Find next available projectile slot
    int slot = -1;
    for(int i = 0; i < 20; i++)
    {
        if(!shot_is_active[i])
        {
            slot = i;
            break;
        }
    }
   
    if(slot == -1) 
    {
      
        return;
    }

    
    shot_enemy_x[slot] = player_x;
    shot_enemy_y[slot] = player_y;
    shot_enemy_type[slot] = shot_type; 
    shot_is_active[slot] = true;
    shot_lifetime[slot] = 0.0f;  

    float projectile_speed = 10.0f;
    
    if (vacuum_dir == 0) // left
    {
        shot_velocity_x[slot] = -projectile_speed;
        shot_velocity_y[slot] = 0.0f;
    }
    else if (vacuum_dir == 1) // up
    {
        shot_velocity_x[slot] = 0.0f;
        shot_velocity_y[slot] = -projectile_speed;
    }
    else if (vacuum_dir == 2) // right
    {
        shot_velocity_x[slot] = projectile_speed;
        shot_velocity_y[slot] = 0.0f;
    }
    else // down (3)
    {
        shot_velocity_x[slot] = 0.0f;
        shot_velocity_y[slot] = projectile_speed;
    }
   
   
}

void shoot_burst_mode(float player_x, float player_y, int vacuum_dir,
                     int captured[], int& cap_count,
                     float shot_enemy_x[], float shot_enemy_y[],
                     float shot_velocity_x[], float shot_velocity_y[],
                     int shot_enemy_type[], bool shot_is_active[],
                     float shot_lifetime[],  // NEW: Add lifetime parameter
                     int& shot_count)
{
  
   
    
    while (cap_count > 0)
    {
        shoot_single_enemy(player_x, player_y, vacuum_dir, captured, cap_count,
                          shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                          shot_enemy_type, shot_is_active, shot_lifetime, shot_count);  // NEW: Pass lifetime
    }
   
  
}
// Updadte all projectile positions and physics
void update_projectiles(float shot_enemy_x[], float shot_enemy_y[],
                       float shot_velocity_x[], float shot_velocity_y[],
                       bool shot_is_active[], float shot_lifetime[],  // NEW: Add lifetime parameter
                       float deltaTime, float max_lifetime,  // NEW: Add deltaTime and max_lifetime
                       int shot_count,
                       char** lvl, int cell_size, int height)
{
    for (int i = 0; i < 20; i++)
    {
        if (!shot_is_active[i])
        {
            continue;
        }

        // NEW: Update lifetime
        shot_lifetime[i] += deltaTime;
       
        // NEW: Check if projectile has exceeded lifetime
        if (shot_lifetime[i] >= max_lifetime)
        {
            shot_is_active[i] = false;
           
            continue;
        }

        // Move projectile
        shot_enemy_x[i] = shot_enemy_x[i] + shot_velocity_x[i];
        shot_enemy_y[i] = shot_enemy_y[i] + shot_velocity_y[i];

        // Add gravity for horizontal shots
        if (shot_velocity_x[i] != 0.0f)
        {
            shot_velocity_y[i] = shot_velocity_y[i] + 0.3f;
        }

        // Check screen bounds
        if (shot_enemy_x[i] < 0 || shot_enemy_x[i] > 1136)
        {
            shot_is_active[i] = false;
           
            continue;
        }
       
        if (shot_enemy_y[i] < 0 || shot_enemy_y[i] > 896)
        {
            shot_is_active[i] = false;
           
            continue;
        }

        // Check platform collision
        int col = (int)(shot_enemy_x[i] / cell_size);
        int row = (int)(shot_enemy_y[i] / cell_size);

        if (row >= 0 && row < height && col >= 0 && col < 18)
        {
            if (lvl[row][col] == '#')
            {
                // Bounce off blocks
                if (shot_velocity_x[i] != 0.0f)
                {
                    shot_velocity_x[i] = -shot_velocity_x[i];
                }
                if (shot_velocity_y[i] > 0)
                {
                    shot_velocity_y[i] = -shot_velocity_y[i] * 0.8f;
                }
            }
        }
    }
}

















// Check if projectiles hit any active enemies
bool check_projectile_hits(float shot_enemy_x[], float shot_enemy_y[],
                          bool shot_is_active[], int shot_count,
                          float ghost_x[], float ghost_y[], bool ghost_active[], int total_ghosts,
                          float skel_x[], float skel_y[], bool skel_active[], int total_skels,
                          float invis_x[], float invis_y[], bool invis_active[], int total_invis,
                          float chelnov_x[], float chelnov_y[], bool chelnov_active[], int total_chelnov,
                          int& hit_projectile, int& hit_enemy_index, int& hit_enemy_type)
{
    hit_projectile = -1;
    hit_enemy_index = -1;
    hit_enemy_type = -1;

    for (int proj = 0; proj < 20; proj++)
    {
        if (!shot_is_active[proj]) continue;

        // Check ghosts (type 0)
        for (int g = 0; g < total_ghosts; g++)
        {
            if (!ghost_active[g]) continue;

            float dx = shot_enemy_x[proj] - ghost_x[g];
            float dy = shot_enemy_y[proj] - ghost_y[g];
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 48)
            {
                hit_projectile = proj;
                hit_enemy_index = g;
                hit_enemy_type = 0;
                return true;
            }
        }

        // Check skeletons (type 1)
        for (int s = 0; s < total_skels; s++)
        {
            if (!skel_active[s]) continue;

            float dx = shot_enemy_x[proj] - skel_x[s];
            float dy = shot_enemy_y[proj] - skel_y[s];
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 48)
            {
                hit_projectile = proj;
                hit_enemy_index = s;
                hit_enemy_type = 1;
                return true;
            }
        }

        // Check Invisible Men (type 2)
        for (int inv = 0; inv < total_invis; inv++)
        {
            if (!invis_active[inv]) continue;

            float dx = shot_enemy_x[proj] - invis_x[inv];
            float dy = shot_enemy_y[proj] - invis_y[inv];
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 48)
            {
                hit_projectile = proj;
                hit_enemy_index = inv;
                hit_enemy_type = 2;
                return true;
            }
        }

        // Check Chelnovs (type 3)
        for (int ch = 0; ch < total_chelnov; ch++)
        {
            if (!chelnov_active[ch]) continue;

            float dx = shot_enemy_x[proj] - chelnov_x[ch];
            float dy = shot_enemy_y[proj] - chelnov_y[ch];
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 48)
            {
                hit_projectile = proj;
                hit_enemy_index = ch;
                hit_enemy_type = 3;
                return true;
            }
        }
    }

    return false;
}
// ============================================================================
// MAIN GAME LOOP
// ============================================================================

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
    char** lvl;        // Level management variables
   
 


int currentLevel = 1;
bool levelComplete = false;
Clock levelCompleteClock;
const float levelCompleteDelay = 3.0f;         // 2D dynamic array representing map cells

    //level and background textures and sprites
    Texture bgTex;
    Sprite bgSprite;
    Texture blockTexture;
    Sprite blockSprite;

    // load background & block textures (files must exist)
    bgTex.loadFromFile("bg/bg.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);

    blockTexture.loadFromFile("bg/block1.png");
    blockSprite.setTexture(blockTexture);

    //player data (position, speed, size etc.)
    float player_x = 500;
    float player_y = 150;

    float speed = 5;

    const float jumpStrength = -20; // negative -> upward
    const float gravity = 1;        // gravity per frame
    bool onGround = false;

    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;

    float terminal_Velocity = 20;   // clamp falling speed

    int PlayerHeight = 102;
    int PlayerWidth = 96;
   
    // ===== SCORING SYSTEM =====
int score = 0;
int combo = 0;  // Track consecutive kills without taking damage
float comboMultiplier = 1.0f;
bool tookDamage = false;  // Track if player took any damage in the level

Font scoreFont;
Text scoreText;
Text comboText;

Clock levelTimer;  // Timer for level completion speed

// Load font
scoreFont.loadFromFile("txt/Roboto-Regular.ttf");

scoreText.setFont(scoreFont);
scoreText.setCharacterSize(30);
scoreText.setFillColor(Color::White);
scoreText.setPosition(20, 10);

comboText.setFont(scoreFont);
comboText.setCharacterSize(25);
comboText.setFillColor(Color::Yellow);
comboText.setPosition(20, 50);


levelTimer.restart();  // Start timing the level

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
const int animationFrames = 4;  
Texture walkTextures[4];        
int currentFrame = 0;  
int frameCounter = 0;          
const int frameDelay = 10;      // (lower = faster)
bool isWalking = false;        

    // Music objects and settings
    Music menuMusic;
    Music lvlMusic;
    menuMusic.openFromFile("music/bgmus.ogg");
    menuMusic.setLoop(true);
    menuMusic.setVolume(40);

    lvlMusic.openFromFile("music/mus.ogg");
    lvlMusic.setLoop(true);
    lvlMusic.setVolume(40);
   

    // ===== PLAYER SELECTION MENU =====
    const int playerOptionsCount = 2;   // number of characters
    Texture playerOptions[playerOptionsCount];
    Sprite playerOptionSprite[playerOptionsCount];

    // Load textures for each character (must exist)
    playerOptions[0].loadFromFile("players/player1.png");
    playerOptions[1].loadFromFile("players/player2.png");

const int bagOptionsCount = 2;
Texture bagOptions[bagOptionsCount];
Sprite bagOptionSprite[bagOptionsCount];
bagOptions[0].loadFromFile("props/bag1.png");
bagOptions[1].loadFromFile("props/bag2.png");
    /// Set positions and scale for menu display
    for(int i = 0; i < playerOptionsCount; i++)
    {
        bagOptionSprite[i].setTexture(bagOptions[i]);
        bagOptionSprite[i].setScale(2,2);
        bagOptionSprite[i].setPosition(300 + i*300+73, 200+41);  // Changed from 300+41 to 200+41
        playerOptionSprite[i].setTexture(playerOptions[i]);
        playerOptionSprite[i].setScale(3,3);
        playerOptionSprite[i].setPosition(300 + i*300, 200); // Changed from 300 to 200
    }

    // ===== DESCRIPTION IMAGES =====
    Texture descTextures[2];
    Sprite descSprites[2];
   
    descTextures[0].loadFromFile("txt/greendesc.png");
    descTextures[1].loadFromFile("txt/yellowdesc.png");
   
    descSprites[0].setTexture(descTextures[0]);
    descSprites[0].setPosition(200, 300);  // Moved right (280→320) and up (500→400)
   
    descSprites[1].setTexture(descTextures[1]);
    descSprites[1].setPosition(500, 300);  // Moved right (580→620) and up (500→400)

   
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
        // Load walking animation frames based on selected player
if(selectedIndex == 0)
{
    walkTextures[0].loadFromFile("players/player1walk1.png");
    walkTextures[1].loadFromFile("players/player1walk2.png");
    walkTextures[2].loadFromFile("players/player1walk3.png");
    walkTextures[3].loadFromFile("players/player1walk4.png");
}
else if(selectedIndex == 1)
{
    walkTextures[0].loadFromFile("players/player2walk1.png");
    walkTextures[1].loadFromFile("players/player2walk2.png");
    walkTextures[2].loadFromFile("players/player2walk3.png");
    walkTextures[3].loadFromFile("players/player2walk4.png");
}
        bagTexture = bagOptions[selectedIndex];
        bagSprite.setTexture(bagTexture);
bagSprite.setScale(2, 2);

        window.clear();
        // highlight selected option
        for(int i = 0; i < playerOptionsCount; i++)
        {
            if(i == selectedIndex)
            {
                playerOptionSprite[i].setColor(Color::Yellow);
                bagOptionSprite[i].setColor(Color::Yellow);
            }
            else
            {
                playerOptionSprite[i].setColor(Color::White);
                bagOptionSprite[i].setColor(Color::White);
            }

            window.draw(bagOptionSprite[i]);
            window.draw(playerOptionSprite[i]);
            window.draw(descSprites[i]);  // This draws the description images
        }
        window.display();
    }
  speed = get_character_speed(selectedIndex, speed);  // Use 'speed' directly instead of 'baseSpeed'
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
    if(selectedIndex==0)
    {
    
    vacuumLeftTex.loadFromFile("players/left.png");  // default player with vacuum left
    vacuumUpTex.loadFromFile("players/up.png");
    vacuumRightTex.loadFromFile("players/right.png");
    vacuumDownTex.loadFromFile("players/down.png");
    }
    else
    {
    vacuumLeftTex.loadFromFile("players/left1.png");  // default player with vacuum left
    vacuumUpTex.loadFromFile("players/up1.png");
    vacuumRightTex.loadFromFile("players/right1.png");
    vacuumDownTex.loadFromFile("players/down1.png");
    }

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
    bool laserLoaded = laserTex.loadFromFile("props/lazer.png"); // try to load laser image
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

    
    /////  BOTTOM BLOCKS
    for (int j = 0; j < width; ++j) 
    lvl[8][j] = '#'; 
    
    lvl[5][6] = '#'; lvl[5][7] = '#'; lvl[5][8] = '#';
    lvl[5][9] = '#'; lvl[5][10] = '#'; lvl[5][11] = '#';

    ////  LEFT SIDE wall
    for(int i = 0; i <=7 ; ++i)
     lvl[i][0] = '#';

    ////  RIGHT PART wall
    for(int i = 0; i <=7; ++i)
     lvl[i][17] = '#';

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
     const int ghostAnimationFrames = 4;
     Texture ghostWalkTextures[4];
     int ghostCurrentFrame[8];      // cuuurrent frame for each ghost
int ghostFrameCounter[8];      // Frame counter for each ghost
const int ghostFrameDelay = 10; // aniimation speed

// Load ghost walking animation frames
ghostWalkTextures[0].loadFromFile("enemies/ghost1.png");
ghostWalkTextures[1].loadFromFile("enemies/ghost2.png");
ghostWalkTextures[2].loadFromFile("enemies/ghost3.png");
ghostWalkTextures[3].loadFromFile("enemies/ghost4.png");

// ===== SKELETON ANIMATION FRAMES =====
const int skel =4;
Texture skelWalkTextures[4];
skelWalkTextures[0].loadFromFile("enemies/skel1.png");
skelWalkTextures[1].loadFromFile("enemies/skel2.png");
skelWalkTextures[2].loadFromFile("enemies/skel3.png");
skelWalkTextures[3].loadFromFile("enemies/skel4.png");


const int skelAnimationFrames = 4;
int skelCurrentFrame[4];
int skelFrameCounter[4];
const int skelFrameDelay = 10;

// Initialize skeleton animation
for(int i = 0; i < skel; i++)
{
    skelCurrentFrame[i] = 0;
    skelFrameCounter[i] = 0;
}


   
    //spawning ghosts
srand(time(0)); // seed RNG

for(int i = 0; i < ghosts; i++)
{
    // pick random tile that is empty and has ground below
    while(true)
    {
        int tx = rand() % width;
        int ty = rand() % (height - 1);

        // Player spawns at row 4, platform is at row 5, columns 6-11
        // Exclude spawning on the player's starting platform
        bool onPlayerPlatform = (ty == 4 && tx >= 6 && tx <= 11);
       
        if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#' && !onPlayerPlatform)
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
       
        ghostSprite[i].setTexture(ghostWalkTextures[0]);
        ghostSprite[i].setScale(2, 2);   // increase ghost size
// Flip sprite if starting direction is right
if(ghost_dir[i] == 1)
{
    ghostSprite[i].setScale(-2, 2);
}
        ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
        ghostCurrentFrame[i] = 0;
    ghostFrameCounter[i] = 0;
    }

    float skel_x[9];
    float skel_y[9];
    float skel_speed[9];
    int skel_dir[9];
    float skel_velocityY[9];
    bool skel_onGround[9];
    Sprite skelSprite[9];
    bool skel_active[9];
 
    float skel_jump_timer[9];      // Timer for each skeleton
    float skel_next_jump_time[9];  // Random time until next jump (4-5 seconds)
    Texture skelTexture;
    skelTexture.loadFromFile("enemies/skeleton.png");
   
    // ===== ENEMY STUN SYSTEM ===== (FIXED)
    bool ghost_stunned[8];     // One for each ghost
    float ghost_stun_timer[8];
    bool skel_stunned[9];      // One for each skeleton
    float skel_stun_timer[9];
   
    // Initialize all to false/0
    for(int i = 0; i < ghosts; i++)
    {
        ghost_stunned[i] = false;
        ghost_stun_timer[i] = 0.0f;
    }
    for(int i = 0; i < skel; i++)
    {
        skel_stunned[i] = false;
        skel_stun_timer[i] = 0.0f;
    }
    // ===== POWER-UP SYSTEM =====
    Texture extraLifeTex, powerTex, rangeTex, speedTex;
    extraLifeTex.loadFromFile("props/extralife.png");
    powerTex.loadFromFile("props/power.png");
    rangeTex.loadFromFile("props/range.png");
    speedTex.loadFromFile("props/speed.png");

    float powerup_x[4];
    float powerup_y[4];
    bool powerup_active[4];
    int powerup_type[4];
    Sprite powerup_sprite[4];
    Clock powerup_spawn_timer[4];
    bool powerup_has_spawned[4];

    for(int i = 0; i < 4; i++)
    {
        powerup_active[i] = false;
        powerup_has_spawned[i] = false;
        powerup_type[i] = i;
    }

    Clock level1PowerUpTimer;
    Clock level2PowerUpTimer;
    bool level1PowerUpSpawned = false;
    bool level2PowerUpSpawned = false;

    Clock speedBoostTimer;
    bool speedBoostActive = false;
    const float speedBoostDuration = 15.0f;

    float baseSpeed = speed;
    float baseSuckStrength = 4.0f;
    float baseVacuumRange = 200.0f;

    float currentSuckStrength = baseSuckStrength;
    float currentVacuumRange = baseVacuumRange;
   
   
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
        }
    }

        skel_speed[i] = 1;
        skel_dir[i] = (rand() % 2 == 0) ? -1 : 1;
        skel_velocityY[i]=0;
        skel_onGround[i]=false;
        skel_active[i]=true;
       
        // ADD THESE TIMER INITIALIZATIONS:
        skel_jump_timer[i] = 0.0f;                                    // Start timer at 0
        skel_next_jump_time[i] = 10.0f + (rand() % 2);                // Random 4-5 seconds
       
        skelSprite[i].setTexture(skelTexture);
        skelSprite[i].setScale(2, 2);

        skelSprite[i].setPosition(skel_x[i], skel_y[i]);
    }


    Event ev;
    bool xKeyPressed=false;
   
    // Released enemies (projectiles) - for shooting captured enemies
    float shot_enemy_x[20];
    float shot_enemy_y[20];
    float shot_velocity_x[20];
    float shot_velocity_y[20];
    int shot_enemy_type[20];  // 0=ghost, 1=skeleton
    bool shot_is_active[20];
    float shot_lifetime[20];      //How long projectile has been alive
const float max_lifetime = 7.0f;  //Projectiles disappear after 3 seconds
   int shot_projectile_count=0;

    // Initialize shot array
    for (int i = 0; i < 20; i++)
    {
        shot_is_active[i] = false;
         shot_lifetime[i] = 0.0f;  // Initialize lifetime
    }

    // Key press tracking (for single-shot detection)
    bool previous_E_pressed = false;
    bool previous_Q_pressed = false;
   
    // ================= LEVEL 2 ENEMY SETUP =================
    // INVISIBLE MAN (3 total)
    const int invisible_men = 3;
    float invis_x[3];
    float invis_y[3];
    float invis_speed[3];
    int invis_dir[3];
    bool invis_active[3];
    bool invis_stunned[3];
    float invis_stun_timer[3];
    Sprite invisSprite[3];

    Texture invisTexture;
    invisTexture.loadFromFile("enemies/invisible.png");  

    for(int i = 0; i < invisible_men; i++)
    {
        invis_active[i] = false;  // Start inactive, spawn sequentially
        invis_stunned[i] = false;
        invis_stun_timer[i] = 0.0f;
        invis_speed[i] = 1.5f;
        invis_dir[i] = 1;
        invisSprite[i].setTexture(invisTexture);
        invisSprite[i].setScale(2, 2);
    }
    // ===== INVISIBLE MAN TELEPORT SYSTEM =====
float invis_teleport_timer[3];
const float invis_teleport_interval = 2.5f;
bool invis_is_invisible[3];
float invis_invisible_timer[3];

// Initialize teleport system
for(int i = 0; i < invisible_men; i++)
{
    invis_teleport_timer[i] = 0.0f;
    invis_is_invisible[i] = false;
    invis_invisible_timer[i] = 0.0f;
}

    // CHELNOV (4 total)
    const int chelnovs = 4;
    float chelnov_x[4];
    float chelnov_y[4];
    float chelnov_speed[4];
    int chelnov_dir[4];
    bool chelnov_active[4];
    bool chelnov_stunned[4];
    float chelnov_stun_timer[4];
    Sprite chelnovSprite[4];

   // Load Chelnov animation frames
const int chelnovWalkFrames = 4;
const int chelnovShootFrames = 4;
const int chelnovVacuumFrames = 4;

Texture chelnovWalkTextures[4];
Texture chelnovShootTextures[4];
Texture chelnovVacuumTextures[4];

// Load walking frames (0-3)
chelnovWalkTextures[0].loadFromFile("enemies/chelnov_walk1.png");
chelnovWalkTextures[1].loadFromFile("enemies/chelnov_walk2.png");
chelnovWalkTextures[2].loadFromFile("enemies/chelnov_walk3.png");
chelnovWalkTextures[3].loadFromFile("enemies/chelnov_walk4.png");

// Load shooting frames (4-7)
chelnovShootTextures[0].loadFromFile("enemies/chelnov_shoot1.png");
chelnovShootTextures[1].loadFromFile("enemies/chelnov_shoot2.png");
chelnovShootTextures[2].loadFromFile("enemies/chelnov_shoot3.png");
chelnovShootTextures[3].loadFromFile("enemies/chelnov_shoot4.png");

// Load vacuum frames (8-11)
chelnovVacuumTextures[0].loadFromFile("enemies/chelnov_vacuum1.png");
chelnovVacuumTextures[1].loadFromFile("enemies/chelnov_vacuum2.png");
chelnovVacuumTextures[2].loadFromFile("enemies/chelnov_vacuum3.png");
chelnovVacuumTextures[3].loadFromFile("enemies/chelnov_vacuum4.png");

// Chelnov animation state tracking
int chelnovCurrentFrame[4];
int chelnovFrameCounter[4];
const int chelnovFrameDelay = 8;

enum ChelnovState { WALKING, SHOOTING, VACUUMED };
ChelnovState chelnovState[4];

// Chelnov shooting system
float chelnov_shoot_timer[4];
const float chelnov_shoot_interval = 4.0f;  // Shoot every 4 seconds
const float chelnov_shoot_duration = 1.0f;  // 1 second shooting animation
bool chelnov_is_shooting[4];

// Chelnov projectiles (4 enemies × 1 projectile each = 4 max)
const int max_chelnov_projectiles = 10;
float chelnov_proj_x[10];
float chelnov_proj_y[10];
float chelnov_proj_vx[10];
float chelnov_proj_vy[10];
bool chelnov_proj_active[10];
float chelnov_proj_lifetime[10];
const float chelnov_proj_speed = 8.0f;
const float chelnov_proj_max_lifetime = 5.0f;

// Initialize projectiles
for(int i = 0; i < max_chelnov_projectiles; i++)
{
    chelnov_proj_active[i] = false;
    chelnov_proj_lifetime[i] = 0.0f;
}

for(int i = 0; i < chelnovs; i++)
{
    chelnov_active[i] = false;
    chelnov_stunned[i] = false;
    chelnov_stun_timer[i] = 0.0f;
    chelnov_speed[i] = 1.2f;
    chelnov_dir[i] = 1;
   
    chelnovSprite[i].setTexture(chelnovWalkTextures[0]);
    chelnovSprite[i].setScale(2, 2);
   
    chelnovCurrentFrame[i] = 0;
    chelnovFrameCounter[i] = 0;
    chelnovState[i] = WALKING;
   
    chelnov_shoot_timer[i] = 0.0f;
    chelnov_is_shooting[i] = false;
}

    // Sequential spawn tracking for Level 2
    Clock enemySpawnClock;
    int enemiesSpawned = 0;
    const float spawnInterval = 2.5f;  // Spawn an enemy every 2.5 seconds
    bool level2EnemiesSpawning = false;
    // Platform spawning for Level 2
   
Clock platformChangeClock;
bool platformNeedsRegen = true;
   
     // ===== SUCKING CAPACITY =====
     int captured[10];
    int cap_count = 0;           // Current count of sucked enemies
    int maxCapacity = 3;       // Maximum enemies that can be sucked at once
   

// ADD DELTA TIME CLOCK FOR SKELETON JUMP TIMING:
    Clock deltaClock;
    //main loop
    while (window.isOpen())
    {
    float deltaTime = deltaClock.restart().asSeconds();
    if(currentLevel == 1 && level1PowerUpTimer.getElapsedTime().asSeconds() == 0.0f)
        {
            level1PowerUpTimer.restart();
        }
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
        ///*** // Moving the character left and right using arrow keys'
        if(!playerDead) // Only allow controls if alive
        {
        isWalking = false;
        if(Keyboard::isKeyPressed(Keyboard::Key::Right ))
        {
            // attempt to move right with collision check
            player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
           
            bagSprite.setScale(-2,2);
            PlayerSprite.setScale(-3,3);
            facingRight = true;// flip horizontally to face right
            isWalking=true;
        }
       
        if(Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
            // attempt to move left
            player_left_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
           
            bagSprite.setScale(2,2);
            PlayerSprite.setScale(3,3);
            facingRight=false; // normal facing left
            isWalking = true;
        }
        if(isWalking)
        {
        frameCounter++;
        if(frameCounter>=frameDelay)
        {
        frameCounter=0;
        currentFrame = (currentFrame+1) % animationFrames;
        PlayerSprite.setTexture(walkTextures[currentFrame]);
        }
        }
        else
        {
        currentFrame = 0;
        frameCounter = 0;
        PlayerSprite.setTexture(PlayerTexture);
        }
       
        // Jump with up if on ground
        if(Keyboard::isKeyPressed(Keyboard::Key::Up))
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
} //end of player controls if alive
       
        // ============= VACUUM SUCKING WITH Space KEY =============
        xKeyPressed = Keyboard::isKeyPressed(Keyboard::Key::Space);

        if(xKeyPressed && vacuumActive)
        {
           update_vacuum(player_x, player_y, vacuumDirection, currentVacuumRange,currentSuckStrength ,
    ghost_x, ghost_y, ghost_active, ghost_stunned, ghost_stun_timer, ghosts,
    skel_x, skel_y, skel_active, skel_stunned, skel_stun_timer, skel,
    invis_x, invis_y, invis_active, invis_stunned, invis_stun_timer, invisible_men,
    chelnov_x, chelnov_y, chelnov_active, chelnov_stunned, chelnov_stun_timer, chelnovs,
    chelnov_is_shooting,  // <--- ADD THIS PARAMETER
    cell_size, true, captured, cap_count, maxCapacity, score
);
        }


       
// ============= SHOOTING CONTROLS - E AND Q KEYS =============
        bool E_key_pressed = Keyboard::isKeyPressed(Keyboard::E);
        bool Q_key_pressed = Keyboard::isKeyPressed(Keyboard::Q);

        // E key = Single Shot (shoot one enemy at a time)
        if (E_key_pressed && !previous_E_pressed)
        {
            if (cap_count > 0)
            {
                shoot_single_enemy(player_x, player_y, vacuumDirection, captured, cap_count,
                  shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                  shot_enemy_type, shot_is_active, shot_lifetime, shot_projectile_count);  
            }
        }

        // Q key = Burst Shot (shoot ALL captured enemies at once)
        if (Q_key_pressed && !previous_Q_pressed)
        {
            if (cap_count > 0)
            {
                shoot_burst_mode(player_x, player_y, vacuumDirection, captured, cap_count,
                shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                shot_enemy_type, shot_is_active, shot_lifetime, shot_projectile_count);
            }
        }


// Check if any projectile hit an enemy
       int hit_projectile = -1;
int hit_enemy_index = -1;
int hit_enemy_type = -1;

if (check_projectile_hits(shot_enemy_x, shot_enemy_y, shot_is_active, shot_projectile_count,
                          ghost_x, ghost_y, ghost_active, ghosts,
                          skel_x, skel_y, skel_active, skel,
                          invis_x, invis_y, invis_active, invisible_men,
                          chelnov_x, chelnov_y, chelnov_active, chelnovs,
                          hit_projectile, hit_enemy_index, hit_enemy_type))
{
    if (hit_projectile >= 0 && hit_enemy_index >= 0)
    {
        shot_is_active[hit_projectile] = false;

        if (hit_enemy_type == 0)  // Ghost
        {
            ghost_active[hit_enemy_index] = false;
            ghost_x[hit_enemy_index] = -1000;
            ghost_y[hit_enemy_index] = -1000;
            score += 100;
            combo++;
           
        }
        else if (hit_enemy_type == 1)  // Skeleton
        {
            skel_active[hit_enemy_index] = false;
            skel_x[hit_enemy_index] = -1000;
            skel_y[hit_enemy_index] = -1000;
            score += 150;
            combo++;
           
        }
        else if (hit_enemy_type == 2)  // Invisible Man
        {
            invis_active[hit_enemy_index] = false;
            invis_x[hit_enemy_index] = -1000;
            invis_y[hit_enemy_index] = -1000;
            score += 200;
            combo++;
           
        }
        else if (hit_enemy_type == 3)  // Chelnov
        {
            chelnov_active[hit_enemy_index] = false;
            chelnov_x[hit_enemy_index] = -1000;
            chelnov_y[hit_enemy_index] = -1000;
            score += 250;
            combo++;
           
        }
    }
}


        // Update key states for next frame
        previous_E_pressed = E_key_pressed;
        previous_Q_pressed = Q_key_pressed;

        // Update all projectile physics
       update_projectiles(shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                  shot_is_active, shot_lifetime, deltaTime, max_lifetime,  // ADD these 3 parameters
                  shot_projectile_count, lvl, cell_size, height);
       
        // ============= DRAW PLAYER OR VACUUM =============
    if(!playerDead)
    {
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
            if(facingRight){
            bagSprite.setPosition(player_x-70+100, player_y+41);
            PlayerSprite.setPosition(player_x+100, player_y);
            }
            else
            {
            bagSprite.setPosition(player_x+73, player_y+41);
            PlayerSprite.setPosition(player_x, player_y);
            }
            window.draw(bagSprite);
            window.draw(PlayerSprite);
        }
}
       // *** LASER BEAM DRAWING WHEN Space IS PRESSED ***
        if(xKeyPressed && vacuumActive && laserLoaded)
        {
            float laser_x = player_x + PlayerWidth/2;
            float laser_y = player_y + PlayerHeight/2;

            // Position laser in front of vacuum based on direction (no rotation needed)
            switch(vacuumDirection)
            {
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


// Draw all active projectiles (shot enemies) 
for (int i = 0; i < 20; i++)
{
    if (shot_is_active[i])
    {
        if(shot_enemy_type[i] == 0) // Ghost
        {
            ghostSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            ghostSprite[0].setColor(Color(255, 100, 100));
            window.draw(ghostSprite[0]);
            ghostSprite[0].setColor(Color::White);
        }
        else if(shot_enemy_type[i] == 1) // Skeleton
        {
            skelSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            skelSprite[0].setColor(Color(255, 100, 100));
            window.draw(skelSprite[0]);
            skelSprite[0].setColor(Color::White);
        }
        else if(shot_enemy_type[i] == 2) // Invisible Man
        {
            invisSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            invisSprite[0].setColor(Color(255, 100, 100));
            window.draw(invisSprite[0]);
            invisSprite[0].setColor(Color::White);
        }
        else if(shot_enemy_type[i] == 3) // Chelnov
        {
            chelnovSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            chelnovSprite[0].setColor(Color(255, 100, 100));
            window.draw(chelnovSprite[0]);
            chelnovSprite[0].setColor(Color::White);
        }
    }
}


// ====== INVISIBLE MAN MOVEMENT WITH TELEPORT ======
for(int i = 0; i < invisible_men; i++)
{
    if(!invis_active[i]) continue;
   
    // Update invisibility effect
    if(invis_is_invisible[i])
    {
        invis_invisible_timer[i] -= deltaTime;
        if(invis_invisible_timer[i] <= 0)
        {
            invis_is_invisible[i] = false;
        }
       
        // DON'T DRAW when invisible - they disappear completely!
        continue;
    }
   
    // If stunned, show normal sprite
    if(invis_stunned[i])
    {
        invisSprite[i].setPosition(invis_x[i], invis_y[i]);
        window.draw(invisSprite[i]);
        continue;
    }
   
    // Update teleport timer
    invis_teleport_timer[i] += deltaTime;
   
    // Random teleportation event
    if(invis_teleport_timer[i] >= invis_teleport_interval)
    {
        invis_teleport_timer[i] = 0.0f;
       
        // 70% chance to teleport, 30% chance to go invisible
        if(rand() % 100 < 70)
        {
            // TELEPORT to random platform
            int attempts = 0;
            while(attempts < 10)
            {
                int tx = 2 + rand() % 14;
                int ty = 2 + rand() % 10;
               
                if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
                {
                    invis_x[i] = tx * cell_size;
                    invis_y[i] = ty * cell_size;
                   
                    break;
                }
                attempts++;
            }
        }
        else
        {
            // Go INVISIBLE
            invis_is_invisible[i] = true;
            invis_invisible_timer[i] = 0.8f;
           
        }
    }
   
    // Normal left-right movement
    invis_x[i] += invis_speed[i] * invis_dir[i];
   
    // Check boundaries and turn around
    int tile_x = invis_x[i] / cell_size;
   
    if(tile_x <= 1 || tile_x >= width-2)
    {
        invis_dir[i] *= -1;
    }
   
    invisSprite[i].setPosition(invis_x[i], invis_y[i]);
    window.draw(invisSprite[i]);
}

// ====== CHELNOV MOVEMENT WITH SHOOTING ======
for(int i = 0; i < chelnovs; i++)
{
    if(!chelnov_active[i]) continue;
   
    // Update shoot timer
    chelnov_shoot_timer[i] += deltaTime;
   
    // Check if it's time to shoot
    if(!chelnov_is_shooting[i] && chelnov_shoot_timer[i] >= chelnov_shoot_interval && !chelnov_stunned[i])
    {
        // Start shooting
        chelnov_is_shooting[i] = true;
        chelnov_shoot_timer[i] = 0.0f;
        chelnovState[i] = SHOOTING;
        chelnovCurrentFrame[i] = 0;
       
        // Fire projectile HORIZONTALLY - aim at player's X position only
        float player_dx = player_x - chelnov_x[i];
        int fire_direction = (player_dx > 0) ? 1 : -1;  // Left or right based on player
       
        for(int p = 0; p < max_chelnov_projectiles; p++)
        {
            if(!chelnov_proj_active[p])
            {
                chelnov_proj_x[p] = chelnov_x[i] + 32;  // Center of Chelnov
                chelnov_proj_y[p] = chelnov_y[i] + 32;
                chelnov_proj_vx[p] = chelnov_proj_speed * fire_direction;  // Move toward player
                chelnov_proj_vy[p] = 0.0f;  // NO vertical movement
                chelnov_proj_active[p] = true;
                chelnov_proj_lifetime[p] = 0.0f;
                 break;
            }
        }
    }
   
    // Handle shooting duration
    if(chelnov_is_shooting[i])
    {
        if(chelnov_shoot_timer[i] >= chelnov_shoot_duration)
        {
            chelnov_is_shooting[i] = false;
            chelnovState[i] = WALKING;
            chelnov_shoot_timer[i] = 0.0f;
        }
    }
   
    // If stunned, show vacuum animation
    if(chelnov_stunned[i])
    {
        chelnovState[i] = VACUUMED;
       
        chelnovFrameCounter[i]++;
        if(chelnovFrameCounter[i] >= chelnovFrameDelay)
        {
            chelnovFrameCounter[i] = 0;
            chelnovCurrentFrame[i] = (chelnovCurrentFrame[i] + 1) % chelnovVacuumFrames;
            chelnovSprite[i].setTexture(chelnovVacuumTextures[chelnovCurrentFrame[i]]);
        }
       
        chelnovSprite[i].setPosition(chelnov_x[i], chelnov_y[i]);
        window.draw(chelnovSprite[i]);
        continue;
    }
   
    // MOVEMENT - Only if NOT shooting
    if(!chelnov_is_shooting[i])
    {
        chelnov_x[i] += chelnov_speed[i] * chelnov_dir[i];
       
        int tile_x = chelnov_x[i] / cell_size;
        int tile_y = chelnov_y[i] / cell_size;
       
        // Check walls
        if(tile_x <= 1 || tile_x >= width-2)
        {
            chelnov_dir[i] *= -1;
        }
       
        // Check edges and turn around
        int next_tile_x = (chelnov_x[i] + chelnov_dir[i] * chelnov_speed[i] * 10) / cell_size;
        int check_ground_y = (chelnov_y[i] + 64 + 5) / cell_size;
       
        if(check_ground_y < height && next_tile_x >= 0 && next_tile_x < width)
        {
            if(lvl[check_ground_y][next_tile_x] != '#')
            {
                chelnov_dir[i] *= -1;
            }
        }
       
        // Animate walking
        chelnovFrameCounter[i]++;
        if(chelnovFrameCounter[i] >= chelnovFrameDelay)
        {
            chelnovFrameCounter[i] = 0;
            chelnovCurrentFrame[i] = (chelnovCurrentFrame[i] + 1) % chelnovWalkFrames;
            chelnovSprite[i].setTexture(chelnovWalkTextures[chelnovCurrentFrame[i]]);
        }
    }
    else  // SHOOTING - animate but don't move
    {
        chelnovFrameCounter[i]++;
        if(chelnovFrameCounter[i] >= chelnovFrameDelay)
        {
            chelnovFrameCounter[i] = 0;
            chelnovCurrentFrame[i] = (chelnovCurrentFrame[i] + 1) % chelnovShootFrames;
            chelnovSprite[i].setTexture(chelnovShootTextures[chelnovCurrentFrame[i]]);
        }
    }
   
    // Flip sprite based on direction
    if(chelnov_dir[i] == 1)
        chelnovSprite[i].setScale(-2, 2);
    else
        chelnovSprite[i].setScale(2, 2);
   
    chelnovSprite[i].setPosition(chelnov_x[i], chelnov_y[i]);
    window.draw(chelnovSprite[i]);
}

// ====== UPDATE CHELNOV PROJECTILES ======
for(int p = 0; p < max_chelnov_projectiles; p++)
{
    if(!chelnov_proj_active[p]) continue;
   
    // Update lifetime
    chelnov_proj_lifetime[p] += deltaTime;
    if(chelnov_proj_lifetime[p] >= chelnov_proj_max_lifetime)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
   
    // Move projectile HORIZONTALLY ONLY
    chelnov_proj_x[p] += chelnov_proj_vx[p];
   
    // Check screen bounds
    if(chelnov_proj_x[p] < 0 || chelnov_proj_x[p] > screen_x ||
       chelnov_proj_y[p] < 0 || chelnov_proj_y[p] > screen_y)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
   
    // Check collision with player
    if(!playerDead)
    {
        float dx = chelnov_proj_x[p] - (player_x + PlayerWidth/2);
        float dy = chelnov_proj_y[p] - (player_y + PlayerHeight/2);
        float dist = sqrt(dx*dx + dy*dy);
       
        if(dist < 40)
        {
            // Hit player!
            playerDead = true;
            deathClock.restart();
            score -= 50;
            if(score < 0) score = 0;
            combo = 0;
            comboMultiplier = 1.0f;
            chelnov_proj_active[p] = false;
         
            continue;
        }
    }
   
    // Draw projectile (red circle)
    CircleShape projShape(8);
    projShape.setFillColor(Color::Red);
    projShape.setPosition(chelnov_proj_x[p] - 8, chelnov_proj_y[p] - 8);
    window.draw(projShape);
}
// ====== UPDATE CHELNOV PROJECTILES ======
for(int p = 0; p < max_chelnov_projectiles; p++)
{
    if(!chelnov_proj_active[p]) continue;
   
    // Update lifetime
    chelnov_proj_lifetime[p] += deltaTime;
    if(chelnov_proj_lifetime[p] >= chelnov_proj_max_lifetime)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
   
    // Move projectile
    chelnov_proj_x[p] += chelnov_proj_vx[p];
    chelnov_proj_y[p] += chelnov_proj_vy[p];
   
    // Check screen bounds
    if(chelnov_proj_x[p] < 0 || chelnov_proj_x[p] > screen_x ||
       chelnov_proj_y[p] < 0 || chelnov_proj_y[p] > screen_y)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
   
    // Check collision with player
    if(!playerDead)
    {
        float dx = chelnov_proj_x[p] - (player_x + PlayerWidth/2);
        float dy = chelnov_proj_y[p] - (player_y + PlayerHeight/2);
        float dist = sqrt(dx*dx + dy*dy);
       
        if(dist < 40)
        {
            // Hit player!
            playerDead = true;
            deathClock.restart();
            score -= 50;
            if(score < 0) score = 0;
            combo = 0;
            comboMultiplier = 1.0f;
            chelnov_proj_active[p] = false;
           
            continue;
        }
    }
   
    // Draw projectile (red circle)
    CircleShape projShape(8);
    projShape.setFillColor(Color::Red);
    projShape.setPosition(chelnov_proj_x[p] - 8, chelnov_proj_y[p] - 8);
    window.draw(projShape);
}
        // ====== GHOST MOVEMENT WITH ANIMATION ======
for(int i = 0; i < ghosts; i++)
{
    if(!ghost_active[i]) continue;

    if(ghost_stunned[i])
    {
        ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
        window.draw(ghostSprite[i]);
        continue;
    }

    int bottomLeftX = ghost_x[i] / cell_size;
    int bottomRightX = (ghost_x[i] + 64) / cell_size;
    int bottomY = (ghost_y[i] + 64) / cell_size;

    if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#')
    {
        ghost_onGround[i] = true;
    }

    if(ghost_onGround[i])
    {
        float nextX = ghost_x[i] + ghost_speed[i] * ghost_dir[i];

        int frontTileX = (nextX + (ghost_dir[i] == 1 ? 64 : 0)) / cell_size;
        int midTileY = (ghost_y[i] + 32) / cell_size;

        int edgeCheckX = (nextX + (ghost_dir[i] == 1 ? 64 : 0)) / cell_size;
        int edgeCheckY = (ghost_y[i] + 64 + 1) / cell_size;

        if(lvl[midTileY][frontTileX] == '#' || lvl[edgeCheckY][edgeCheckX] != '#')
        {
            ghost_dir[i] *= -1;
            if(ghost_dir[i] == 1)
                ghostSprite[i].setScale(-2, 2);
            else
                ghostSprite[i].setScale(2, 2);
        }
        else
        {
            ghost_x[i] = nextX;
        }
       
        // ANIMATE GHOST
        ghostFrameCounter[i]++;
        if(ghostFrameCounter[i] >= ghostFrameDelay)
        {
            ghostFrameCounter[i] = 0;
            ghostCurrentFrame[i] = (ghostCurrentFrame[i] + 1) % ghostAnimationFrames;
            ghostSprite[i].setTexture(ghostWalkTextures[ghostCurrentFrame[i]]);
        }
    }

    ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
    window.draw(ghostSprite[i]);
}

        // ====== SKELETON MOVEMENT WITH ANIMATION ======
for(int j = 0; j < skel; j++)
{
    if(!skel_active[j]) continue;

    if(skel_stunned[j])
    {
        skelSprite[j].setPosition(skel_x[j], skel_y[j]);
        window.draw(skelSprite[j]);
        continue;
    }

    skel_jump_timer[j] += deltaTime;

    if(!skel_onGround[j])
    {
        skel_velocityY[j] += gravity;
        if(skel_velocityY[j] > terminal_Velocity)
        {
            skel_velocityY[j] = terminal_Velocity;
        }
    }

    float nextY = skel_y[j] + skel_velocityY[j];

    if(skel_velocityY[j] < 0)
    {
        int top_y = (int)(nextY) / cell_size;
       
        if(top_y <= 0)
        {
            int left_x = (int)(skel_x[j]) / cell_size;
            int mid_x = (int)(skel_x[j] + 32) / cell_size;
            int right_x = (int)(skel_x[j] + 64) / cell_size;
           
            if(lvl[0][left_x] == '#' || lvl[0][mid_x] == '#' || lvl[0][right_x] == '#')
            {
                skel_velocityY[j] = 0;
                skel_y[j] = cell_size;
                skel_onGround[j] = false;
            }
            else
            {
                skel_y[j] = nextY;
                skel_onGround[j] = false;
            }
        }
        else
        {
            skel_y[j] = nextY;
            skel_onGround[j] = false;
        }
    }
    else if(skel_velocityY[j] > 0)
    {
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
            skel_y[j] = nextY;
            skel_onGround[j] = false;
        }
    }
    else
    {
        skel_y[j] = nextY;
        skel_onGround[j] = false;
    }

    if(skel_onGround[j])
    {
        if(skel_jump_timer[j] >= skel_next_jump_time[j])
        {
            skel_velocityY[j] = -20.0f;
            skel_onGround[j] = false;
           
            skel_jump_timer[j] = 0.0f;
            skel_next_jump_time[j] = 10.0f + (rand() % 2);
           
            if(rand() % 2 == 0)
            {
                skel_dir[j] *= -1;
            }
        }
        else
        {
            float nextp = skel_x[j] + skel_speed[j] * skel_dir[j];

            int frontskelX = (nextp + (skel_dir[j] == 1 ? 64 : 0)) / cell_size;
            int midTileY = (skel_y[j] + 32) / cell_size;

            int edgeCheckX = (nextp + (skel_dir[j] == 1 ? 64 : 0)) / cell_size;
            int edgeCheckY = (skel_y[j] + 64 + 1) / cell_size;

            if(lvl[midTileY][frontskelX] == '#' || lvl[edgeCheckY][edgeCheckX] != '#')
            {
                skel_dir[j] *= -1;
            }
            else
            {
                skel_x[j] = nextp;
            }
        }
       
        // ANIMATE SKELETON
        skelFrameCounter[j]++;
        if(skelFrameCounter[j] >= skelFrameDelay)
        {
            skelFrameCounter[j] = 0;
            skelCurrentFrame[j] = (skelCurrentFrame[j] + 1) % skelAnimationFrames;
            skelSprite[j].setTexture(skelWalkTextures[skelCurrentFrame[j]]);
        }
    }

    skelSprite[j].setPosition(skel_x[j], skel_y[j]);
    window.draw(skelSprite[j]);
}
  // ====== CHECK PLAYER-ENEMY COLLISIONS ======
if(!playerDead)
{
    // Check ghost collisions
    for(int i = 0; i < ghosts; i++)
    {
        if(ghost_active[i] && !ghost_stunned[i])  // FIXED: Don't collide with stunned ghosts!
        {
            if(check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
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
            if(skel_active[i] && !skel_stunned[i])  // FIXED: Don't collide with stunned skeletons!
            {
               if(check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
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
    // Check Invisible Man collisions
if(!playerDead)
{
    for(int i = 0; i < invisible_men; i++)
    {
        if(invis_active[i] && !invis_stunned[i])
        {
            if(check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                               invis_x[i], invis_y[i], 64))
            {
                playerDead = true;
                deathClock.restart();
                score -= 50;
                if(score < 0) score = 0;
                combo = 0;
                comboMultiplier = 1.0f;
                break;
            }
        }
    }
}

// Check Chelnov collisions
if(!playerDead)
{
    for(int i = 0; i < chelnovs; i++)
    {
        if(chelnov_active[i] && !chelnov_stunned[i])
        {
            if(check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                               chelnov_x[i], chelnov_y[i], 64))
            {
                playerDead = true;
                deathClock.restart();
                score -= 50;
                if(score < 0) score = 0;
                combo = 0;
                comboMultiplier = 1.0f;
                break;
            }
        }
    }
}
}
if(currentLevel == 1 && !level1PowerUpSpawned)
{
    float elapsed = level1PowerUpTimer.getElapsedTime().asSeconds();
   
    if(elapsed >= 10.0f)
    {
        int randomType = rand() % 4;
       
        if(randomType == 0)
            spawn_powerup(powerup_x[0], powerup_y[0], powerup_active[0], powerup_type[0],
                         powerup_sprite[0], powerup_spawn_timer[0], powerup_has_spawned[0],
                         lvl, width, height, cell_size, 0, extraLifeTex);
        else if(randomType == 1)
            spawn_powerup(powerup_x[1], powerup_y[1], powerup_active[1], powerup_type[1],
                         powerup_sprite[1], powerup_spawn_timer[1], powerup_has_spawned[1],
                         lvl, width, height, cell_size, 1, powerTex);
        else if(randomType == 2)
            spawn_powerup(powerup_x[2], powerup_y[2], powerup_active[2], powerup_type[2],
                         powerup_sprite[2], powerup_spawn_timer[2], powerup_has_spawned[2],
                         lvl, width, height, cell_size, 2, rangeTex);
        else
            spawn_powerup(powerup_x[3], powerup_y[3], powerup_active[3], powerup_type[3],
                         powerup_sprite[3], powerup_spawn_timer[3], powerup_has_spawned[3],
                         lvl, width, height, cell_size, 3, speedTex);
       
        level1PowerUpSpawned = true;
    }
}

if(currentLevel == 2 && !level2PowerUpSpawned)
{
    float elapsed = level2PowerUpTimer.getElapsedTime().asSeconds();
   
    if(elapsed >= 15.0f)
    {
        int randomType = rand() % 4;
       
        if(randomType == 0)
            spawn_powerup(powerup_x[0], powerup_y[0], powerup_active[0], powerup_type[0],
                         powerup_sprite[0], powerup_spawn_timer[0], powerup_has_spawned[0],
                         lvl, width, height, cell_size, 0, extraLifeTex);
        else if(randomType == 1)
            spawn_powerup(powerup_x[1], powerup_y[1], powerup_active[1], powerup_type[1],
                         powerup_sprite[1], powerup_spawn_timer[1], powerup_has_spawned[1],
                         lvl, width, height, cell_size, 1, powerTex);
        else if(randomType == 2)
            spawn_powerup(powerup_x[2], powerup_y[2], powerup_active[2], powerup_type[2],
                         powerup_sprite[2], powerup_spawn_timer[2], powerup_has_spawned[2],
                         lvl, width, height, cell_size, 2, rangeTex);
        else
            spawn_powerup(powerup_x[3], powerup_y[3], powerup_active[3], powerup_type[3],
                         powerup_sprite[3], powerup_spawn_timer[3], powerup_has_spawned[3],
                         lvl, width, height, cell_size, 3, speedTex);
       
        level2PowerUpSpawned = true;
    }
}

// Power-up collision detection
if(!playerDead)
{
    for(int i = 0; i < 4; i++)
    {
        if(powerup_active[i])
        {
            if(check_powerup_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                                      powerup_x[i], powerup_y[i], 64))
            {
                if(powerup_type[i] == 0)  // EXTRA_LIFE
                {
                    playerLives++;
                   
                }
                else if(powerup_type[i] == 1)  // POWER
                {
                    currentSuckStrength = baseSuckStrength * 2.0f;
                   
                }
                else if(powerup_type[i] == 2)  // RANGE
                {
                    currentVacuumRange = baseVacuumRange * 1.5f;
                   
                }
                else if(powerup_type[i] == 3)  // SPEED
                {
                    speed = baseSpeed * 2.0f;
                    speedBoostActive = true;
                    speedBoostTimer.restart();
                 
                }
               
                powerup_active[i] = false;
                score += 50;
            }
        }
    }
}

// Speed boost timer
if(speedBoostActive)
{
    if(speedBoostTimer.getElapsedTime().asSeconds() >= speedBoostDuration)
    {
        speed = baseSpeed;
        speedBoostActive = false;
       
    }
}
if(currentLevel == 1 && !levelComplete)
{
    if(check_level_complete(ghost_active, ghosts,
                           skel_active, skel,
                           invis_active, 0,
                           chelnov_active, 0))
    {
        levelComplete = true;
        levelCompleteClock.restart();
       
    }
}

// Wait 3 seconds then transition
if(levelComplete && levelCompleteClock.getElapsedTime().asSeconds() >= levelCompleteDelay)
{
    currentLevel = 2;
      playerLives=3;
      maxCapacity=5;
    levelComplete = false;
   
    // Reset level
    change_to_level2(lvl, height, width);
 
   
    // Activate Level 2 enemies
    level2EnemiesSpawning = true;
    enemySpawnClock.restart();
    enemiesSpawned = 0;
   
    // Reset platform timer
    platformChangeClock.restart();
    platformNeedsRegen = false;
   
    // Reset player position
    player_x = 500;
    player_y = 150;
    velocityY = 0;
}


       
       // ====== HANDLE PLAYER DEATH AND RESPAWN ======
        if(playerDead)
        {
            float elapsedTime = deathClock.getElapsedTime().asSeconds();
           
            // Make player blink during death
            if((int)(elapsedTime * 4) % 2 == 0)
            {
                PlayerSprite.setColor(Color(255, 255, 255, 100));
                bagSprite.setColor(Color(255, 255, 255, 100));
            }
            else
            {
                PlayerSprite.setColor(Color(255, 255, 255, 255));
                bagSprite.setColor(Color(255, 255, 255, 255));
            }
           
            if(elapsedTime >= respawnDelay)
            {
                playerLives--;
               
                if(playerLives > 0)
                {
                    respawn_player(player_x, player_y, velocityY, playerDead);
                    PlayerSprite.setColor(Color(255, 255, 255, 255));
                    bagSprite.setColor(Color(255, 255, 255, 255));
                    vacuumActive = false;
                    cap_count = 0;
                }
                else
                {
                // PENALTY: Death (all 3 lives lost)
    score -= 200;
    if(score < 0) score = 0;
                    // Game Over - close window
                    window.close();
                }
            }
        }

// Update and draw score
scoreText.setString("Score: " + to_string(score) + "  Lives: " + to_string(playerLives));
window.draw(scoreText);

// Draw combo if active
if(combo > 0)
{
    comboText.setString("Combo: " + to_string(combo) + "x  Multiplier: " + to_string(comboMultiplier) + "x");
    window.draw(comboText);
}
 
 // ====== CHECK LEVEL COMPLETION ======
 if(currentLevel == 2)
{
    float platformTime = platformChangeClock.getElapsedTime().asSeconds();
   
    if(platformTime >= 20.0f)
    {
        generate_random_slanted_platform(lvl, height, width, true);  // true = clear old platform
        platformChangeClock.restart();
    }
}
 // ====== LEVEL 2 SEQUENTIAL ENEMY SPAWNING ======
if(currentLevel == 2 && level2EnemiesSpawning)
{
    float spawnElapsed = enemySpawnClock.getElapsedTime().asSeconds();
   
    if(spawnElapsed >= 20.0f)
    {
        enemySpawnClock.restart();
       
        // Total enemies to spawn: 3 invisible + 4 chelnov + 4 ghosts + 9 skeletons = 20
       
        // Spawn invisible men first (0-2)
        if(enemiesSpawned < 3)
        {
            int idx = enemiesSpawned;
            // Find random spawn position
            while(true)
            {
                int tx = 2 + rand() % 14;
                int ty = rand() % (height - 1);
               
                if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
                {
                    invis_x[idx] = tx * cell_size;
                    invis_y[idx] = ty * cell_size;
                    invis_active[idx] = true;
                    invis_dir[idx] = (rand() % 2 == 0) ? -1 : 1;
                    invisSprite[idx].setPosition(invis_x[idx], invis_y[idx]);
                    break;
                }
            }
        }
        // Then spawn chelnovs (3-6)
        else if(enemiesSpawned >= 3 && enemiesSpawned < 7)
        {
            int idx = enemiesSpawned - 3;
            while(true)
            {
                int tx = 2 + rand() % 14;
                int ty = rand() % (height - 1);
               
                if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
                {
                    chelnov_x[idx] = tx * cell_size;
                    chelnov_y[idx] = ty * cell_size;
                    chelnov_active[idx] = true;
                    chelnov_dir[idx] = (rand() % 2 == 0) ? -1 : 1;
                    chelnovSprite[idx].setPosition(chelnov_x[idx], chelnov_y[idx]);
                    break;
                }
            }
        }
        // Then spawn ghosts (7-10)
        else if(enemiesSpawned >= 7 && enemiesSpawned < 11)
        {
            int idx = enemiesSpawned - 7;
            while(true)
            {
                int tx = 2 + rand() % 14;
                int ty = rand() % (height - 1);
               
                if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
                {
                    ghost_x[idx] = tx * cell_size;
                    ghost_y[idx] = ty * cell_size;
                    ghost_active[idx] = true;
                    ghost_dir[idx] = (rand() % 2 == 0) ? -1 : 1;
                    ghostSprite[idx].setPosition(ghost_x[idx], ghost_y[idx]);
                    break;
                }
            }
        }
       
        // Finally spawn skeletons (11-19) - now up to 9 skeletons in level 2
        else if(enemiesSpawned >= 11 && enemiesSpawned < 20)
        {
            int idx = enemiesSpawned - 11;
            while(true)
            {
                int tx = 2 + rand() % 14;
                int ty = rand() % (height - 1);
               
                if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#')
                {
                    skel_x[idx] = tx * cell_size;
                    skel_y[idx] = ty * cell_size;
                    skel_active[idx] = true;
                    skel_dir[idx] = (rand() % 2 == 0) ? -1 : 1;
                    skel_jump_timer[idx] = 0.0f;
                    skel_next_jump_time[idx] = 2.0f + (rand() % 3);
                    skel_stunned[idx] = false;
                    skel_stun_timer[idx] = 0.0f;
                    skelSprite[idx].setTexture(skelTexture);
                    skelSprite[idx].setScale(2, 2);
                    skelSprite[idx].setPosition(skel_x[idx], skel_y[idx]);
                    break;
                }
            }
        }
       
       
        enemiesSpawned++;
       
        if(enemiesSpawned >= 20)
        {
            level2EnemiesSpawning = false;
             }
    }
}
// ====== CHECK LEVEL COMPLETION ======
if(!levelComplete && !playerDead)
{
   if(currentLevel == 1)
{
    // For Level 1, pass empty arrays for invisible men and chelnov
    bool empty_invis[3] = {false, false, false};
    bool empty_chelnov[4] = {false, false, false, false};
   
    if(check_level_complete(ghost_active, ghosts, skel_active, skel,
                           empty_invis, 0, empty_chelnov, 0))
    {
        levelComplete = true;
        levelCompleteClock.restart();
        score += 500;
      
    }
}
   
    else if(currentLevel == 2)
    {
        // Check all Level 2 enemies
        bool all_defeated = true;
       
        for(int i = 0; i < invisible_men; i++)
            if(invis_active[i]) all_defeated = false;
           
        for(int i = 0; i < chelnovs; i++)
            if(chelnov_active[i]) all_defeated = false;
           
        for(int i = 0; i < 4; i++)
            if(ghost_active[i]) all_defeated = false;
           
        for(int i = 0; i < 9; i++)
            if(skel_active[i]) all_defeated = false;
       
        if(all_defeated && !level2EnemiesSpawning)
        {
            levelComplete = true;
            levelCompleteClock.restart();
            score += 1000;
           
        }
    }
}
   

// ====== SHOW COMPLETION MESSAGE & MOVE TO LEVEL 2 ======
if(levelComplete && currentLevel == 1)
{
    float elapsed = levelCompleteClock.getElapsedTime().asSeconds();
   
    // Draw "LEVEL 1 COMPLETE" message
    Text completeText;
    completeText.setFont(scoreFont);
    completeText.setCharacterSize(60);
    completeText.setFillColor(Color::Green);
    completeText.setString("LEVEL 1 COMPLETE!");
    completeText.setPosition(200, 350);
    window.draw(completeText);
   
    Text nextText;
    nextText.setFont(scoreFont);
    nextText.setCharacterSize(40);
    nextText.setFillColor(Color::Yellow);
    nextText.setString("Moving to Level 2...");
    nextText.setPosition(280, 450);
    window.draw(nextText);
 
  // NOW FIX THE TIMING ISSUE
// In main() function, find the level transition code (around line 2520-2570)
// REPLACE the level 2 transition section with this:

// After 3 seconds, load Level 2
if(elapsed >= levelCompleteDelay)
{
 
    currentLevel = 2;
   
    // Reset Lives to 3 
    playerLives = 3;
   
    levelComplete = false;
   
    // Reset level map
    change_to_level2(lvl, height, width);
   
    
    player_x = 500;
    player_y = 4 * 64 - 102; // (Row 4 * cell_size) - PlayerHeight
   
    velocityY = 0;
    cap_count = 0;
    vacuumActive = false;

   

    // Deactivvate Level 1 enemies
    for(int i = 0; i < ghosts; i++)
    {
        ghost_active[i] = false;
        ghost_x[i] = -1000;
        ghost_y[i] = -1000;
    }

    for(int i = 0; i < skel; i++)
    {
        skel_active[i] = false;
        skel_x[i] = -1000;
        skel_y[i] = -1000;
    }

    // Start Level 2 sequential spawning
    level2EnemiesSpawning = true;
    enemiesSpawned = 0;
    enemySpawnClock.restart();
    level2PowerUpTimer.restart();
    level2PowerUpSpawned = false;
   
    for(int i = 0; i < 4; i++)
    {
        powerup_active[i] = false;
        powerup_has_spawned[i] = false;
    }

    
    platformChangeClock.restart();

    levelComplete = false;
   
}
}
if(currentLevel == 2 && !levelComplete && !playerDead)
{
    float platformElapsed = platformChangeClock.getElapsedTime().asSeconds();
   
    if(platformElapsed >= 20.0f)  // Every 20 seconds
    {
        platformChangeClock.restart();
       
       
       
        // Clear old platform and generate new one
        generate_random_slanted_platform(lvl, height, width, false);
    }
}

for(int i = 0; i < 4; i++)
{
    if(powerup_active[i])
    {
        float bobOffset = sin(powerup_spawn_timer[i].getElapsedTime().asSeconds() * 3.0f) * 5.0f;
        powerup_sprite[i].setPosition(powerup_x[i], powerup_y[i] + bobOffset);
        window.draw(powerup_sprite[i]);
    }
}
window.display();

}
//stopping music and deleting level array (cleanup)
    lvlMusic.stop();
    for (int i = 0; i < height; i++)
        delete[] lvl[i];
    delete[] lvl;
   
    return 0;
}

       
   



