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

float get_character_speed(int characterIndex, float baseSpeed)
{
    if(characterIndex == 0)
    {
        return baseSpeed * 1.5f;  
    }
    else if(characterIndex == 1)
    {
        return baseSpeed * 1.2f;  
    }
    return baseSpeed;  
}


void update_vacuum(float player_x, float player_y, int vacuumDirection, float vacuum_range, float suck_strength,
    float ghost_x[], float ghost_y[], bool ghost_active[], bool ghost_stunned[], float ghost_stun_timer[], int ghosts, float skel_x[], float skel_y[], bool skel_active[], bool skel_stunned[], float skel_stun_timer[], int skel, float invis_x[], float invis_y[], bool invis_active[], bool invis_stunned[], float invis_stun_timer[], int invis_count, float chelnov_x[], float chelnov_y[], bool chelnov_active[], bool chelnov_stunned[], float chelnov_stun_timer[], int chelnov_count,
bool chelnov_is_shooting[], const int cell_size, bool vacuum_on, int captured[], int &cap_count, int max_capacity, int &score);


    void generate_random_slanted_platform(char** lvl, int height, int width, bool clearOld);
    
    
void shoot_single_enemy(float player_x, float player_y, int vacuum_dir, int captured[], int& cap_count, float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], int shot_enemy_type[], bool shot_is_active[], int& shot_count);


void shoot_burst_mode(float player_x, float player_y, int vacuum_dir, int captured[], int& cap_count, float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], int shot_enemy_type[], bool shot_is_active[], int& shot_count);


void update_projectiles(float shot_enemy_x[], float shot_enemy_y[], float shot_velocity_x[], float shot_velocity_y[], bool shot_is_active[], float shot_lifetime[], float deltaTime, float max_lifetime, int shot_count, char** lvl, int cell_size, int height);


bool check_projectile_hits(float shot_enemy_x[], float shot_enemy_y[], bool shot_is_active[], int shot_count,float ghost_x[], float ghost_y[], bool ghost_active[], int total_ghosts,float skel_x[], float skel_y[], bool skel_active[], int total_skels,float invis_x[], float invis_y[], bool invis_active[], int total_invis, float chelnov_x[], float chelnov_y[], bool chelnov_active[], int total_chelnov, int& hit_projectile, int& hit_enemy_index, int& hit_enemy_type);

bool is_platform_reachable(char** lvl, int x, int y, int width, int height);

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

void change_to_level2(char** lvl, int height, int width)
{ 
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            lvl[i][j] = ' ';
        }
    }
   
    // Bottom floor 
    for(int j = 0; j < width; ++j)
        lvl[height-6][j] = '#';
   
    // Left wall
    for(int i = 0; i < height-6; ++i)
        lvl[i][0] = '#';
   
    // Right wall
    for(int i = 0; i < height-6; ++i)
        lvl[i][width-1] = '#';
   
    // Top ceiling 
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
  
    int length = 5;
    int attempts = 0;
    bool placed = false;
   
    while(attempts < 100 && !placed)
    {
        int direction = rand() % 2;
        int start_x, start_y;
       
        start_y = 6 + (rand() % 2);
       
        if (direction == 0) { 
            start_x = 4 + rand() % 3;
        } else { // \ Ramp (Up-Left)
            start_x = 10 + rand() % 3;
        }
       
        bool valid = true;
       
        for(int i = 0; i < length; i++)
        {
            int y = start_y - i;
           
         
            int x_slope = (direction == 0) ? start_x + i : start_x - i;
            int x_block = (direction == 0) ? x_slope + 1 : x_slope - 1;  
           
          
            if(x_slope <= 0 || x_slope >= width-1 || x_block <= 0 || x_block >= width-1 || y <= 0 || y >= height-6)
            {
                valid = false;
                break;
            }
            
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
                int x_block = (direction == 0) ? x_slope + 1 : x_slope - 1;  
               
               
                lvl[y][x_slope] = slope_char;
                lvl[y][x_block] = '#';
            }
           
            placed = true;
            
        }
       
        attempts++;
    }
   
    if(!placed)
    {
       
        int backup_y = 4;
        for(int j = 7; j <= 10; ++j) {
            lvl[backup_y][j] = '#';
        }
    }
}


void display_level(RenderWindow& window, char**lvl, Texture& bgTex, Sprite& bgSprite,
                   Texture& blockTexture, Sprite& blockSprite,
                   const int height, const int width, const int cell_size)
{
    window.draw(bgSprite);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
           
            if (lvl[i][j] == '#')
            {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
           
            else if (lvl[i][j] == '/')
            {
              
                int mini_size = cell_size / 4;  
               
                blockSprite.setScale(0.25f, 0.25f);  
               
                
                for(int k = 0; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 48);
                    window.draw(blockSprite);
                }
               
               
                for(int k = 1; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 32);
                    window.draw(blockSprite);
                }
               
                for(int k = 2; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 16);
                    window.draw(blockSprite);
                }
               
               
                blockSprite.setPosition(j * cell_size + 48, i * cell_size);
                window.draw(blockSprite);
             
                blockSprite.setScale(1.0f, 1.0f);
            }
         
            else if (lvl[i][j] == '\\')
            {
                int mini_size = cell_size / 4;  
               
                blockSprite.setScale(0.25f, 0.25f);  
               
               
                for(int k = 0; k < 4; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 48);
                    window.draw(blockSprite);
                }
               
               
                for(int k = 0; k < 3; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 32);
                    window.draw(blockSprite);
                }
               
               
                for(int k = 0; k < 2; k++)
                {
                    blockSprite.setPosition(j * cell_size + k * mini_size,
                                           i * cell_size + 16);
                    window.draw(blockSprite);
                }
               
                
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
               
               
                blockSprite.setScale(1.0f, 1.0f);
            }
        }
    }
}


void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
    int col_mid = (player_x + Pwidth / 2) / cell_size;
int row_feet = (player_y + Pheight) / cell_size;


char tile_center = lvl[row_feet][col_mid];
char tile_left = lvl[row_feet][col_mid - 1];
char tile_right = lvl[row_feet][col_mid + 1];


if (row_feet >= 0 && row_feet < Pheight && col_mid >= 1 && col_mid < Pwidth - 1)
{
    if (tile_center == '/' || tile_left == '/' || tile_right == '/')
    {
        
        player_x -= 2.5f; 
        velocityY = 1.5f;
        onGround = false;
        player_y += velocityY;
        return;
    }
    else if (tile_center == '\\' || tile_left == '\\' || tile_right == '\\')
    {
       
        player_x += 2.5f; 
        velocityY = 1.5f;
        onGround = false;
        player_y += velocityY;
        return;
    }
}
    
    if (!onGround)
    {
        
        velocityY += gravity;
       
        if (velocityY >= terminal_Velocity)
        {
            velocityY = terminal_Velocity;
        }
    }
   
 
    offset_y = player_y;
   
    offset_y += velocityY;
   
   
    if (velocityY < 0)  
    {
        int top_y = (int)(offset_y) / cell_size;
       
       
        if(top_y <= 0)
        {
            int left_x = (int)(player_x) / cell_size;
            int mid_x = (int)(player_x + Pwidth / 2) / cell_size;
            int right_x = (int)(player_x + Pwidth) / cell_size;
           
           
            if(lvl[0][left_x] == '#' || lvl[0][mid_x] == '#' || lvl[0][right_x] == '#')
            {
                
                velocityY = 0;
                player_y = cell_size;  
                onGround = false;
                return;
            }
        }
       
       
        player_y = offset_y;
        onGround = false;
        return;
    }
   
   
    if (velocityY > 0)
    {
       
        char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
       
        char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
      
        char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

        if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
        {
          
            onGround = true;
            velocityY = 0;
           
        }
        else
        {
            
            player_y = offset_y;
            onGround = false;
        }
    }
    else
    {
        
        player_y = offset_y;
        onGround = false;
    }
}


void player_left_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
   
    offset_x = player_x;    
    offset_x -= speed;      

   
    char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
    char left_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];

    if (left_top == '#' || left_mid == '#' || left_bottom == '#')
    {
       
        offset_x = player_x ;
    }
    else
    {
       
        player_x = offset_x;
    }
}


void player_right_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x;
    offset_x += speed;

    
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

void player_ceiling_collision(char** lvl, float& offset_y, float& velocityY, float& player_x, float& player_y, const int cell_size, int& Pwidth)
{
    
    offset_y = player_y;
    offset_y += velocityY;

  
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

bool check_player_enemy_collision(
    float player_x, float player_y, int PlayerWidth, int PlayerHeight,
    float enemy_x, float enemy_y, int enemy_size)
{
  
    return (player_x < enemy_x + enemy_size &&
            player_x + PlayerWidth > enemy_x &&
            player_y < enemy_y + enemy_size &&
            player_y + PlayerHeight > enemy_y);
}


void respawn_player(float& player_x, float& player_y, float& velocityY, bool& playerDead)
{
    player_x = 500;       
    player_y = 150;
    velocityY = 0;       
    playerDead = false;  
}

bool check_powerup_collision(float player_x, float player_y, int PlayerWidth, int PlayerHeight,
                            float powerup_x, float powerup_y, int powerup_size)
{
    return (player_x < powerup_x + powerup_size &&
            player_x + PlayerWidth > powerup_x &&
            player_y < powerup_y + powerup_size &&
            player_y + PlayerHeight > powerup_y);
}

void spawn_powerup(float& powerup_x, float& powerup_y, bool& powerup_active,
                   int& powerup_type, Sprite& powerup_sprite, Clock& spawn_timer,
                   bool& has_spawned, char** lvl, int width, int height,
                   int cell_size, int type, Texture& texture)
{
    if(has_spawned) return;  
   
    powerup_type = type;
    powerup_sprite.setTexture(texture);
    powerup_sprite.setScale(2, 2);
   
 
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
   
    
    powerup_x = 400;
    powerup_y = 300;
    powerup_active = true;
    has_spawned = true;
    spawn_timer.restart();
}

void update_vacuum(
    float player_x, float player_y, int vacuumDirection,
    float vacuum_range, float suck_strength,
    float ghost_x[], float ghost_y[], bool ghost_active[], bool ghost_stunned[], float ghost_stun_timer[], int ghosts,
    float skel_x[], float skel_y[], bool skel_active[], bool skel_stunned[], float skel_stun_timer[], int skel,
    float invis_x[], float invis_y[], bool invis_active[], bool invis_stunned[], float invis_stun_timer[], int invis_count,
    float chelnov_x[], float chelnov_y[], bool chelnov_active[], bool chelnov_stunned[], float chelnov_stun_timer[], int chelnov_count,
    bool chelnov_is_shooting[],  
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
                    captured[cap_count] = 2;  
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


for (int i = 0; i < chelnov_count; i++)
{
    if (!chelnov_active[i]) continue;
   
    
    if (chelnov_is_shooting[i])
    {
        continue; 
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
                       float shot_lifetime[],  
                       int& shot_count)
{
    if (cap_count <= 0)
    {
        return;
    }

    cap_count = cap_count - 1;
    int shot_type = captured[cap_count];
   
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
    
    if (vacuum_dir == 0) 
    {
        shot_velocity_x[slot] = -projectile_speed;
        shot_velocity_y[slot] = 0.0f;
    }
    else if (vacuum_dir == 1) 
    {
        shot_velocity_x[slot] = 0.0f;
        shot_velocity_y[slot] = -projectile_speed;
    }
    else if (vacuum_dir == 2) 
    {
        shot_velocity_x[slot] = projectile_speed;
        shot_velocity_y[slot] = 0.0f;
    }
    else 
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
                     float shot_lifetime[],  
                     int& shot_count)
{
  
   
    
    while (cap_count > 0)
    {
        shoot_single_enemy(player_x, player_y, vacuum_dir, captured, cap_count,
                          shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                          shot_enemy_type, shot_is_active, shot_lifetime, shot_count);  
    }
   
  
}

void update_projectiles(float shot_enemy_x[], float shot_enemy_y[],
                       float shot_velocity_x[], float shot_velocity_y[],
                       bool shot_is_active[], float shot_lifetime[],  
                       float deltaTime, float max_lifetime,  
                       int shot_count,
                       char** lvl, int cell_size, int height)
{
    for (int i = 0; i < 20; i++)
    {
        if (!shot_is_active[i])
        {
            continue;
        }

        
        shot_lifetime[i] += deltaTime;
       
        if (shot_lifetime[i] >= max_lifetime)
        {
            shot_is_active[i] = false;
           
            continue;
        }

      
        shot_enemy_x[i] = shot_enemy_x[i] + shot_velocity_x[i];
        shot_enemy_y[i] = shot_enemy_y[i] + shot_velocity_y[i];

        
        if (shot_velocity_x[i] != 0.0f)
        {
            shot_velocity_y[i] = shot_velocity_y[i] + 0.3f;
        }

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

      
        int col = (int)(shot_enemy_x[i] / cell_size);
        int row = (int)(shot_enemy_y[i] / cell_size);

        if (row >= 0 && row < height && col >= 0 && col < 18)
        {
            if (lvl[row][col] == '#')
            {
              
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
   
 


int currentLevel = 1;
bool levelComplete = false;
Clock levelCompleteClock;
const float levelCompleteDelay = 3.0f;         

  
    Texture bgTex;
    Sprite bgSprite;
    Texture blockTexture;
    Sprite blockSprite;

    bgTex.loadFromFile("Assets/bg/bg.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0,0);

    blockTexture.loadFromFile("Assets/bg/block1.png");
    blockSprite.setTexture(blockTexture);

    float player_x = 500;
    float player_y = 150;

    float speed = 5;

    const float jumpStrength = -20; 
    const float gravity = 1;        
    bool onGround = false;

    float offset_x = 0;
    float offset_y = 0;
    float velocityY = 0;

    float terminal_Velocity = 20;   

    int PlayerHeight = 102;
    int PlayerWidth = 96;
   
    
int score = 0;
int combo = 0;  
float comboMultiplier = 1.0f;
bool tookDamage = false; 

Font scoreFont;
Text scoreText;
Text comboText;

Clock levelTimer;  
scoreFont.loadFromFile("Assets/txt/Roboto-Regular.ttf");

scoreText.setFont(scoreFont);
scoreText.setCharacterSize(30);
scoreText.setFillColor(Color::White);
scoreText.setPosition(20, 10);

comboText.setFont(scoreFont);
comboText.setCharacterSize(25);
comboText.setFillColor(Color::Yellow);
comboText.setPosition(20, 50);


levelTimer.restart();  
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
const int frameDelay = 10;      
bool isWalking = false;        

    // Music objects and settings
    Music menuMusic;
    Music lvlMusic;
    menuMusic.openFromFile("Assets/music/bgmus.ogg");
    menuMusic.setLoop(true);
    menuMusic.setVolume(40);

    lvlMusic.openFromFile("Assets/music/mus.ogg");
    lvlMusic.setLoop(true);
    lvlMusic.setVolume(40);
   

    //PLAYER SELECTION MENnU
    const int playerOptionsCount = 2;   
    Texture playerOptions[playerOptionsCount];
    Sprite playerOptionSprite[playerOptionsCount];

    playerOptions[0].loadFromFile("Assets/players/player1.png");
    playerOptions[1].loadFromFile("Assets/players/player2.png");

const int bagOptionsCount = 2;
Texture bagOptions[bagOptionsCount];
Sprite bagOptionSprite[bagOptionsCount];
bagOptions[0].loadFromFile("Assets/props/bag1.png");
bagOptions[1].loadFromFile("Assets/props/bag2.png");
    /// Set positions and scale for menu display
    for(int i = 0; i < playerOptionsCount; i++)
    {
        bagOptionSprite[i].setTexture(bagOptions[i]);
        bagOptionSprite[i].setScale(2,2);
        bagOptionSprite[i].setPosition(300 + i*300+73, 200+41);  
        playerOptionSprite[i].setTexture(playerOptions[i]);
        playerOptionSprite[i].setScale(3,3);
        playerOptionSprite[i].setPosition(300 + i*300, 200); 
    }

    Texture descTextures[2];
    Sprite descSprites[2];
   
    descTextures[0].loadFromFile("Assets/txt/greendesc.png");
    descTextures[1].loadFromFile("Assets/txt/yellowdesc.png");
   
    descSprites[0].setTexture(descTextures[0]);
    descSprites[0].setPosition(200, 300);  
   
    descSprites[1].setTexture(descTextures[1]);
    descSprites[1].setPosition(500, 300);  

   
    int selectedIndex = 0;
    bool playerChosen = false;
    menuMusic.play(); // play menu music

  
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

       
        PlayerTexture = playerOptions[selectedIndex];
        PlayerSprite.setTexture(PlayerTexture);
        PlayerSprite.setScale(3,3);
        PlayerSprite.setPosition(player_x, player_y);
        // next Loading walking animation frames based on selected player
if(selectedIndex == 0)
{
    walkTextures[0].loadFromFile("Assets/players/player1walk1.png");
    walkTextures[1].loadFromFile("Assets/players/player1walk2.png");
    walkTextures[2].loadFromFile("Assets/players/player1walk3.png");
    walkTextures[3].loadFromFile("Assets/players/player1walk4.png");
}
else if(selectedIndex == 1)
{
    walkTextures[0].loadFromFile("Assets/players/player2walk1.png");
    walkTextures[1].loadFromFile("Assets/players/player2walk2.png");
    walkTextures[2].loadFromFile("Assets/players/player2walk3.png");
    walkTextures[3].loadFromFile("Assets/players/player2walk4.png");
}
        bagTexture = bagOptions[selectedIndex];
        bagSprite.setTexture(bagTexture);
bagSprite.setScale(2, 2);

        window.clear();
       
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
            window.draw(descSprites[i]);  
        }
        window.display();
    }
  speed = get_character_speed(selectedIndex, speed);  
bool isJumping = false;  
   

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

    
    bool vacuumActive = false;
    int vacuumDirection = 0; 


    Texture vacuumLeftTex, vacuumUpTex, vacuumRightTex, vacuumDownTex;
    Sprite vacuumLeftSprite, vacuumUpSprite, vacuumRightSprite, vacuumDownSprite;

    if(selectedIndex==0)
    {
    
    vacuumLeftTex.loadFromFile("Assets/players/left.png");  
    vacuumRightTex.loadFromFile("Assets/players/right.png");
    vacuumDownTex.loadFromFile("Assets/players/down.png");
    }
    else
    {
    vacuumLeftTex.loadFromFile("Assets/players/left1.png");  
    vacuumUpTex.loadFromFile("Assets/players/up1.png");
    vacuumRightTex.loadFromFile("Assets/players/right1.png");
    vacuumDownTex.loadFromFile("Assets/players/down1.png");
    }

    vacuumLeftSprite.setTexture(vacuumLeftTex);
    vacuumLeftSprite.setScale(3, 3);

    vacuumUpSprite.setTexture(vacuumUpTex);
    vacuumUpSprite.setScale(3, 3);

    vacuumRightSprite.setTexture(vacuumRightTex);
    vacuumRightSprite.setScale(3, 3);

    vacuumDownSprite.setTexture(vacuumDownTex);
    vacuumDownSprite.setScale(3, 3);

    Texture laserTex;
    Sprite laserSprite;
    bool laserLoaded = laserTex.loadFromFile("Assets/props/lazer.png"); 
    if(laserLoaded)
    {
        laserSprite.setTexture(laserTex);
    }

    lvl = new char* [height];
    for (int i = 0; i < height; i += 1)
    {
        lvl[i] = new char[width];
    }
    lvlMusic.play();
    lvlMusic.setLoop(true);

    for(int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            lvl[i][j] = ' ';
        }
    }

    for (int j = 0; j < width; ++j) 
    lvl[8][j] = '#'; 
    
    lvl[5][6] = '#'; lvl[5][7] = '#'; lvl[5][8] = '#';
    lvl[5][9] = '#'; lvl[5][10] = '#'; lvl[5][11] = '#';

    for(int i = 0; i <=7 ; ++i)
     lvl[i][0] = '#';

    for(int i = 0; i <=7; ++i)
     lvl[i][17] = '#';

    lvl[3][0] = '#'; lvl[3][1] = '#'; lvl[3][2] = '#'; lvl[3][3] = '#';
  
    lvl[3][14] = '#'; lvl[3][15] = '#'; lvl[3][16] = '#'; lvl[3][17] = '#';
    

    for (int j = 0; j < width; ++j) lvl[0][j] = '#';

    const int ghosts = 8;

    float ghost_x[8];
    float ghost_y[8];
    float ghost_speed[8];
    int ghost_dir[8];
    float ghost_velocityY[8];
    bool ghost_onGround[8];
    Sprite ghostSprite[8];
    bool ghost_active[8];  
     const int ghostAnimationFrames = 4;
     Texture ghostWalkTextures[4];
     int ghostCurrentFrame[8];      
int ghostFrameCounter[8];      
const int ghostFrameDelay = 10; 


ghostWalkTextures[0].loadFromFile("Assets/enemies/ghost1.png");
ghostWalkTextures[1].loadFromFile("Assets/enemies/ghost2.png");
ghostWalkTextures[2].loadFromFile("Assets/enemies/ghost3.png");
ghostWalkTextures[3].loadFromFile("Assets/enemies/ghost4.png");


const int skel =4;
Texture skelWalkTextures[4];
skelWalkTextures[0].loadFromFile("Assets/enemies/skel1.png");
skelWalkTextures[1].loadFromFile("Assets/enemies/skel2.png");
skelWalkTextures[2].loadFromFile("Assets/enemies/skel3.png");
skelWalkTextures[3].loadFromFile("Assets/enemies/skel4.png");


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


  
srand(time(0)); 

for(int i = 0; i < ghosts; i++)
{
   
    while(true)
    {
        int tx = rand() % width;
        int ty = rand() % (height - 1);

       
        bool onPlayerPlatform = (ty == 4 && tx >= 6 && tx <= 11);
       
        if(lvl[ty][tx] != '#' && lvl[ty + 1][tx] == '#' && !onPlayerPlatform)
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
       
        ghostSprite[i].setTexture(ghostWalkTextures[0]);
        ghostSprite[i].setScale(2, 2); 
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
 
    float skel_jump_timer[9];      
    float skel_next_jump_time[9];  
    Texture skelTexture;
    skelTexture.loadFromFile("Assets/enemies/skeleton.png");
   
  
    bool ghost_stunned[8];     
    float ghost_stun_timer[8];
    bool skel_stunned[9];      
    float skel_stun_timer[9];
   
   
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
    
    Texture extraLifeTex, powerTex, rangeTex, speedTex;
    extraLifeTex.loadFromFile("Assets/props/extralife.png");
    powerTex.loadFromFile("Assets/props/power.png");
    rangeTex.loadFromFile("Assets/props/range.png");
    speedTex.loadFromFile("Assets/props/speed.png");

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
    
    while(true)
    {
        int tx = rand() % width;
        int ty = rand() % (height - 1);

        // Exclude spawning on the player starting platform
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
       
        
        skel_jump_timer[i] = 0.0f;                                    
        skel_next_jump_time[i] = 10.0f + (rand() % 2);                
       
        skelSprite[i].setTexture(skelTexture);
        skelSprite[i].setScale(2, 2);

        skelSprite[i].setPosition(skel_x[i], skel_y[i]);
    }


    Event ev;
    bool xKeyPressed=false;
   
   
    float shot_enemy_x[20];
    float shot_enemy_y[20];
    float shot_velocity_x[20];
    float shot_velocity_y[20];
    int shot_enemy_type[20];  
    bool shot_is_active[20];
    float shot_lifetime[20];      
const float max_lifetime = 7.0f;  
   int shot_projectile_count=0;

 
    for (int i = 0; i < 20; i++)
    {
        shot_is_active[i] = false;
         shot_lifetime[i] = 0.0f;  
    }

    
    bool previous_E_pressed = false;
    bool previous_Q_pressed = false;
   
  
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
    invisTexture.loadFromFile("Assets/enemies/invisible.png");  

    for(int i = 0; i < invisible_men; i++)
    {
        invis_active[i] = false;  
        invis_stunned[i] = false;
        invis_stun_timer[i] = 0.0f;
        invis_speed[i] = 1.5f;
        invis_dir[i] = 1;
        invisSprite[i].setTexture(invisTexture);
        invisSprite[i].setScale(2, 2);
    }
    
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


    const int chelnovs = 4;
    float chelnov_x[4];
    float chelnov_y[4];
    float chelnov_speed[4];
    int chelnov_dir[4];
    bool chelnov_active[4];
    bool chelnov_stunned[4];
    float chelnov_stun_timer[4];
    Sprite chelnovSprite[4];

   // Loading Chelnov animation frames
const int chelnovWalkFrames = 4;
const int chelnovShootFrames = 4;
const int chelnovVacuumFrames = 4;

Texture chelnovWalkTextures[4];
Texture chelnovShootTextures[4];
Texture chelnovVacuumTextures[4];

// next Load walking frames 
chelnovWalkTextures[0].loadFromFile("Assets/enemies/chelnov_walk1.png");
chelnovWalkTextures[1].loadFromFile("Assets/enemies/chelnov_walk2.png");
chelnovWalkTextures[2].loadFromFile("Assets/enemies/chelnov_walk3.png");
chelnovWalkTextures[3].loadFromFile("Assets/enemies/chelnov_walk4.png");

// next Load shooting frames 
chelnovShootTextures[0].loadFromFile("Assets/enemies/chelnov_shoot1.png");
chelnovShootTextures[1].loadFromFile("Assets/enemies/chelnov_shoot2.png");
chelnovShootTextures[2].loadFromFile("Assets/enemies/chelnov_shoot3.png");
chelnovShootTextures[3].loadFromFile("Assets/enemies/chelnov_shoot4.png");

// Load vacuum frames 
chelnovVacuumTextures[0].loadFromFile("Assets/enemies/chelnov_vacuum1.png");
chelnovVacuumTextures[1].loadFromFile("Assets/enemies/chelnov_vacuum2.png");
chelnovVacuumTextures[2].loadFromFile("Assets/enemies/chelnov_vacuum3.png");
chelnovVacuumTextures[3].loadFromFile("Assets/enemies/chelnov_vacuum4.png");

int chelnovCurrentFrame[4];
int chelnovFrameCounter[4];
const int chelnovFrameDelay = 8;

enum ChelnovState { WALKING, SHOOTING, VACUUMED };
ChelnovState chelnovState[4];

// Chelnov shooting system
float chelnov_shoot_timer[4];
const float chelnov_shoot_interval = 4.0f;  
const float chelnov_shoot_duration = 1.0f;  
bool chelnov_is_shooting[4];

const int max_chelnov_projectiles = 10;
float chelnov_proj_x[10];
float chelnov_proj_y[10];
float chelnov_proj_vx[10];
float chelnov_proj_vy[10];
bool chelnov_proj_active[10];
float chelnov_proj_lifetime[10];
const float chelnov_proj_speed = 8.0f;
const float chelnov_proj_max_lifetime = 5.0f;


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

   
    Clock enemySpawnClock;
    int enemiesSpawned = 0;
    const float spawnInterval = 2.5f;  
    bool level2EnemiesSpawning = false;
  
   
Clock platformChangeClock;
bool platformNeedsRegen = true;
   
 
     int captured[10];
    int cap_count = 0;          
    int maxCapacity = 3;       
   


    Clock deltaClock;
 
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
                
            }
        }

   
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        window.clear();

      
       display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
       player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
        /// Moving the character left and right using arrow keys'
        if(!playerDead) // Only allow controls if alive
        {
        isWalking = false;
        if(Keyboard::isKeyPressed(Keyboard::Key::Right ))
        {
         
            player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
           
            bagSprite.setScale(-2,2);
            PlayerSprite.setScale(-3,3);
            facingRight = true;
            isWalking=true;
        }
       
        if(Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
          
            player_left_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
           
            bagSprite.setScale(2,2);
            PlayerSprite.setScale(3,3);
            facingRight=false; 
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
       
        if(Keyboard::isKeyPressed(Keyboard::Key::Up))
        {
            if(onGround)
            {
                velocityY=jumpStrength;
            }
        }

      
        if(Keyboard::isKeyPressed(Keyboard::Key::W))
        {
            vacuumActive = true;
            vacuumDirection = 1; 
        }
        else if(Keyboard::isKeyPressed(Keyboard::Key::A))
        {
            vacuumActive = true;
            vacuumDirection = 0; 
        }
        else if(Keyboard::isKeyPressed(Keyboard::Key::S))
        {
            vacuumActive = true;
            vacuumDirection = 3; 
        }
        else if(Keyboard::isKeyPressed(Keyboard::Key::D))
        {
            vacuumActive = true;
            vacuumDirection = 2; 
        }
        else
        {
            vacuumActive = false; 
        }
} 
        xKeyPressed = Keyboard::isKeyPressed(Keyboard::Key::Space);

        if(xKeyPressed && vacuumActive)
        {
           update_vacuum(player_x, player_y, vacuumDirection, currentVacuumRange,currentSuckStrength ,
    ghost_x, ghost_y, ghost_active, ghost_stunned, ghost_stun_timer, ghosts,
    skel_x, skel_y, skel_active, skel_stunned, skel_stun_timer, skel,
    invis_x, invis_y, invis_active, invis_stunned, invis_stun_timer, invisible_men,
    chelnov_x, chelnov_y, chelnov_active, chelnov_stunned, chelnov_stun_timer, chelnovs,
    chelnov_is_shooting,  /
    cell_size, true, captured, cap_count, maxCapacity, score
);
        }



        bool E_key_pressed = Keyboard::isKeyPressed(Keyboard::E);
        bool Q_key_pressed = Keyboard::isKeyPressed(Keyboard::Q);

        if (E_key_pressed && !previous_E_pressed)
        {
            if (cap_count > 0)
            {
                shoot_single_enemy(player_x, player_y, vacuumDirection, captured, cap_count,
                  shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                  shot_enemy_type, shot_is_active, shot_lifetime, shot_projectile_count);  
            }
        }

      
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

        if (hit_enemy_type == 0)  
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


      
        previous_E_pressed = E_key_pressed;
        previous_Q_pressed = Q_key_pressed;

     
       update_projectiles(shot_enemy_x, shot_enemy_y, shot_velocity_x, shot_velocity_y,
                  shot_is_active, shot_lifetime, deltaTime, max_lifetime,  
                  shot_projectile_count, lvl, cell_size, height);
       
    if(!playerDead)
    {
        if(vacuumActive)
        {
            
            switch(vacuumDirection)
            {
                case 0: 
                    vacuumLeftSprite.setPosition(player_x, player_y);
                    window.draw(vacuumLeftSprite);
                    break;
                case 1: 
                    vacuumUpSprite.setPosition(player_x, player_y);
                    window.draw(vacuumUpSprite);
                    break;
                case 2: 
                    vacuumRightSprite.setPosition(player_x, player_y);
                    window.draw(vacuumRightSprite);
                    break;
                case 3: 
                    vacuumDownSprite.setPosition(player_x, player_y);
                    window.draw(vacuumDownSprite);
                    break;
            }
        }
        else
        {
            
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
      
        if(xKeyPressed && vacuumActive && laserLoaded)
        {
            float laser_x = player_x + PlayerWidth/2;
            float laser_y = player_y + PlayerHeight/2;

           
            switch(vacuumDirection)
            {
                case 0: 
                    laserSprite.setScale(3, 2);
                    laserSprite.setPosition(laser_x - 190, laser_y - 20);
                    break;
                case 1: 
                    laserSprite.setScale(2, 3);
                    laserSprite.setPosition(laser_x - 20, laser_y - 150);
                    break;
                case 2: 
                    laserSprite.setScale(-3, 2);
                    laserSprite.setPosition(laser_x + 190, laser_y - 20);
                    break;
                case 3: 
                    laserSprite.setScale(2, 3);
                    laserSprite.setPosition(laser_x - 20, laser_y + 50);
                    break;
            }
            window.draw(laserSprite);
        }


for (int i = 0; i < 20; i++)
{
    if (shot_is_active[i])
    {
        if(shot_enemy_type[i] == 0) 
        {
            ghostSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            ghostSprite[0].setColor(Color(255, 100, 100));
            window.draw(ghostSprite[0]);
            ghostSprite[0].setColor(Color::White);
        }
        else if(shot_enemy_type[i] == 1) 
        {
            skelSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            skelSprite[0].setColor(Color(255, 100, 100));
            window.draw(skelSprite[0]);
            skelSprite[0].setColor(Color::White);
        }
        else if(shot_enemy_type[i] == 2) 
        {
            invisSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            invisSprite[0].setColor(Color(255, 100, 100));
            window.draw(invisSprite[0]);
            invisSprite[0].setColor(Color::White);
        }
        else if(shot_enemy_type[i] == 3) 
        {
            chelnovSprite[0].setPosition(shot_enemy_x[i], shot_enemy_y[i]);
            chelnovSprite[0].setColor(Color(255, 100, 100));
            window.draw(chelnovSprite[0]);
            chelnovSprite[0].setColor(Color::White);
        }
    }
}


for(int i = 0; i < invisible_men; i++)
{
    if(!invis_active[i]) continue;
   
    
    if(invis_is_invisible[i])
    {
        invis_invisible_timer[i] -= deltaTime;
        if(invis_invisible_timer[i] <= 0)
        {
            invis_is_invisible[i] = false;
        }
       
      
        continue;
    }
   
    
    if(invis_stunned[i])
    {
        invisSprite[i].setPosition(invis_x[i], invis_y[i]);
        window.draw(invisSprite[i]);
        continue;
    }
   
    invis_teleport_timer[i] += deltaTime;
   
    
    if(invis_teleport_timer[i] >= invis_teleport_interval)
    {
        invis_teleport_timer[i] = 0.0f;
       
        
        if(rand() % 100 < 70)
        {
           
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
            
            invis_is_invisible[i] = true;
            invis_invisible_timer[i] = 0.8f;
           
        }
    }
   
    
    invis_x[i] += invis_speed[i] * invis_dir[i];
   
    
    int tile_x = invis_x[i] / cell_size;
   
    if(tile_x <= 1 || tile_x >= width-2)
    {
        invis_dir[i] *= -1;
    }
   
    invisSprite[i].setPosition(invis_x[i], invis_y[i]);
    window.draw(invisSprite[i]);
}


for(int i = 0; i < chelnovs; i++)
{
    if(!chelnov_active[i]) continue;
   
    
    chelnov_shoot_timer[i] += deltaTime;
   

    if(!chelnov_is_shooting[i] && chelnov_shoot_timer[i] >= chelnov_shoot_interval && !chelnov_stunned[i])
    {
      
        chelnov_is_shooting[i] = true;
        chelnov_shoot_timer[i] = 0.0f;
        chelnovState[i] = SHOOTING;
        chelnovCurrentFrame[i] = 0;
       
       
        float player_dx = player_x - chelnov_x[i];
        int fire_direction = (player_dx > 0) ? 1 : -1;  
       
        for(int p = 0; p < max_chelnov_projectiles; p++)
        {
            if(!chelnov_proj_active[p])
            {
                chelnov_proj_x[p] = chelnov_x[i] + 32;  
                chelnov_proj_y[p] = chelnov_y[i] + 32;
                chelnov_proj_vx[p] = chelnov_proj_speed * fire_direction; 
                chelnov_proj_vy[p] = 0.0f; 
                chelnov_proj_active[p] = true;
                chelnov_proj_lifetime[p] = 0.0f;
                 break;
            }
        }
    }
   
 
    if(chelnov_is_shooting[i])
    {
        if(chelnov_shoot_timer[i] >= chelnov_shoot_duration)
        {
            chelnov_is_shooting[i] = false;
            chelnovState[i] = WALKING;
            chelnov_shoot_timer[i] = 0.0f;
        }
    }
   
    
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
   
    
    if(!chelnov_is_shooting[i])
    {
        chelnov_x[i] += chelnov_speed[i] * chelnov_dir[i];
       
        int tile_x = chelnov_x[i] / cell_size;
        int tile_y = chelnov_y[i] / cell_size;
       
      
        if(tile_x <= 1 || tile_x >= width-2)
        {
            chelnov_dir[i] *= -1;
        }
       
        int next_tile_x = (chelnov_x[i] + chelnov_dir[i] * chelnov_speed[i] * 10) / cell_size;
        int check_ground_y = (chelnov_y[i] + 64 + 5) / cell_size;
       
        if(check_ground_y < height && next_tile_x >= 0 && next_tile_x < width)
        {
            if(lvl[check_ground_y][next_tile_x] != '#')
            {
                chelnov_dir[i] *= -1;
            }
        }
       
      
        chelnovFrameCounter[i]++;
        if(chelnovFrameCounter[i] >= chelnovFrameDelay)
        {
            chelnovFrameCounter[i] = 0;
            chelnovCurrentFrame[i] = (chelnovCurrentFrame[i] + 1) % chelnovWalkFrames;
            chelnovSprite[i].setTexture(chelnovWalkTextures[chelnovCurrentFrame[i]]);
        }
    }
    else  
    {
        chelnovFrameCounter[i]++;
        if(chelnovFrameCounter[i] >= chelnovFrameDelay)
        {
            chelnovFrameCounter[i] = 0;
            chelnovCurrentFrame[i] = (chelnovCurrentFrame[i] + 1) % chelnovShootFrames;
            chelnovSprite[i].setTexture(chelnovShootTextures[chelnovCurrentFrame[i]]);
        }
    }
   
  
    if(chelnov_dir[i] == 1)
        chelnovSprite[i].setScale(-2, 2);
    else
        chelnovSprite[i].setScale(2, 2);
   
    chelnovSprite[i].setPosition(chelnov_x[i], chelnov_y[i]);
    window.draw(chelnovSprite[i]);
}

for(int p = 0; p < max_chelnov_projectiles; p++)
{
    if(!chelnov_proj_active[p]) continue;
   
   
    chelnov_proj_lifetime[p] += deltaTime;
    if(chelnov_proj_lifetime[p] >= chelnov_proj_max_lifetime)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
   

    chelnov_proj_x[p] += chelnov_proj_vx[p];
  
    if(chelnov_proj_x[p] < 0 || chelnov_proj_x[p] > screen_x ||
       chelnov_proj_y[p] < 0 || chelnov_proj_y[p] > screen_y)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
   
    
    if(!playerDead)
    {
        float dx = chelnov_proj_x[p] - (player_x + PlayerWidth/2);
        float dy = chelnov_proj_y[p] - (player_y + PlayerHeight/2);
        float dist = sqrt(dx*dx + dy*dy);
       
        if(dist < 40)
        {
            
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
   
    
    CircleShape projShape(8);
    projShape.setFillColor(Color::Red);
    projShape.setPosition(chelnov_proj_x[p] - 8, chelnov_proj_y[p] - 8);
    window.draw(projShape);
}

for(int p = 0; p < max_chelnov_projectiles; p++)
{
    if(!chelnov_proj_active[p]) continue;
   
   
    chelnov_proj_lifetime[p] += deltaTime;
    if(chelnov_proj_lifetime[p] >= chelnov_proj_max_lifetime)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
 
    chelnov_proj_x[p] += chelnov_proj_vx[p];
    chelnov_proj_y[p] += chelnov_proj_vy[p];
   
  
    if(chelnov_proj_x[p] < 0 || chelnov_proj_x[p] > screen_x ||
       chelnov_proj_y[p] < 0 || chelnov_proj_y[p] > screen_y)
    {
        chelnov_proj_active[p] = false;
        continue;
    }
   
   
    if(!playerDead)
    {
        float dx = chelnov_proj_x[p] - (player_x + PlayerWidth/2);
        float dy = chelnov_proj_y[p] - (player_y + PlayerHeight/2);
        float dist = sqrt(dx*dx + dy*dy);
       
        if(dist < 40)
        {
           
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
   
   
    CircleShape projShape(8);
    projShape.setFillColor(Color::Red);
    projShape.setPosition(chelnov_proj_x[p] - 8, chelnov_proj_y[p] - 8);
    window.draw(projShape);
}
        
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
       
        /
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
  
if(!playerDead)
{
    
    for(int i = 0; i < ghosts; i++)
    {
        if(ghost_active[i] && !ghost_stunned[i])  
        {
            if(check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                               ghost_x[i], ghost_y[i], 64))
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
   
    
    if(!playerDead)
    {
        for(int i = 0; i < skel; i++)
        {
            if(skel_active[i] && !skel_stunned[i])  
            {
               if(check_player_enemy_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                               skel_x[i], skel_y[i], 64))
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


if(!playerDead)
{
    for(int i = 0; i < 4; i++)
    {
        if(powerup_active[i])
        {
            if(check_powerup_collision(player_x, player_y, PlayerWidth, PlayerHeight,
                                      powerup_x[i], powerup_y[i], 64))
            {
                if(powerup_type[i] == 0)  
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

// Wait 3 seconds after level 1 completesthen transition
if(levelComplete && levelCompleteClock.getElapsedTime().asSeconds() >= levelCompleteDelay)
{
    currentLevel = 2;
      playerLives=3;
      maxCapacity=5;
    levelComplete = false;
   
   
    change_to_level2(lvl, height, width);
 
   
    
    level2EnemiesSpawning = true;
    enemySpawnClock.restart();
    enemiesSpawned = 0;
   
  
    platformChangeClock.restart();
    platformNeedsRegen = false;
   
   
    player_x = 500;
    player_y = 150;
    velocityY = 0;
}


       
       
        if(playerDead)
        {
            float elapsedTime = deathClock.getElapsedTime().asSeconds();
          
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
                
    score -= 200;
    if(score < 0) score = 0;
                  
                    window.close();
                }
            }
        }


scoreText.setString("Score: " + to_string(score) + "  Lives: " + to_string(playerLives));
window.draw(scoreText);


if(combo > 0)
{
    comboText.setString("Combo: " + to_string(combo) + "x  Multiplier: " + to_string(comboMultiplier) + "x");
    window.draw(comboText);
}
 
 
 if(currentLevel == 2)
{
    float platformTime = platformChangeClock.getElapsedTime().asSeconds();
   
    if(platformTime >= 20.0f)
    {
        generate_random_slanted_platform(lvl, height, width, true);  
        platformChangeClock.restart();
    }
}
 
if(currentLevel == 2 && level2EnemiesSpawning)
{
    float spawnElapsed = enemySpawnClock.getElapsedTime().asSeconds();
   
    if(spawnElapsed >= 20.0f)
    {
        enemySpawnClock.restart();
       
        
        if(enemiesSpawned < 3)
        {
            int idx = enemiesSpawned;
          
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

if(!levelComplete && !playerDead)
{
   if(currentLevel == 1)
{
    
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
   

if(levelComplete && currentLevel == 1)
{
    float elapsed = levelCompleteClock.getElapsedTime().asSeconds();
   
    
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
 
 
if(elapsed >= levelCompleteDelay)
{
 
    currentLevel = 2;
   
  
    playerLives = 3;
   
    levelComplete = false;
   
   
    change_to_level2(lvl, height, width);
   
    
    player_x = 500;
    player_y = 4 * 64 - 102; 
   
    velocityY = 0;
    cap_count = 0;
    vacuumActive = false;

   

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
   
    if(platformElapsed >= 20.0f)  
    {
        platformChangeClock.restart();
       
       
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

    lvlMusic.stop();
    for (int i = 0; i < height; i++)
        delete[] lvl[i];
    delete[] lvl;
   
    return 0;
}

       
   



