 class Tile {
  public:
    bool isSolid = false;
};

//https://store.steampowered.com/app/1718240/Beaver_Clicker/
#include <Wire.h>
#include <SPI.h>
//#include <Adafruit_BMP280.h>
//#include <NMEAGPS.h>

using namespace std;

/*Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();*/

static const int RXPin = 4, TXPin = 3;

float temp;
float pressure;

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define gridSizeX 15
#define gridSizeY 15
Tile tiles [gridSizeX][gridSizeY];

int lLineCollum, rLineCollum;
#define lineFromWall 4
#define lineWidth 5
#define lineChonk 2
float ballX = 0, ballY = 0;
int tileBallX, tileBallY;

float angle = 45;
float xVel, yVel;
#define speed 1
#define delayTime 300

int lLineTiles [lineWidth];
int rLineTiles [lineWidth];
int computerLineMove;
int desiredAiPos;
int lLinePos;
int rLinePos;

////////////////////////////////////////////////////////////////////////////

#define comCallsign "TSB, "
#define pongCallsign "TGB, "
#define Callsign "TFB, "

void setup() {
  Serial.begin(9600); // begin transmission
  while ( !Serial )    // wait for native usb
  {
    Serial.println("usb missing");
    delay(100);
  }
  Serial.println("usb connected");

  //InitBMP();

  SetupMap();
  //pinMode(6, OUTPUT);
  Serial.println();
  //Serial.println("Callsign, BMPTemp, LMTemp, NTCTemp, BMPPressure, MPXPressure, BMPCalculatedAltitude, Uptime");
  Serial.println("pong");
  Serial.println("PongCallsign, BallPosX, BallPosY, BallAngle, GroundPaddlePos, CansatPaddlePos");
  PrintMapData();
}
void PrintMapData()
{
  Serial.print("Grid Size X = "); Serial.println(gridSizeX);
  Serial.print("Grid Size Y = "); Serial.println(gridSizeY);
  Serial.print("Line Size = "); Serial.println(lineWidth);
}
void loop() {
  /*ReadBMP();
  float d[] = {
    temp,
    ReadLM(0, 1), //offset, sensitivity
    ReadNTC(0, 1), //off, grad
    pressure,
    ReadMPX(),
    BMPAltutude(991),
    millis()
  };
  PrintData(Callsign, d, 7);*/

  Communicate();
  MoveLines();
  MoveBall();
  delay(delayTime);
  //PrintMap();

  float ballData[] = {
    ballX,
    ballY,
    angle,
    computerLineMove,
    desiredAiPos,
    millis()
  };
  PrintData(pongCallsign, ballData, 6);
}

#pragma region pong
void SetupMap()
{
  for (int x = 0; x < gridSizeX; x++){
    for (int y = 0; y < gridSizeY; y++){
      if((x == 0 || x == gridSizeX-1) || (y == 0 || y == gridSizeY-1)) tiles[x][y].isSolid = true; // sets the tiles on the edges to solid blocks
    }
  }
  InstertLines();
  SetupBall();
  //PrintMap();
}
void InstertLines()
{
  // set the collums where the lines will be placed
  lLineCollum = lineFromWall-1;
  rLineCollum = gridSizeX - lineFromWall;

  // set position of the lines to the middle tile
  int middleTile = gridSizeY/2;
  lLinePos = middleTile; rLinePos = middleTile;
  computerLineMove = lLinePos;

  int distFromEdge = (gridSizeY-lineWidth)/2; // finds how far the lines will be from the edges

  int i = 0;
  for (int y = 0; y < gridSizeY; y++){ // loop through the y axis
    if(y > distFromEdge-1 && y < gridSizeY-distFromEdge){
      //tiles[lLineCollum][y].isSolid = true;
      lLineTiles[i] = y;
      tiles[rLineCollum][y].isSolid = true;
      rLineTiles[i] = y;

      i++;
    }
  }
}
void SetupBall(){
  // set the ball to the middle of the screen
  ballX = gridSizeX/2;
  ballY = gridSizeY/2;
  //set the velocity of the ball
  xVel = cos(angle) * speed;
  yVel = sin(angle) * speed;

  MoveLines();
  MoveBall();
}

void PrintMap() { //solely for debugging
  for (int y = 0; y < gridSizeY; y++){
    Serial.println(); // new line for each row of tiles in the grid (y).
    for (int x = 0; x < gridSizeX; x++){
      Tile t = tiles[x][y];
      if(t.isSolid) Serial.print(" # ");
      else          Serial.print(" . ");
    }
  }
  Serial.println(); // new line for actual data
}

void MoveBall(){
  float nextXPos = (ballX+xVel);
  float nextYPos = (ballY+yVel);
  if(nextYPos <= 1 || nextYPos >= gridSizeY-2)  // flip if next position is a wall
  {
    yVel *= -1;
    angle = RandomAngle(angle);
  }
  if(tileBallX == 1 || tileBallX == gridSizeX-2) // check if ball has hit goal
  {
    // set ball to center
    ballX = gridSizeX/2;
    ballY = gridSizeY/2;
  }
  
  // check if ball hits the right line
  if(tileBallX >= rLineCollum-1 && Normalize(xVel) == 1){
    xVel *= -1; // flip
    angle = RandomAngle(angle);
  }
  // check if ball hits the left line
  if(tileBallX <= lLineCollum+1 && Normalize(xVel) == -1){
    xVel *= -1; // flip
    angle = RandomAngle(angle);
  }

  ballX += xVel;
  ballY += yVel;
  //set the tile representing the ball to the ball's closest tile
  tileBallX = (int)(ballX+0.5f);
  tileBallY = (int)(ballY+0.5f);

  tiles[tileBallX][tileBallY].isSolid = true;
}

float RandomAngle(float origAngle)
{
  float n = random(-5, 5);
  
  int xVelAbs = Normalize(xVel);
  int yVelAbs = Normalize(yVel);

  xVel = cos(angle) * xVelAbs * speed;
  yVel = sin(angle) * yVelAbs * speed;

  return origAngle + n;
}

void MoveLines()
{
  #pragma region groundLine  
  for (int i = 0; i < lineWidth; i++){
    tiles[lLineCollum][lLineTiles[i]].isSolid = false; // disable the tiles for the line temporarily
    lLineTiles[i] -= lLinePos-computerLineMove; // move tiles based on input
    tiles[lLineCollum][lLineTiles[i]].isSolid = true; // enable the new tiles       
  }
  lLinePos = computerLineMove;
  #pragma endregion groundLine

  #pragma region aiLine
  desiredAiPos = tileBallY;
  if(desiredAiPos < lineWidth-1-lineChonk && desiredAiPos > lineChonk){
    for (int j = 0; j < lineWidth; j++){
      tiles[rLineCollum][rLineTiles[j]].isSolid = false; // disable the tiles for the line temporarily
      rLineTiles[j] -= rLinePos-desiredAiPos; // move tiles based on input
      tiles[rLineCollum][rLineTiles[j]].isSolid = true; // enable the new tiles       
    }
  }
  rLinePos = desiredAiPos;
  #pragma endregion aiLine
}

void Communicate() {
  //Serial.println("Hello World!");
  String val;
  while (Serial.available() > 0) {
    val = val + (char)Serial.read(); // read data byte by byte and store it
    delay(1);
  }
  /*bool containsPingsign = val.indexOf("ping") >= 0; // check for the callsigs
  if(containsPingsign)
  {
    float d[] = {
        millis()
    };
    PrintData("", d, 1);
    Serial.println(val);
  }*/
  //bool containsCallsign = val.indexOf(comCallsign) >= 0; // check for the callsigs
  //Serial.print(val + "repeater");
  //bool containsCallsign = val.substring(0) == comCallsign; // check for the callsigs
  //if(containsCallsign)

    //val.remove(0, 7);
    //val.remove(5,val.length()-5);
    //val.remove(0, val.length()-11);
    //Serial.println(val);
    //val.remove(val.length()-1, 1);
    float n = val.toFloat(); // converts the input to a number (returns 0 if input was a string)
    
    //Serial.print(n);
    //Serial.print(val);
    if (n != 0) {
      computerLineMove = (int)n;
    }
}
#pragma endregion pong

void PrintData(String callsign, float data[], int length){
  Serial.println();
  Serial.print(callsign);

  for(int i = 0; i < length; i++)
  {
    Serial.print(data[i]);
    Serial.print(", ");
  }
}

int Normalize(float n){
  if(n >= 0)  return 1;
  else        return -1;
}
