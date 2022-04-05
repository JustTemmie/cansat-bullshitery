#define gridSizeX 20
#define gridSizeY 20
Tile tiles [gridSizeX][gridSizeY];

void setup() {
  Serial.begin(9600); // begin transmission
  SetupMap();
}
void loop() {
  Communicate();
}

void SetupMap()
{
  for (int x = 0; x < gridSizeX; x++){
    for (int y = 0; y < gridSizeY; y++){
      if((x == 0 || x == gridSizeX) || (y == 0)) tiles[x][y].isSolid = true;
    }
  }
  PrintMap();
}

void PrintMap() {
  for (int x = 0; x < gridSizeX; x++){
    Serial.println();
    for (int y = 0; y < gridSizeY; y++){
      Tile t = tiles[x][y];
      if(t.isSolid) Serial.print("#");
      else          Serial.print(".");
    }
  }
}
void Communicate() {
  String val;
  while (Serial.available() > 0) {
    val = val + (char)Serial.read(); // read data byte by byte and store it
  }
  float n = val.toFloat();
  if (n != 0) {
    Serial.println(n); // send the received data back to raspberry pi
  }
}

class Tile {
  public:
    bool isSolid = false;
};