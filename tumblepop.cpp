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
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
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

char left_top = lvl[(int)(player_y) / cell_size][(int)(offset_x) / cell_size];
char left_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x) / cell_size];
char left_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x) / cell_size];

// check if it hits any of the block if yes then
if (left_top == '#' || left_mid == '#' || left_bottom == '#')
{
offset_x=player_x ; // stays at original position no moving
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

char right_top = lvl[(int)(player_y) / cell_size][(int)(offset_x + Pwidth) / cell_size];
char right_mid = lvl[(int)(player_y + Pheight/2) / cell_size][(int)(offset_x + Pwidth) / cell_size];
char right_bottom = lvl[(int)(player_y + Pheight) / cell_size][(int)(offset_x + Pwidth) / cell_size];

// check if player hits the blocks
if (right_top == '#' || right_mid == '#' || right_bottom == '#')
{
// if yes dont move stay on origional posiiton
offset_x=player_x ;

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
                window.close();
            if(ev.type == Event::KeyPressed)
            {
                if(ev.key.code == Keyboard::Right)
                    selectedIndex = (selectedIndex + 1) % playerOptionsCount;
                if(ev.key.code == Keyboard::Left)
                    selectedIndex = (selectedIndex - 1 + playerOptionsCount) % playerOptionsCount;
                if(ev.key.code == Keyboard::Enter){
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
                playerOptionSprite[i].setColor(Color::Yellow);
            else
                playerOptionSprite[i].setColor(Color::White);
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
vacuumLeftTex.loadFromFile("player1.png");  // default player with vacuum left
vacuumUpTex.loadFromFile("vacuum top.png");
vacuumRightTex.loadFromFile("right vacuum.png");
vacuumDownTex.loadFromFile("vacuum bottom.png");

// Setup vacuum sprites
vacuumLeftSprite.setTexture(vacuumLeftTex);
vacuumLeftSprite.setScale(3, 3);

vacuumUpSprite.setTexture(vacuumUpTex);
vacuumUpSprite.setScale(3, 3);

vacuumRightSprite.setTexture(vacuumRightTex);
vacuumRightSprite.setScale(3, 3);

vacuumDownSprite.setTexture(vacuumDownTex);
vacuumDownSprite.setScale(3, 3);



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
lvl[3][0] = '#';
lvl[3][1] = '#';
lvl[3][2] = '#';
lvl[3][3] = '#';

////  LEFT FORWARD PART
lvl[3][14] = '#';
lvl[3][15] = '#';
lvl[3][16] = '#';
lvl[3][17] = '#';

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
Sprite ghostSprite[8];

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

	ghostSprite[i].setTexture(ghostTexture);
	ghostSprite[i].setScale(2, 2);   // increase ghost size

	ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
}
// skeletons
const int skel = 8;

float skel_x[8];
float skel_y[8];
float skel_speed[8];
int skel_dir[8];
Sprite skelSprite[8];

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

	skelSprite[i].setTexture(skelTexture);
	skelSprite[i].setScale(2, 2);   // increase ghost size

	skelSprite[i].setPosition(skel_x[i], skel_y[i]);
}

	Event ev;
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
		
///***		// Moving the character left and right using arrow keys

		if(Keyboard::isKeyPressed(Keyboard::Key::Right )){
		 player_right_collision(lvl, offset_x, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, speed);
		 PlayerSprite.setScale(-3,3); 
		 // the player png pixels are 32x34 but game is 96x102 so we need to increase it by 3
		 // player face on right as player is already facing left thats why we use neg sign to flip its face
		 }
		  if(Keyboard::isKeyPressed(Keyboard::Key::Left)){
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
		if(onGround){
			velocityY=jumpStrength;
			}
			
			
		}
		
		// ============= VACUUM CONTROL WITH WASD =============
if(Keyboard::isKeyPressed(Keyboard::Key::W)) {
    vacuumActive = true;
    vacuumDirection = 1; // Up
}
else if(Keyboard::isKeyPressed(Keyboard::Key::A)) {
    vacuumActive = true;
    vacuumDirection = 0; // Left
}
else if(Keyboard::isKeyPressed(Keyboard::Key::S)) {
    vacuumActive = true;
    vacuumDirection = 3; // Down
}
else if(Keyboard::isKeyPressed(Keyboard::Key::D)) {
    vacuumActive = true;
    vacuumDirection = 2; // Right
}
else {
    vacuumActive = false; // No vacuum key pressed
}
// ========================================
               
              
		// ============= DRAW PLAYER OR VACUUM =============
if(vacuumActive) {
    // Draw vacuum sprite based on direction
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
PlayerSprite.setPosition(player_x, player_y);
window.draw(PlayerSprite);
}
		//  GHOST MOVEMENT 
for(int i = 0; i < 8; i++)
{
float nextX = ghost_x[i] + ghost_speed[i] * ghost_dir[i];
int frontTileX = (nextX + (ghost_dir[i] == 1 ? 32 : 0)) / cell_size;
int footTileY = (ghost_y[i] + 64) / cell_size;
if(lvl[footTileY][frontTileX] != '#')
{
ghost_dir[i] *= -1;
}
else
{
ghost_x[i] = nextX;
}
ghostSprite[i].setPosition(ghost_x[i], ghost_y[i]);
window.draw(ghostSprite[i]);	
//skeleton movemement
}
for(int j=0; j<4; j++)
{
float nextp = skel_x[j] + skel_speed[j] * skel_dir[j];
int frontskelX = (nextp + skel_dir[j] ==1 ? 32 : 0) /cell_size;
int backskelY = (skel_y[j] + 64) / cell_size;
if(lvl[backskelY][frontskelX] != '#')
{
skel_dir[j] *= -1;
}
else
skel_x[j] = nextp;
skelSprite[j].setPosition(skel_x[j]  ,skel_y[j]);
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

