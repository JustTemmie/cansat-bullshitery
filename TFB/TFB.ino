class Tile {
  public:
    bool isSolid = false;
};

//https://store.steampowered.com/app/1718240/Beaver_Clicker/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
//#include <NMEAGPS.h>

using namespace std;

Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

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
float speed = 1;
float xVel, yVel;
#define delayTime 100

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

  InitBMP();

  SetupMap();
  //pinMode(6, OUTPUT);
  Serial.println();
  Serial.println("Callsign, BMPTemp, LMTemp, NTCTemp, BMPPressure, MPXPressure, BMPCalculatedAltitude");
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
  ReadBMP();
  float d[] = {
    temp,
    ReadLM(0, 1), //offset, sensitivity
    ReadNTC(0, 1), //off, grad
    pressure,
    ReadMPX(),
    BMPAltutude(991)
  };
  PrintData(Callsign, d, 6);

  MoveLines();
  MoveBall();
  delay(delayTime);
  //PrintMap();
  Communicate();

  float ballData[] = {
    ballX,
    ballY,
    angle,
    computerLineMove,
    desiredAiPos
  };
  PrintData(pongCallsign, ballData, 5);
}

#pragma region primary
void InitBMP() {
  unsigned status;
  status = bmp.begin();
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();
}

void ReadBMP() {
  float f = bmp.readTemperature();
  
  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);

  temp = temp_event.temperature;
  pressure = pressure_event.pressure;
}
float BMPAltutude(float seaPressure){
  return bmp.readAltitude(seaPressure);
}
float ReadMPX() {
  float volt = BitToVolt(1);
  return 10*(volt/(0.009*5)+(0.095/0.009));
}
float ReadLM(float lmOffset, float lmSens){
  float volt = BitToVolt(2);
  return (volt - lmOffset)*lmSens;
}
float ReadNTC(float ntcOff, float ntcGrad) {
  float bit = analogRead(A0);
  float volt = BitToVolt(0);
  return ntcOff + ntcGrad * volt;
}

float BitToVolt(int n){    //Function to convert raw ADC-data (0-255) to volt
  int raw = analogRead(n);
  float volt = (float)raw*5.000/1023;
  return volt;
}
#pragma endregion primary

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
  xVel = cos(angle);
  yVel = sin(angle);

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
  tiles[tileBallX][tileBallY].isSolid = false;
  
  /*//find the next position to check if there's a tile in the way
  int nextXPos = (int)(ballX+xVel+0.5f);
  if(tiles[nextXPos][tileBallY].isSolid) {
    xVel *= -1;
  }
  int nextYPos = (int)(ballY+yVel+0.5f);
  if(tiles[nextYPos][tileBallX].isSolid) {
    yVel *= -1;
  }*/
  //move the ball

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
  if(tileBallX == rLineCollum-1){
    if(nextYPos <= lLinePos+lineChonk && nextYPos >= lLinePos-lineChonk){
      xVel *= -1; // flip
      angle = RandomAngle(angle);
    }
  }
  // check if ball hits the left line
  if(tileBallX == rLineCollum+1){
    if(nextYPos <= rLinePos+lineChonk && nextYPos >= rLinePos-lineChonk){
      xVel *= -1; // flip
      angle = RandomAngle(angle);
    }
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

  xVel = cos(angle) * xVelAbs;
  yVel = sin(angle) * yVelAbs;

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
  if(desiredAiPos < lineWidth-1-lineChonk && desiredAiPos > lineChonk)
  for (int j = 0; j < lineWidth; j++){
    tiles[rLineCollum][rLineTiles[j]].isSolid = false; // disable the tiles for the line temporarily
    rLineTiles[j] -= rLinePos-desiredAiPos; // move tiles based on input
    tiles[rLineCollum][rLineTiles[j]].isSolid = true; // enable the new tiles       
  }
  rLinePos = desiredAiPos;
  #pragma endregion aiLine
}

void Communicate() {
  //Serial.println("Hello World!");
  String val;
  while (Serial.available() > 0) {
    val = val + (char)Serial.read(); // read data byte by byte and store it
  }
  bool containsCallsign = val.indexOf(comCallsign) >= 0; // check for the callsigs
  if(containsCallsign)
  {
    val.remove(0, 5);
    float n = val.toFloat(); // converts the input to a number (returns 0 if input was a string)
    if (n != 0) {
      computerLineMove = (int)n;
      //Serial.print(n);
    }
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
  Serial.print("A"); // look idk man
}

int Normalize(float n){
  if(n >= 0)  return 1;
  else        return -1;
}