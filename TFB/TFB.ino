class Tile {
  public:
    bool isSolid = false;
};

#define gridSizeX 15
#define gridSizeY 15
Tile tiles [gridSizeX][gridSizeY];

int lLineCollum, rLineCollum;
#define lineFromWall 3
#define lineWidth 5
float ballX, ballY;
int tileBallX, tileBallY;

void setup() {
  Serial.begin(9600); // begin transmission
  SetupMap();
  pinMode(6, OUTPUT);
}
void loop() {
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

  for (int y = 0; y < gridSizeY; y++){ // loop through the y axis
    if(y > distFromEdge-1 && y < gridSizeY-distFromEdge){
      tiles[lLineCollum][y].isSolid = true;
      tiles[rLineCollum][y].isSolid = true;
    }
  }
}
void SetupBall(){
  // set the ball to the middle of the screen
  ballX = gridSizeX/2;
  ballY = gridSizeY/2;
  
  tileBallX = (int)(ballX+0.5f);
  tileBallY = (int)(ballY+0.5f);
  MoveBall();
}

void PrintMap() {
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
  // code
  tiles[tileBallX][tileBallY].isSolid = true;
}
void Communicate() {
  String val;
  while (Serial.available() > 0) {
    val = val + (char)Serial.read(); // read data byte by byte and store it
  }
  float n = val.toFloat(); // converts the input to a number (returns 0 if input was a string)
  if (n != 0) {
    Serial.println(n);
    if(n == 6.0f){
      digitalWrite(6, HIGH);
    }
  }
}
