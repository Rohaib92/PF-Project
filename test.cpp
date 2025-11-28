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

// ================= FUNCTION DEFINITIONS =================
void display_level(RenderWindow& window, char** lvl, Texture& bgTex, Sprite& bgSprite, Texture& blockTexture, Sprite& blockSprite, const int height, const int width, const int cell_size)
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
        }
    }
}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
    offset_y = player_y;
    offset_y += velocityY;

    char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x) / cell_size];
    char bottom_right_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
    char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

    if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
        onGround = true;
    else
    {
        player_y = offset_y;
        onGround = false;
    }

    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
    }
    else
    {
        velocityY = 0;
    }
}

void player_left_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x - speed;

    char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
    char left_mid = lvl[(int)(player_y + Pheight / 2) / cell_size][(int)(offset_x) / cell_size];
    char left_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];

    if (left_top == '#' || left_mid == '#' || left_bottom == '#')
        offset_x = player_x;
    else
        player_x = offset_x;
}

void player_right_collision(char** lvl, float& offset_x, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, float speed)
{
    offset_x = player_x + speed;

    char right_top = lvl[(int)(player_y) / cell_size][(int)(offset_x + Pwidth) / cell_size];
    char right_mid = lvl[(int)(player_y + Pheight / 2) / cell_size][(int)(offset_x + Pwidth) / cell_size];
    char right_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x + Pwidth) / cell_size];

    if (right_top == '#' || right_mid == '#' || right_bottom == '#')
        offset_x = player_x;
    else
        player_x = offset_x;
}

void player_ceiling_collision(char** lvl, float& offset_y, float& velocityY, float& player_x, float& player_y, const int cell_size, int& Pwidth)
{
    offset_y = player_y + velocityY;

    char top_left = lvl[(int)(offset_y) / cell_size][(int)(player_x) / cell_size];
    char top_mid = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];
    char top_right = lvl[(int)(offset_y) / cell_size][(int)(player_x + Pwidth) / cell_size];

    if (top_left == '#' || top_mid == '#' || top_right == '#')
        velocityY = 0;
    else
        player_y = offset_y;
}

// ================= MAIN FUNCTION =================
int main()
{
    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // ===== LEVEL SETUP =====
    const int cell_size = 64;
    const int height = 14;
    const int width = 18;
    char** lvl = new char* [height];
    for (int i = 0; i < height; i++)
    {
        lvl[i] = new char[width];
        for (int j = 0; j < width; j++)
            lvl[i][j] = ' ';
    }

    // Bottom floor
    for (int j = 0; j < width; j++)
        lvl[8][j] = '#';

    // Some platforms
    for (int j = 6; j <= 11; j++)
        lvl[5][j] = '#';

    // Left and right walls
    for (int i = 0; i < 8; i++)
    {
        lvl[i][0] = '#';
        lvl[i][17] = '#';
    }

    // Top wall
    for (int j = 0; j < width; j++)
        lvl[0][j] = '#';

    // ===== TEXTURES & SPRITES =====
    Texture bgTex, blockTexture, PlayerTexture;
    Sprite bgSprite, blockSprite, PlayerSprite;

    bgTex.loadFromFile("bg.png");
    bgSprite.setTexture(bgTex);

    blockTexture.loadFromFile("block1.png");
    blockSprite.setTexture(blockTexture);

    // ===== MUSIC =====
    Music lvlMusic;
    lvlMusic.openFromFile("mus.ogg");
    lvlMusic.setVolume(20);
    lvlMusic.play();
    lvlMusic.setLoop(true);

    // ===== PLAYER DATA =====
    float player_x = 500, player_y = 150;
    float speed = 5;
    const float jumpStrength = -20;
    const float gravity = 1;
    bool onGround = false;
    float velocityY = 0;
    float offset_x = 0, offset_y = 0;
    float terminal_Velocity = 20;
    int PlayerHeight = 102, PlayerWidth = 96;

    // ===== VACUUM SYSTEM =====
    bool vacuumActive = false;
    int vacuumDirection = 0; // 0=left,1=up,2=right,3=down
    Texture vacuumLeftTex, vacuumUpTex, vacuumRightTex, vacuumDownTex;
    Sprite vacuumLeftSprite, vacuumUpSprite, vacuumRightSprite, vacuumDownSprite;
    vacuumLeftTex.loadFromFile("player.png");
    vacuumUpTex.loadFromFile("vacuum top.png");
    vacuumRightTex.loadFromFile("right vacuum.png");
    vacuumDownTex.loadFromFile("vacuum bottom.png");
    vacuumLeftSprite.setTexture(vacuumLeftTex); vacuumLeftSprite.setScale(3,3);
    vacuumUpSprite.setTexture(vacuumUpTex); vacuumUpSprite.setScale(3,3);
    vacuumRightSprite.setTexture(vacuumRightTex); vacuumRightSprite.setScale(3,3);
    vacuumDownSprite.setTexture(vacuumDownTex); vacuumDownSprite.setScale(3,3);

    // ===== PLAYER SELECTION MENU =====
    const int playerOptionsCount = 2;
    Texture playerOptions[playerOptionsCount];
    Sprite playerOptionSprite[playerOptionsCount];
    playerOptions[0].loadFromFile("player1.png");
    playerOptions[1].loadFromFile("player2.png");
    for(int i = 0; i < playerOptionsCount; i++)
    {
        playerOptionSprite[i].setTexture(playerOptions[i]);
        playerOptionSprite[i].setScale(3,3);
        playerOptionSprite[i].setPosition(300 + i*300, 300);
    }
    int selectedIndex = 0;
    bool playerChosen = false;

    // FONT FOR MENU INSTRUCTION (OPTIONAL)
    Font font;
    font.loadFromFile("arial.ttf");
    Text instruction("Use ← → to select, Enter to confirm", font, 30);
    instruction.setFillColor(Color::White);
    instruction.setPosition(200, 150);

    // ===== PLAYER SELECTION LOOP =====
    while(!playerChosen && window.isOpen())
    {
        Event ev;
        while(window.pollEvent(ev))
        {
            if(ev.type == Event::Closed)
                window.close();
            if(ev.type == Event::KeyPressed)
            {
                if(ev.key.code == Keyboard::Right)
                    selectedIndex = (selectedIndex + 1) % playerOptionsCount;
                if(ev.key.code == Keyboard::Left)
                    selectedIndex = (selectedIndex - 1 + playerOptionsCount) % playerOptionsCount;
                if(ev.key.code == Keyboard::Enter)
                    playerChosen = true;
            }
        }

        window.clear();
        for(int i = 0; i < playerOptionsCount; i++)
        {
            if(i == selectedIndex)
                playerOptionSprite[i].setColor(Color::Yellow);
            else
                playerOptionSprite[i].setColor(Color::White);
            window.draw(playerOptionSprite[i]);
        }
        window.draw(instruction);
        window.display();
    }

    // ===== SET PLAYER TEXTURE AFTER SELECTION =====
    PlayerTexture = playerOptions[selectedIndex];
    PlayerSprite.setTexture(PlayerTexture);
    PlayerSprite.setScale(3,3);
    PlayerSprite.setPosition(player_x, player_y);

    // ================== MAIN GAME LOOP ==================
    Event ev;
    while(window.isOpen())
    {
        while(window.pollEvent(ev))
        {
            if(ev.type == Event::Closed)
                window.close();
        }

        if(Keyboard::isKeyPressed(Keyboard::Escape))
            window.close();

        window.clear();
        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
        player_gravity(lvl, offset_y, velocityY, onGround, gravity, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);

        // PLAYER MOVEMENT
        if(Keyboard::isKeyPressed(Keyboard::Key::Right))
        {
            player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            PlayerSprite.setScale(-3,3);
        }
        if(Keyboard::isKeyPressed(Keyboard::Key::Left))
        {
            player_left_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
            PlayerSprite.setScale(3,3);
        }
        if(velocityY < 0)
            player_ceiling_collision(lvl, offset_y, velocityY, player_x, player_y, cell_size, PlayerWidth);
        if(Keyboard::isKeyPressed(Keyboard::Key::Space) && onGround)
            velocityY = jumpStrength;

        // VACUUM CONTROL
        if(Keyboard::isKeyPressed(Keyboard::Key::W)) { vacuumActive = true; vacuumDirection = 1; }
        else if(Keyboard::isKeyPressed(Keyboard::Key::A)) { vacuumActive = true; vacuumDirection = 0; }
        else if(Keyboard::isKeyPressed(Keyboard::Key::S)) { vacuumActive = true; vacuumDirection = 3; }
        else if(Keyboard::isKeyPressed(Keyboard::Key::D)) { vacuumActive = true; vacuumDirection = 2; }
        else vacuumActive = false;

        // DRAW PLAYER OR VACUUM
        if(vacuumActive)
        {
            switch(vacuumDirection)
            {
                case 0: vacuumLeftSprite.setPosition(player_x, player_y); window.draw(vacuumLeftSprite); break;
                case 1: vacuumUpSprite.setPosition(player_x, player_y); window.draw(vacuumUpSprite); break;
                case 2: vacuumRightSprite.setPosition(player_x, player_y); window.draw(vacuumRightSprite); break;
                case 3: vacuumDownSprite.setPosition(player_x, player_y); window.draw(vacuumDownSprite); break;
            }
        }
        else
        {
            PlayerSprite.setPosition(player_x, player_y);
            window.draw(PlayerSprite);
        }

        // ================= GHOSTS / SKELETONS =================
        // You can add your ghost and skeleton movement here (as in your original code)

        window.display();
    }

    // CLEANUP
    lvlMusic.stop();
    for(int i = 0; i < height; i++)
        delete[] lvl[i];
    delete[] lvl;

    return 0;
}

