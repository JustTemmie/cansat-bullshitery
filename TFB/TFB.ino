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
float ballX, ballY;
int tileBallX, tileBallY;

float angle = 30;
float speed = 1;
float xVel, yVel;
#define delayTime 150

int lLineTiles [lineWidth];
int rLineTiles [lineWidth];
int computerLineMove;

void setup() {
  Serial.begin(9600); // begin transmission
  SetupMap();
  //pinMode(6, OUTPUT);
}
void loop() {
  delay(delayTime);
  MoveLines();
  MoveBall();
  PrintMap();
  Communicate();
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
  PrintMap();
}
void InstertLines()
{
  // set the collums where the lines will be placed
  lLineCollum = lineFromWall-1;
  rLineCollum = gridSizeX - lineFromWall;

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
  bool atEdge = false;

  // check if it's trying to move into a wall)
  atEdge =  (lLineTiles[0] == 1 && computerLineMove == 1) ||
            (lLineTiles[lineWidth-1] == gridSizeY-2 && computerLineMove == -1);

  if(!atEdge)
  {
    for (int i = 0; i < lineWidth; i++){
      tiles[lLineCollum][lLineTiles[i]].isSolid = false; // disable the tiles for the line temporarily
      lLineTiles[i]-=computerLineMove; // move tiles based on input
      tiles[lLineCollum][lLineTiles[i]].isSolid = true; // enable the new tiles       
    }
  }
  computerLineMove = 0;
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
