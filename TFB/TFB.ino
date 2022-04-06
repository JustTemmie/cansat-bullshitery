int gridSizeX, gridSizeY;


void setup() {
  Serial.begin(9600); // begin transmission
}
void loop() {
  Communicate();
}

void Communicate() {
  Serial.println("something");
  delay(1);
};
