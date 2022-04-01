int gridSizeX, gridSizeY;


void setup() {
  Serial.begin(9600); // begin transmission
}
void loop() {
  Communicate();
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
    int x, y;
};