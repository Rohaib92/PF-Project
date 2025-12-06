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
void update_vacuum(float player_x, float player_y, int vacuumDirection, float vacuum_range, float suck_strength, float ghost_x[], float ghost_y[], bool ghost_active[], bool ghost_stunned[], float ghost_stun_timer[], int ghosts, float skel_x[], float skel_y[], bool skel_active[], bool skel_stunned[], float skel_stun_timer[], int skel, const int cell_size, bool vacuum_on, int captured[], int &cap_count, int max_capacity, int &score);
void shoot_single_enemy(float player_x, float player_y, int vacuum_dir, int captured[], int& cap_count, float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], int shot_enemy_type[], bool shot_is_active[], int& shot_count);
void shoot_burst_mode(float player_x, float player_y, int vacuum_dir, int captured[], int& cap_count, float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], int shot_enemy_type[], bool shot_is_active[], int& shot_count);
void update_projectiles(float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], bool shot_is_active[], int shot_count, char** lvl, int cell_size, int height);
bool check_projectile_hits(float shot_enemy_x[], float shot_enemy_y[], bool shot_is_active[], int shot_count, float ghost_x[], float ghost_y[], bool ghost_active[], int total_ghosts, float skel_x[], float skel_y[], bool skel_active[], int total_skels, int& hit_projectile, int& hit_enemy_index, bool& hit_was_ghost);

// Level 2 helper functions
bool is_platform_reachable(char** lvl, int x, int y, int width, int height);
bool check_on_slant(float player_x, float player_y, int player_width, int player_height,
                    float slant_x1[], float slant_y1[], float slant_x2[], float slant_y2[],
                    bool slant_active[], float slant_slide_force[], int slant_count, float& slide_force);


// Check if all enemies are defeated then level  completed
// here ghost_active[] and skel_active[] means alivee
bool check_level_complete(bool ghost_active[], int total_ghosts,
                         bool skel_active[], int total_skels)
{
    // loop through all ghosts if any ghost is alive then level 1 is not ovr yet
    for(int i = 0; i < total_ghosts; i++)
    {
        if(ghost_active[i])
        {
          return false;
        }
    }
   // same for skeletons
    for(int i = 0; i < total_skels; i++)
    {
        if(skel_active[i]) return false;
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
void change_to_level2(char** lvl, int height, int width, float slant_x1[], float slant_y1[],
                     float slant_x2[], float slant_y2[], bool slant_active[],
                     float slant_slide_force[], int& slant_count)
{
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            lvl[i][j] = ' ';
        }
    }
   
    slant_count = 0;
    for(int i = 0; i < 5; i++)
    {
        slant_active[i] = false;
    }
   
    // Boundaries
    for(int j = 0; j < width; ++j)
        lvl[13][j] = '#';
   
    for(int i = 0; i < height; ++i)
        lvl[i][0] = '#';
   
    for(int i = 0; i < height; ++i)
        lvl[i][17] = '#';
   
    for(int j = 0; j < width; ++j)
        lvl[0][j] = '#';
   
    // Starting platform
    for(int j = 7; j <= 10; ++j)
        lvl[11][j] = '#';
   
    // Generate 5-7 random horizontal platforms
    int num_platforms = 5 + rand() % 3;
   
    for(int p = 0; p < num_platforms; p++)
    {
        int attempts = 0;
        while(attempts < 25)
        {
            int px = 2 + rand() % 13;
            int py = 3 + rand() % 8;
            int pwidth = 2 + rand() % 4;
           
            bool valid = true;
           
            for(int j = px; j < px + pwidth && j < width-1; j++)
            {
                if(lvl[py][j] == '#' || (py > 0 && lvl[py-1][j] == '#'))
                {
                    valid = false;
                    break;
                }
            }
           
            if(valid && is_platform_reachable(lvl, px, py, width, height))
            {
                for(int j = px; j < px + pwidth && j < width-1; j++)
                {
                    lvl[py][j] = '#';
                }
                break;
            }
            attempts++;
        }
    }
   
    // Generate 2-3 slanted platforms
    int num_slants = 2 + rand() % 2;
   
    for(int s = 0; s < num_slants && slant_count < 5; s++)
    {
        int attempts = 0;
        while(attempts < 30)
        {
            int start_x = 3 + rand() % 10;
            int start_y = 4 + rand() % 6;
            int length = 3 + rand() % 4;
            int direction = rand() % 2;
           
            bool valid = true;
           
            for(int i = 0; i < length; i++)
            {
                int x = direction == 0 ? start_x + i : start_x - i;
                int y = start_y + i;
               
                if(x <= 0 || x >= width-1 || y >= height-1 || lvl[y][x] == '#')
                {
                    valid = false;
                    break;
                }
            }
           
            if(valid)
            {
                slant_x1[slant_count] = start_x * 64;
                slant_y1[slant_count] = start_y * 64;
               
                if(direction == 0)
                {
                    slant_x2[slant_count] = (start_x + length) * 64;
                    slant_y2[slant_count] = (start_y + length) * 64;
                    slant_slide_force[slant_count] = 1.5f;  // Slide down-right
                }
                else
                {
                    slant_x2[slant_count] = (start_x - length) * 64;
                    slant_y2[slant_count] = (start_y + length) * 64;
                    slant_slide_force[slant_count] = -1.5f;  // Slide down-left
                }
               
                slant_active[slant_count] = true;
                slant_count++;
                break;
            }
            attempts++;
        }
    }
   
    cout << "[LEVEL 2] Generated with " << slant_count << " slanted platforms" << endl;
}
            bool check_on_slant(float player_x, float player_y, int player_width, int player_height,
                    float slant_x1[], float slant_y1[], float slant_x2[], float slant_y2[],
                    bool slant_active[], float slant_slide_force[], int slant_count, float& slide_force)
                   
{
    slide_force = 0.0f;
   
    for(int i = 0; i < slant_count; i++)
    {
        if(!slant_active[i]) continue;
       
        float px = player_x + player_width / 2.0f;
        float py = player_y + player_height;
       
        float dx = slant_x2[i] - slant_x1[i];
        float dy = slant_y2[i] - slant_y1[i];
       
        if(dx == 0) continue;
       
        float slope = dy / dx;
        float intercept = slant_y1[i] - slope * slant_x1[i];
        float line_y = slope * px + intercept;
       
        float min_x = slant_x1[i] < slant_x2[i] ? slant_x1[i] : slant_x2[i];
        float max_x = slant_x1[i] > slant_x2[i] ? slant_x1[i] : slant_x2[i];
       
        if(px >= min_x && px <= max_x && abs(py - line_y) < 25.0f)
        {
            slide_force = slant_slide_force[i];
            return true;
        }
    }
   
    return false;
}
// Next display level 2 window
// window this displays SFML window
// height and width are screen dmensions and cell_size is size of screen in pixels
void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size, float slant_x1[], float slant_y1[], float slant_x2[], float slant_y2[], bool slant_active[], int slant_count)
{
    window.draw(bgSprite);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (lvl[i][j] == '#')
            {
                // Draw block sprite by conerting grids int pixels
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
        }
    }
    // Draw slanted platforms
    for(int i = 0; i < slant_count; i++)
    {
        if(!slant_active[i]) continue;
       
        float dx = slant_x2[i] - slant_x1[i];
        float dy = slant_y2[i] - slant_y1[i];
        float length = sqrt(dx*dx + dy*dy);
        float angle = atan2(dy, dx) * 180.0f / 3.14159f;
       
        RectangleShape slantShape(Vector2f(length, 12));
        slantShape.setPosition(slant_x1[i], slant_y1[i]);
        slantShape.setRotation(angle);
        slantShape.setFillColor(Color(200, 100, 50));
        window.draw(slantShape);
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
void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float slant_x1[], float slant_y1[], float slant_x2[], float slant_y2[], bool slant_active[], float slant_slide_force[], int slant_count, float& slide_velocity_x)
{
    // Check if on slanted platform FIRST (for Level 2)
    float slide_force = 0.0f;
    bool on_slant = check_on_slant(player_x, player_y, Pwidth, Pheight, slant_x1, slant_y1, slant_x2, slant_y2, slant_active, slant_slide_force, slant_count, slide_force);
   
    if(on_slant)
    {
        onGround = true;
        velocityY = 0;
        slide_velocity_x = slide_force;
        return;
    }
    else
    {
        slide_velocity_x = 0.0f;
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
    const int cell_size, bool vacuum_on,
    int captured[], int &cap_count, int max_capacity, int &score)
{
    if (!vacuum_on)
    {
        return;
    }

    float player_center_x = player_x + 96 / 2.0f;
    float player_center_y = player_y + 102 / 2.0f;

    // Vacuum direction as unit vector
    float dir_x = 0, dir_y = 0;
    switch (vacuumDirection)
    {
        case 0: dir_x = -1; dir_y = 0; break; // Left
        case 1: dir_x = 0; dir_y = -1; break; // Up
        case 2: dir_x = 1; dir_y = 0; break;  // Right
        case 3: dir_x = 0; dir_y = 1; break;  // Down
    }

    // CRITICAL: Capture distance (when enemy is close enough to be captured)
    const float capture_distance = 50.0f;

    // Process ghosts
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
                // Stun the ghost (stops its AI movement)
                ghost_stunned[i] = true;
                ghost_stun_timer[i] = 1.5f;

                // FIXED: Only capture if VERY close AND under capacity
                if (distance < capture_distance && cap_count < max_capacity)
                {
                    // Capture ghost (type 0)
                    captured[cap_count] = 0;
                    cap_count = cap_count + 1;
                    ghost_active[i] = false;
                    ghost_stunned[i] = false;
                    ghost_x[i] = -1000;
                    ghost_y[i] = -1000;
                   
                    // Award capture points
                    score = score + 50;
                    cout << "[CAPTURE] Ghost captured! Total: " << cap_count << endl;
                }
                else
                {
                    // FIXED: Pull ghost toward player GRADUALLY
                    ghost_x[i] -= to_ghost_x * suck_strength;
                    ghost_y[i] -= to_ghost_y * suck_strength;
                }
            }
        }
    }

    // Process skeletons (same logic)
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

                // FIXED: Only capture if VERY close AND under capacity
                if (distance < capture_distance && cap_count < max_capacity)
                {
                    // Capture skeleton (type 1)
                    captured[cap_count] = 1;
                    cap_count = cap_count + 1;
                    skel_active[i] = false;
                    skel_stunned[i] = false;
                    skel_x[i] = -1000;
                    skel_y[i] = -1000;
                   
                    // Award capture points
                    score = score + 75;
                    cout << "[CAPTURE] Skeleton captured! Total: " << cap_count << endl;
                }
                else
                {
                    // FIXED: Pull skeleton toward player GRADUALLY
                    skel_x[i] -= to_skel_x * suck_strength;
                    skel_y[i] -= to_skel_y * suck_strength;
                }
            }
        }
    }
}

void shoot_single_enemy(float player_x, float player_y, int vacuum_dir,
                       int captured[], int& cap_count,
                       float shot_enemy_x[], float shot_enemy_y[],
                       float shot_velocity_x[], float shot_velocity_y[],
                       int shot_enemy_type[], bool shot_is_active[], int& shot_count)
{
    if (cap_count <= 0)
    {
        return;
    }

    // LIFO: Pop last captured enemy
    cap_count = cap_count - 1;
    int shot_type = captured[cap_count];

    // Create new projectile
    shot_enemy_x[shot_count] = player_x;
    shot_enemy_y[shot_count] = player_y;
    shot_enemy_type[shot_count] = shot_type; // Use actual enemy type
    shot_is_active[shot_count] = true;

    float projectile_speed = 10.0f;

    // Set velocity based on vacuum direction
    if (vacuum_dir == 0) // left
    {
        shot_velocity_x[shot_count] = -projectile_speed;
        shot_velocity_y[shot_count] = 0.0f;
    }
    else if (vacuum_dir == 1) // up
    {
        shot_velocity_x[shot_count] = 0.0f;
        shot_velocity_y[shot_count] = -projectile_speed;
    }
    else if (vacuum_dir == 2) // right
    {
        shot_velocity_x[shot_count] = projectile_speed;
        shot_velocity_y[shot_count] = 0.0f;
    }
    else // down (3)
    {
        shot_velocity_x[shot_count] = 0.0f;
        shot_velocity_y[shot_count] = projectile_speed;
    }

    shot_count = shot_count + 1;
    if (shot_count >= 20)
    {
        shot_count = 0;
    }
}

void shoot_burst_mode(float player_x, float player_y, int vacuum_dir,
                     int captured[], int& cap_count,
                     float shot_enemy_x[], float shot_enemy_y[],
                     float shot_velocity_x[], float shot_velocity_y[],
                     int shot_enemy_type[], bool shot_is_active[], int& shot_count)
{
    // Shoot all captured enemies in LIFO order
    while (cap_count > 0)
    {
        shoot_single_enemy(player_x, player_y, vacuum_dir, captured, cap_count,
                          shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                          shot_enemy_type, shot_is_active, shot_count);
    }
}

// Update all projectile positions and physics
void update_projectiles(float shot_enemy_x[], float shot_enemy_y[],
                       float shot_velocity_x[], float shot_velocity_y[],
                       bool shot_is_active[], int shot_count,
                       char** lvl, int cell_size, int height)
{
    for (int i = 0; i < 20; i++)
    {
        if (!shot_is_active[i])
        {
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
        }

        // Bounce at top
        if (shot_enemy_y[i] < 0)
        {
            shot_velocity_y[i] = -shot_velocity_y[i] * 0.8f;
            shot_enemy_y[i] = 0;
        }

        // Bounce at bottom
        if (shot_enemy_y[i] > (height - 1) * cell_size)
        {
            shot_velocity_y[i] = -8.0f;
            shot_enemy_y[i] = (height - 1) * cell_size - 48;
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
            }
        }
    }
}

// Check if projectiles hit any active enemies
bool check_projectile_hits(float shot_enemy_x[], float shot_enemy_y[],
                          bool shot_is_active[], int shot_count,
                          float ghost_x[], float ghost_y[], bool ghost_active[], int total_ghosts,
                          float skel_x[], float skel_y[], bool skel_active[], int total_skels,
                          int& hit_projectile, int& hit_enemy_index, bool& hit_was_ghost)
{
    hit_projectile = -1;
    hit_enemy_index = -1;
    hit_was_ghost = false;

    // Check all active projectiles
    for (int proj = 0; proj < 20; proj++)
    {
        if (!shot_is_active[proj])
        {
            continue;
        }

        // Check collision with ghosts
        for (int g = 0; g < total_ghosts; g++)
        {
            if (!ghost_active[g])
            {
                continue;
            }

            float dx = shot_enemy_x[proj] - ghost_x[g];
            float dy = shot_enemy_y[proj] - ghost_y[g];
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 48)
            {
                hit_projectile = proj;
                hit_enemy_index = g;
                hit_was_ghost = true;
                return true;
            }
        }

        // Check collision with skeletons
        for (int s = 0; s < total_skels; s++)
        {
            if (!skel_active[s])
            {
                continue;
            }

            float dx = shot_enemy_x[proj] - skel_x[s];
            float dy = shot_enemy_y[proj] - skel_y[s];
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < 48)
            {
                hit_projectile = proj;
                hit_enemy_index = s;
                hit_was_ghost = false;
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
   
    // Slanted platform arrays
const int MAX_SLANTS = 5;
float slant_x1[5];
float slant_y1[5];
float slant_x2[5];
float slant_y2[5];
bool slant_active[5];
float slant_slide_force[5];
int slant_count = 0;

for(int i = 0; i < MAX_SLANTS; i++)
{
    slant_active[i] = false;
}

float slide_velocity_x = 0.0f;


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
    bgTex.loadFromFile("bg.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);

    blockTexture.loadFromFile("block1.png");
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
if(!scoreFont.loadFromFile("Roboto-Regular.ttf"))
{
    // Font loading failed - handle error if needed
    cout << "Error loading font!" << endl;
}

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
   
    descTextures[0].loadFromFile("greendesc.png");
    descTextures[1].loadFromFile("yellowdesc.png");
   
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
    walkTextures[0].loadFromFile("player1walk1.png");
    walkTextures[1].loadFromFile("player1walk2.png");
    walkTextures[2].loadFromFile("player1walk3.png");
    walkTextures[3].loadFromFile("player1walk4.png");
}
else if(selectedIndex == 1)
{
    walkTextures[0].loadFromFile("player2walk1.png");
    walkTextures[1].loadFromFile("player2walk2.png");
    walkTextures[2].loadFromFile("player2walk3.png");
    walkTextures[3].loadFromFile("player2walk4.png");
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
  float baseSpeed = 5;
    speed = get_character_speed(selectedIndex, baseSpeed);
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
     const int ghostAnimationFrames = 4;
     Texture ghostWalkTextures[4];
     int ghostCurrentFrame[8];      // cuuurrent frame for each ghost
int ghostFrameCounter[8];      // Frame counter for each ghost
const int ghostFrameDelay = 10; // aniimation speed

// Load ghost walking animation frames
ghostWalkTextures[0].loadFromFile("ghost1.png");
ghostWalkTextures[1].loadFromFile("ghost2.png");
ghostWalkTextures[2].loadFromFile("ghost3.png");
ghostWalkTextures[3].loadFromFile("ghost4.png");


     
   
   
   
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
 
    float skel_jump_timer[4];      // Timer for each skeleton
    float skel_next_jump_time[4];  // Random time until next jump (4-5 seconds)
    Texture skelTexture;
    skelTexture.loadFromFile("skeleton.png");
   
    // ===== ENEMY STUN SYSTEM ===== (FIXED)
    bool ghost_stunned[8];     // One for each ghost
    float ghost_stun_timer[8];
    bool skel_stunned[4];      // One for each skeleton
    float skel_stun_timer[4];
   
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
        skel_next_jump_time[i] = 4.0f + (rand() % 2);                // Random 4-5 seconds
       
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
    int shot_projectile_count = 0;

    // Initialize shot array
    for (int i = 0; i < 20; i++)
    {
        shot_is_active[i] = false;
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
    invisTexture.loadFromFile("invisible.png");  // You need this image file!

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

    Texture chelnovTexture;
    chelnovTexture.loadFromFile("chelnov.png");  // You need this image file!

    for(int i = 0; i < chelnovs; i++)
    {
        chelnov_active[i] = false;  // Start inactive
        chelnov_stunned[i] = false;
        chelnov_stun_timer[i] = 0.0f;
        chelnov_speed[i] = 1.2f;
        chelnov_dir[i] = 1;
        chelnovSprite[i].setTexture(chelnovTexture);
        chelnovSprite[i].setScale(2, 2);
    }

    // Sequential spawn tracking for Level 2
    Clock enemySpawnClock;
    int enemiesSpawned = 0;
    const float spawnInterval = 2.5f;  // Spawn an enemy every 2.5 seconds
    bool level2EnemiesSpawning = false;
   
     // ===== SUCKING CAPACITY =====
     int captured[10];
    int cap_count = 0;           // Current count of sucked enemies
    const int maxCapacity = 3;       // Maximum enemies that can be sucked at once
   

// ADD DELTA TIME CLOCK FOR SKELETON JUMP TIMING:
    Clock deltaClock;
    //main loop
    while (window.isOpen())
    {
    float deltaTime = deltaClock.restart().asSeconds();
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
       display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size, slant_x1, slant_y1, slant_x2, slant_y2, slant_active, slant_count);
        player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, slant_x1, slant_y1, slant_x2, slant_y2, slant_active, slant_slide_force, slant_count, slide_velocity_x);

        ///*** // Moving the character left and right using arrow keys'
        if(!playerDead) // Only allow controls if alive
        {
        isWalking = false;
        if(Keyboard::isKeyPressed(Keyboard::Key::Right ))
        {
            // attempt to move right with collision check
            player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            player_x += slide_velocity_x;  // Apply sliding from slanted platforms
            bagSprite.setScale(-2,2);
            PlayerSprite.setScale(-3,3);
            facingRight = true;// flip horizontally to face right
            isWalking=true;
        }
       
        if(Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
            // attempt to move left
            player_left_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            player_x += slide_velocity_x;  // Apply sliding from slanted platforms
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
            update_vacuum(
                player_x, player_y, vacuumDirection, 200.0f, 4.0f,
                ghost_x, ghost_y, ghost_active, ghost_stunned, ghost_stun_timer, ghosts,
                skel_x, skel_y, skel_active, skel_stunned, skel_stun_timer, skel,
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
                                  shot_enemy_type, shot_is_active, shot_projectile_count);
                cout << "[SHOOT] Single shot! Remaining captured: " << cap_count << endl;
            }
        }

        // Q key = Burst Shot (shoot ALL captured enemies at once)
        if (Q_key_pressed && !previous_Q_pressed)
        {
            if (cap_count > 0)
            {
                shoot_burst_mode(player_x, player_y, vacuumDirection, captured, cap_count,
                                shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                                shot_enemy_type, shot_is_active, shot_projectile_count);
                cout << "[BURST] All enemies shot!" << endl;
            }
        }


// Check if any projectile hit an enemy
        int hit_projectile = -1;
        int hit_enemy_index = -1;
        bool hit_was_ghost = false;

        if (check_projectile_hits(shot_enemy_x, shot_enemy_y, shot_is_active, shot_projectile_count,
                                  ghost_x, ghost_y, ghost_active, ghosts,
                                  skel_x, skel_y, skel_active, skel,
                                  hit_projectile, hit_enemy_index, hit_was_ghost))
        {
            if (hit_projectile >= 0 && hit_enemy_index >= 0)
            {
                // Deactivate the projectile
                shot_is_active[hit_projectile] = false;

                if (hit_was_ghost)
                {
                    // Deactivate the ghost
                    ghost_active[hit_enemy_index] = false;
                    ghost_x[hit_enemy_index] = -1000;
                    ghost_y[hit_enemy_index] = -1000;
                   
                    // Award bonus points for defeating with projectile
                    score += 100;  // Ghost defeat bonus
                    combo++;
                   
                    cout << "[HIT] Ghost defeated by projectile! Score: " << score << endl;
                }
                else
                {
                    // Deactivate the skeleton
                    skel_active[hit_enemy_index] = false;
                    skel_x[hit_enemy_index] = -1000;
                    skel_y[hit_enemy_index] = -1000;
                   
                    // Award bonus points for defeating with projectile
                    score += 150;  // Skeleton defeat bonus (worth more)
                    combo++;
                   
                    cout << "[HIT] Skeleton defeated by projectile! Score: " << score << endl;
                }
            }
        }


        // Update key states for next frame
        previous_E_pressed = E_key_pressed;
        previous_Q_pressed = Q_key_pressed;

        // Update all projectile physics
        update_projectiles(shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                          shot_is_active, shot_projectile_count, lvl, cell_size, height);

       
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



  // ============= DRAW PROJECTILES =============
        // Draw all active projectiles (shot enemies)
        for (int i = 0; i < 20; i++)
        {
            if (shot_is_active[i])
            {
                // Draw projectile using ghost sprite with red tint
                ghostSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
                ghostSprite[0].setColor(Color(255, 100, 100)); // Red tint for projectiles
                window.draw(ghostSprite[0]);
                ghostSprite[0].setColor(Color::White); // Reset color back to white
            }
        }

// ====== INVISIBLE MAN MOVEMENT (Level 2) ======
for(int i = 0; i < invisible_men; i++)
{
    if(!invis_active[i]) continue;
   
    if(invis_stunned[i])
    {
        invisSprite[i].setPosition(invis_x[i], invis_y[i]);
        window.draw(invisSprite[i]);
        continue;
    }
   
    // Simple left-right movement
    invis_x[i] += invis_speed[i] * invis_dir[i];
   
    // Check boundaries and turn around
    int tile_x = invis_x[i] / cell_size;
    int tile_y = invis_y[i] / cell_size;
   
    if(tile_x <= 1 || tile_x >= width-2)
    {
        invis_dir[i] *= -1;
    }
   
    invisSprite[i].setPosition(invis_x[i], invis_y[i]);
    window.draw(invisSprite[i]);
}

// ====== CHELNOV MOVEMENT (Level 2) ======
for(int i = 0; i < chelnovs; i++)
{
    if(!chelnov_active[i]) continue;
   
    if(chelnov_stunned[i])
    {
        chelnovSprite[i].setPosition(chelnov_x[i], chelnov_y[i]);
        window.draw(chelnovSprite[i]);
        continue;
    }
   
    // Simple left-right movement
    chelnov_x[i] += chelnov_speed[i] * chelnov_dir[i];
   
    // Check boundaries and turn around
    int tile_x = chelnov_x[i] / cell_size;
    int tile_y = chelnov_y[i] / cell_size;
   
    if(tile_x <= 1 || tile_x >= width-2)
    {
        chelnov_dir[i] *= -1;
    }
   
    chelnovSprite[i].setPosition(chelnov_x[i], chelnov_y[i]);
    window.draw(chelnovSprite[i]);
}
        // ====== GHOST MOVEMENT
       for(int i = 0; i < ghosts; i++)
{
    if(!ghost_active[i]) continue; // skip inactive ghosts

 if(ghost_stunned[i])
    {
        // Just draw the ghost in place, don't move it
        ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
        window.draw(ghostSprite[i]);
        continue;  // Skip all movement logic
    }

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
        if(ghost_dir[i] == 1) // Now moving right
    {
        ghostSprite[i].setScale(-2, 2);
       
    }
   
   
    else // Now moving left
    {
        ghostSprite[i].setScale(2, 2);
               

    }
    }
    else
       
        {
            ghost_x[i] = nextX;
        }
       
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


       // ====== SKELETON MOVEMENT WITH GRAVITY AND TIMED JUMPING ======
        for(int j = 0; j < skel; j++)
        {
            if(!skel_active[j]) continue;  // skip inactive skeletons

            if(skel_stunned[j])
            {
                // Just draw the skeleton in place, don't move it
                skelSprite[j].setPosition(skel_x[j], skel_y[j]);
                window.draw(skelSprite[j]);
                continue;  // Skip all movement logic
            }

            // Update jump timer
            skel_jump_timer[j] += deltaTime;

            // Apply gravity first
            if(!skel_onGround[j])
            {
                skel_velocityY[j] += gravity;
                if(skel_velocityY[j] > terminal_Velocity)
                {
                    skel_velocityY[j] = terminal_Velocity;
                }
            }

            // GRAVITY - predict next vertical position
            float nextY = skel_y[j] + skel_velocityY[j];

            // Check ground collision when FALLING DOWN only (same as player)
            if(skel_velocityY[j] > 0)
            {
                // Check ground collision (bottom of skeleton)
                int bottomLeftX = skel_x[j] / cell_size;
                int bottomRightX = (skel_x[j] + 64) / cell_size;
                int bottomY = (nextY + 64) / cell_size;

                if(lvl[bottomY][bottomLeftX] == '#' || lvl[bottomY][bottomRightX] == '#')
                {
                    // landed on platform
                    skel_onGround[j] = true;
                    skel_velocityY[j] = 0;
                    // Don't update position - stay on platform
                }
                else
                {
                    // falling
                    skel_y[j] = nextY;
                    skel_onGround[j] = false;
                }
            }
            else
            {
                // When jumping UP, move freely through blocks (like player)
                skel_y[j] = nextY;
                skel_onGround[j] = false;
            }

            // HORIZONTAL MOVEMENT - only move left/right if on ground
            if(skel_onGround[j])
            {
                // CHECK IF IT'S TIME TO JUMP (every 4-5 seconds)
                if(skel_jump_timer[j] >= skel_next_jump_time[j])
                {
                    // JUMP! - Give skeleton upward velocity
                    skel_velocityY[j] = -18.0f;  // Jump strength (same as player)
                    skel_onGround[j] = false;
                   
                    // Reset timer and set next random jump time (4-5 seconds)
                    skel_jump_timer[j] = 0.0f;
                    skel_next_jump_time[j] = 4.0f + (rand() % 2);  // Random 4 or 5 seconds
                   
                    // Randomly change direction 50% of the time
                    if(rand() % 2 == 0)
                    {
                        skel_dir[j] *= -1;  // Turn around while jumping
                    }
                }
                else
                {
                    // Normal walking - check for walls
                    float nextp = skel_x[j] + skel_speed[j] * skel_dir[j];

                    // Check the tile directly in front of the skeleton
                    int frontskelX = (nextp + (skel_dir[j] == 1 ? 64 : 0)) / cell_size;
                    int midTileY = (skel_y[j] + 32) / cell_size;

                    // Check if there's a gap ahead (no ground to walk on)
                    int edgeCheckX = (nextp + (skel_dir[j] == 1 ? 64 : 0)) / cell_size;
                    int edgeCheckY = (skel_y[j] + 64 + 1) / cell_size;

                    // Turn around if there's a wall OR edge ahead
                    if(lvl[midTileY][frontskelX] == '#' || lvl[edgeCheckY][edgeCheckX] != '#')
                    {
                        skel_dir[j] *= -1;  // Turn around
                    }
                    else
                    {
                        // No obstacle - keep walking
                        skel_x[j] = nextp;
                    }
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
 
 // ====== LEVEL 2 SEQUENTIAL ENEMY SPAWNING ======
if(currentLevel == 2 && level2EnemiesSpawning)
{
    float spawnElapsed = enemySpawnClock.getElapsedTime().asSeconds();
   
    if(spawnElapsed >= spawnInterval)
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
                    cout << "[SPAWN] Invisible Man " << (idx+1) << " spawned!" << endl;
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
                    cout << "[SPAWN] Chelnov " << (idx+1) << " spawned!" << endl;
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
                    cout << "[SPAWN] Ghost " << (idx+1) << " spawned!" << endl;
                    break;
                }
            }
        }
        // Finally spawn skeletons (11-19)
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
                    skelSprite[idx].setPosition(skel_x[idx], skel_y[idx]);
                    cout << "[SPAWN] Skeleton " << (idx+1) << " spawned!" << endl;
                    break;
                }
            }
        }
       
        enemiesSpawned++;
       
        if(enemiesSpawned >= 20)
        {
            level2EnemiesSpawning = false;
            cout << "[LEVEL 2] All enemies spawned!" << endl;
        }
    }
}
// ====== CHECK LEVEL COMPLETION ======
if(!levelComplete && !playerDead)
{
    if(currentLevel == 1)
    {
        if(check_level_complete(ghost_active, ghosts, skel_active, skel))
        {
            levelComplete = true;
            levelCompleteClock.restart();
            score += 500;
            cout << "[LEVEL] Level 1 Complete!" << endl;
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
            cout << "[LEVEL] Level 2 Complete!" << endl;
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
   
    // After 3 seconds, load Level 2
    if(elapsed >= levelCompleteDelay)
    {
        currentLevel = 2;
       
        // Change to Level 2 layout
     change_to_level2(lvl, height, width, slant_x1, slant_y1, slant_x2, slant_y2, slant_active, slant_slide_force, slant_count);
       
        // Reset player position
        player_x = 500;
        player_y = 150;
        velocityY = 0;
        cap_count = 0;
        vacuumActive = false;
       
        // Deactivate ALL enemies (Level 2 has no enemies)
        // Deactivate Level 1 enemies
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
       
        levelComplete = false;
        cout << "[LEVEL] Now in Level 2 - No Enemiegs!" << endl;
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

       
   


