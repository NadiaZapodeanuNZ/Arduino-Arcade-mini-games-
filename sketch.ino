#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUTTON_UP 2
#define BUTTON_DOWN 3
#define BUTTON_LEFT 8
#define BUTTON_RIGHT 5
#define BUTTON_BACK_SELECT 7
#define SNAKE_PIECE_SIZE     3
#define MAX_SNAKE_LENGTH   165
#define MAP_SIZE_X          24
#define MAP_SIZE_Y          20
#define STARTING_SNAKE_SIZE  5
#define SNAKE_MOVE_DELAY    200
#define MAX_SNAKE_LENGTH 64
#define OLED_RESET 4



int16_t dinoY = 29; 
int16_t dinoX = 2;          
int16_t dinoHeight = 25;      
int16_t dinoWidth = 26;
        
int verticalSpeed = 0;      
int gravity = 1;            
int jumpStrength = -10;     
bool isJumping = false;   
bool onGround = true;     


int obstacle1_x = 112;
int obstacle1_y = 32; 
int obstacle1Width = 11;
int obstacle1Height = 23;
int obstacleSpeed = 4;
int score = 0;

static const unsigned char PROGMEM dino1[]={
  // 'dino', 25x26px
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x06, 0xff, 0x00, 0x00, 0x0e, 0xff, 0x00,
0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x0f, 0xc0, 0x00,
0x00, 0x0f, 0xfc, 0x00, 0x40, 0x0f, 0xc0, 0x00, 0x40, 0x1f, 0x80, 0x00, 0x40, 0x7f, 0x80, 0x00,
0x60, 0xff, 0xe0, 0x00, 0x71, 0xff, 0xa0, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x7f, 0xff, 0x80, 0x00,
0x7f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00,
0x03, 0xfc, 0x00, 0x00, 0x01, 0xdc, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00, 0x01, 0x8c, 0x00, 0x00,
0x01, 0x0c, 0x00, 0x00, 0x01, 0x8e, 0x00, 0x00
};

static const unsigned char PROGMEM tree1[]={
  // 'tree1', 11x23px
0x1e, 0x00, 0x1f, 0x00, 0x1f, 0x40, 0x1f, 0xe0, 0x1f, 0xe0, 0xdf, 0xe0, 0xff, 0xe0, 0xff, 0xe0,
0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xe0, 0xff, 0xc0, 0xff, 0x00, 0xff, 0x00, 0x7f, 0x00,
0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00
};


const byte buttonPins[] = {2, 3, 8 , 5, 7};
int offsetMapX;
int offsetMapY;
int currentTime = 0;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int generateFruitStart = 0;

const char* menuItems[] = {"SELECT A GAME", "SNAKE", "TREX RUNNER"};
int currentIndex = 1;
const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

int inMenu = 1;
int inSnake = 0;
int inTREX = 0;



int8_t snake[MAX_SNAKE_LENGTH][2];
uint8_t snake_length;
int8_t fruit[2];

typedef enum {
  MENU,
  START,
  RUNNING,
  GAMEOVER
} State;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    BACK_SELECT,
    NONE
} Direction;

State gameState = MENU;
Direction currentDirection;

int moveTimeSnake = 0;

void setup() 
{
  Serial.begin(9600);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_BACK_SELECT, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) 
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  offsetMapX = SCREEN_WIDTH - SNAKE_PIECE_SIZE * MAP_SIZE_X - 2;
  offsetMapY = 2;

  randomSeed(analogRead(A0));
}

void loop() 
{
  switch (gameState) 
  {
    case MENU:
      if (inMenu) 
      {
        displayMenu();
      }
      break;

    case START:
      if (inSnake) 
      {
         runSnakeGame();
      } 
      if (inTREX)
      {
        runDinoGame();
      }
      break;

    case RUNNING:
      if (inSnake) 
      { 
        runSnakeGame();
      }
      if (inTREX)
      {
        runDinoGame();
      }
      break;

    case GAMEOVER:
      {displayGameOver();}
      break;
  }
}

//---------------------------------FUNCTII PT MENIU -------------------------------------------------------------
void displayMenu() 
{
  display.clearDisplay();

  for (int i = 0; i < menuSize; i++) 
  {
    display.setCursor(0, i * 20);
    if (i == currentIndex) {
      display.fillRect(0, i * 20, 128, 10, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    } else {
      display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    }
    display.print(menuItems[i]);
  }

  if (digitalRead(BUTTON_UP) == LOW) 
  {
    currentIndex--;
    if (currentIndex < 1) currentIndex = menuSize - 1; 
    delay(200);
  }

  if (digitalRead(BUTTON_DOWN) == LOW) 
  {
    currentIndex++;
    if (currentIndex >= menuSize) currentIndex = 1; 
    delay(200); 
  }

  if (digitalRead(BUTTON_BACK_SELECT) == LOW) 
  {
    selectGame(currentIndex);
    delay(200);
  }

  display.display();
}

void selectGame(int index) 
{
  display.clearDisplay();
  display.setCursor(0, 0);

  switch (index) 
  {
    case 1:
      display.print("You selected SNAKE!");
      inMenu = 0;
      inSnake = 1;
      inTREX = 0;
      gameState = START;
      break;
    case 2:
      display.print("You selected T-REX RUNNER!");
      inMenu = 0;
      inSnake = 0;
      inTREX = 1;
      gameState = START;
      break;
    default:
      display.print("Invalid selection!");
  }

  display.display();
  delay(2000);
  display.clearDisplay();
}

void runSnakeGame() 
{
  if (gameState == START) 
  {
    currentDirection = NONE;
    resetSnake();
    generateFruitSnakeStart();
    drawScoreSnake();
    drawMapSnake();
    displayGame();

    if (buttonPress()) 
     {gameState = RUNNING;
     display.clearDisplay();
     } 
  }
  if (gameState == RUNNING) 
{ 
  readDirection();
  currentTime = millis();
  if (currentTime % SNAKE_MOVE_DELAY == 0) 
   {
    if (moveSnake()) 
    {

      gameState = GAMEOVER;
      displayGameOver();
      return;
    }


    checkFruitSnake();
    displayGame();
    drawMapSnake();
    drawScoreSnake();
  }
}

}
void resetSnake() 
{
  snake_length = STARTING_SNAKE_SIZE;
  for (int i = 0; i < snake_length; i++) 
  {
    snake[i][0] = MAP_SIZE_X / 2 - i;
    snake[i][1] = MAP_SIZE_Y / 2;
  }
}

void displayGame() 
{
  for (int i = 0; i < snake_length; i++) 
  {
    int pixelX = offsetMapX + snake[i][0] * SNAKE_PIECE_SIZE;
    int pixelY = offsetMapY + snake[i][1] * SNAKE_PIECE_SIZE;
    display.fillRect(pixelX, pixelY, SNAKE_PIECE_SIZE, SNAKE_PIECE_SIZE, SSD1306_WHITE);
  }
  int fruitPixelX = offsetMapX + fruit[0] * SNAKE_PIECE_SIZE;
  int fruitPixelY = offsetMapY + fruit[1] * SNAKE_PIECE_SIZE;
  display.fillRect(fruitPixelX, fruitPixelY, SNAKE_PIECE_SIZE, SNAKE_PIECE_SIZE, SSD1306_WHITE);

  display.display();
}


void displayGameOver() 
{
  display.clearDisplay();
  display.setCursor(30, 30);
  display.setTextSize(1);
  display.print("GAME OVER!");
  display.display();
  delay(2000);
  gameState = MENU;
  inMenu = 1;
  inSnake = 0;
  inTREX = 0;
}

bool collisionCheckSnake(int8_t x, int8_t y) 
{
  for (int i = 1; i < snake_length; i++) {
    if (x == snake[i][0] && y == snake[i][1]) return true;
  }
  if (x < 0 || y < 0 || x >= MAP_SIZE_X || y >= MAP_SIZE_Y) return true;
  return false;
}

void checkCollisionsSnake() 
{
  if (collisionCheckSnake(snake[0][0], snake[0][1])) {
    gameState = GAMEOVER;
  }
}

void checkFruitSnake() 
{
  if (fruit[0] == snake[0][0] && fruit[1] == snake[0][1]) 
  {
    if (snake_length < MAX_SNAKE_LENGTH) 
    {
      snake[snake_length][0] = snake[snake_length - 1][0];
      snake[snake_length][1] = snake[snake_length - 1][1];
      snake_length++;
    }
    generateFruitSnake();
  }
}


void generateFruitSnakeStart() 
{
  
    if (generateFruitStart == 0)
    {
      bool b = false;
      do 
      {
        b = false;
        fruit[0] = random(0, MAP_SIZE_X);
        fruit[1] = random(0, MAP_SIZE_Y);
        for (int i = 0; i < snake_length; i++) 
        {
          if (fruit[0] == snake[i][0] && fruit[1] == snake[i][1]) 
          {
            b = true;
            continue;
          }
        }
      } while (b);
    }
    generateFruitStart = 1;
  
}

void generateFruitSnake() 
{
  bool b = false;
  do 
  {
    b = false;
    fruit[0] = random(0, MAP_SIZE_X);
    fruit[1] = random(0, MAP_SIZE_Y);
    for (int i = 0; i < snake_length; i++) 
    {
      if (fruit[0] == snake[i][0] && fruit[1] == snake[i][1]) 
      {
        b = true;
        continue;
      }
    }
  } while (b);
}

void drawMapSnake() 
{
  display.drawRect(offsetMapX, offsetMapY, MAP_SIZE_X * SNAKE_PIECE_SIZE + 2, MAP_SIZE_Y * SNAKE_PIECE_SIZE + 2, SSD1306_WHITE);
  display.display();
}

void drawScoreSnake() 
{
  
  display.fillRect(0, 0, 54, 10, SSD1306_BLACK);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(2, 2);
  display.print(F("Score:"));
  display.print(snake_length - STARTING_SNAKE_SIZE);

 
  display.display();
}

bool moveSnake() {
  int8_t x = snake[0][0];
  int8_t y = snake[0][1];
  switch (currentDirection) {
    case LEFT:
      x -= 1;
      break;
    case UP:
      y -= 1;
      break;
    case RIGHT:
      x += 1;
      break;
    case DOWN:
      y += 1;
      break;
  }

 
  if (collisionCheckSnake(x, y)) {
    return true;
  }

  
  if (fruit[0] != snake[0][0] || fruit[1] != snake[0][1]) {
    int tailX = offsetMapX + snake[snake_length - 1][0] * SNAKE_PIECE_SIZE;
    int tailY = offsetMapY + snake[snake_length - 1][1] * SNAKE_PIECE_SIZE;
    display.fillRect(tailX, tailY, SNAKE_PIECE_SIZE, SNAKE_PIECE_SIZE, SSD1306_BLACK);
  }

  
  for (int i = snake_length - 1; i > 0; i--) 
  {
    snake[i][0] = snake[i - 1][0];
    snake[i][1] = snake[i - 1][1];
  }
  snake[0][0] = x;
  snake[0][1] = y;

  return false;
}




bool buttonPress() {
  for (byte i = 0; i < 4; i++) 
  {
    byte buttonPin = buttonPins[i];
    if (digitalRead(buttonPin) == LOW) {
      return true;
    }
  }
  return false;
}

void readDirection() 
{
  Direction pressed = NONE;
  if (digitalRead(BUTTON_UP) == LOW) {
    pressed = UP;
    Serial.println(F("UP button pressed"));
  } else if (digitalRead(BUTTON_DOWN) == LOW) {
    pressed = DOWN;
    Serial.println(F("DOWN button pressed"));
  } else if (digitalRead(BUTTON_LEFT) == LOW) {
    pressed = LEFT;
    Serial.println(F("LEFT button pressed"));
  } else if (digitalRead(BUTTON_RIGHT) == LOW) {
    pressed = RIGHT;
    Serial.println(F("RIGHT button pressed"));
  } else if (digitalRead(BUTTON_BACK_SELECT) == LOW) {
    pressed = BACK_SELECT;
    Serial.println(F("BACK_SELECT button pressed"));
  } else {
    pressed = NONE;
  }

  if (pressed == BACK_SELECT) {
    currentDirection = NONE;
    inSnake = 0; 
    inMenu = 1;
    gameState = MENU;
    inTREX = 0;
    Serial.println(F("Back to Menu"));
  }
  if (pressed != NONE) 
  {
    if ((pressed == UP && currentDirection != DOWN) ||
        (pressed == DOWN && currentDirection != UP) ||
        (pressed == LEFT && currentDirection != RIGHT) ||
        (pressed == RIGHT && currentDirection != LEFT)) 
    {
      currentDirection = pressed; 
      Serial.print(F("currentDirection = "));
      switch (currentDirection) {
        case UP:
          Serial.println(F("UP"));
          break;
        case DOWN:
          Serial.println(F("DOWN"));
          break;
        case LEFT:
          Serial.println(F("LEFT"));
          break;
        case RIGHT:
          Serial.println(F("RIGHT"));
          break;
        default:
          Serial.println(F("NONE"));
          break;
      }
    }
  }
}

void drawScoreDino() 
{
  
  display.fillRect(80, 0, 120, 10, SSD1306_BLACK);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(80, 2);
  display.print(F("Score:"));
  display.print(score);

 
  display.display();
}
void drawMapDinoStart() 
{
display.drawBitmap (2,29,dino1,25,26,SSD1306_WHITE);
display.drawBitmap (obstacle1_x, obstacle1_y, tree1, 11, 23, SSD1306_WHITE);
display.drawLine(0,54,127,54,SSD1306_WHITE);
drawScoreDino();
display.display();
}


void drawDino() {
   display.drawBitmap(dinoX, dinoY, dino1, 25, 26, SSD1306_WHITE);
}

void drawObstacle() {
  display.drawBitmap(obstacle1_x, 32, tree1, 11, 23, SSD1306_WHITE);
}


void runDinoGame()
{
  if (gameState == START)
  {
    drawMapDinoStart();
    if (buttonPress())
    {
      display.clearDisplay();
      gameState = RUNNING;
    }
  }

  if (gameState == RUNNING)
  {
    display.drawLine(0, 54, 127, 54, SSD1306_WHITE);
    drawScoreDino();
    display.display();
    if (digitalRead(2) == LOW && onGround) {
      verticalSpeed = jumpStrength;
      onGround = false;
      isJumping = true;
    }
    if (!onGround) 
    {
      display.drawBitmap(dinoX, dinoY, dino1, 25, 26, SSD1306_BLACK);
      dinoY += verticalSpeed;
      verticalSpeed += gravity;

      if (dinoY >= 29) 
      {
        dinoY = 29;
        onGround = true;
        verticalSpeed = 0;
      }
      display.drawBitmap(dinoX, dinoY, dino1, 25, 26, SSD1306_WHITE);
    }

    
    display.drawBitmap(obstacle1_x, 32, tree1, 11, 23, SSD1306_BLACK);

    obstacle1_x -= obstacleSpeed;
    if (obstacle1_x < -11) 
    {
      score++;
      obstacle1_x = SCREEN_WIDTH - 11;
    }
    display.drawBitmap(obstacle1_x, 32, tree1, 11, 23, SSD1306_WHITE);

   
    if (obstacle1_x < dinoWidth + 5 && obstacle1_x + obstacle1Width > 5 && dinoY + dinoHeight > obstacle1_y) 
    {
      gameState = GAMEOVER;
      score = 0;
      obstacle1_x = 112;
    }

    drawDino();
    drawObstacle();

    display.display();
    delay(20);
  }
}
