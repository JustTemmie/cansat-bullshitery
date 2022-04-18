class Tile {
  public:
    bool isSolid = false;
};

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
#define delayTime 150

int lLineTiles [lineWidth];
int rLineTiles [lineWidth];
int computerLineMove;
int desiredAiPos;
int lLinePos;
int rLinePos;

void setup() {
  Serial.begin(9600); // begin transmission
  SetupMap();
  //pinMode(6, OUTPUT);
  Serial.println();
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
  MoveLines();
  MoveBall();
  delay(delayTime);
  //PrintMap();
  Communicate();

  float d[] = {
    ballX,
    ballY,
    angle,
    computerLineMove,
    desiredAiPos
  };
  PrintBallInfo(d);
}

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
  
  //find the next position to check if there's a tile in the way
  int nextXPos = (int)(ballX+xVel+0.5f);
  if(tiles[nextXPos][tileBallY].isSolid) {
    xVel *= -1;
  }
  int nextYPos = (int)(ballY+yVel+0.5f);
  if(tiles[nextYPos][tileBallX].isSolid) {
    yVel *= -1;
  }
  //move the ball
  ballX += xVel;
  ballY += yVel;
  //set the tile representing the ball to the ball's closest tile
  tileBallX = (int)(ballX+0.5f);
  tileBallY = (int)(ballY+0.5f);

  tiles[tileBallX][tileBallY].isSolid = true;
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

void PrintBallInfo(float data[]){
  Serial.println();
  Serial.print("TFB");

  for(int i = 0; i < 5; i++)
  {
    Serial.print(", ");
    Serial.print(data[i]);
  }
}

void Communicate() {
  //Serial.println("Hello World!");
  String val;
  while (Serial.available() > 0) {
    val = val + (char)Serial.read(); // read data byte by byte and store it
  }
  float n = val.toFloat(); // converts the input to a number (returns 0 if input was a string)
  if (n != 0) {
    computerLineMove = (int)n;
    Serial.print(computerLineMove);
  }
}
