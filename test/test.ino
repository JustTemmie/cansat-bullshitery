void setup(){
  Serial.begin(9600); // begin transmission

}

void loop() {
  String val;
  while (Serial.available() > 0) {
    val = val + (char)Serial.read(); // read data byte by byte and store it
    delay(1);
  }

    Serial.println();

}
