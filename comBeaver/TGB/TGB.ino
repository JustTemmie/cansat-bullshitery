//btw temmie made this script 

int n = 0;


void setup() {
  Serial.begin(9600); // begin transmission
  
}



void loop(){
    //Serial.println("Hello World!");
  String val;
  while (Serial.available() > 0) {
    val = val + (char)Serial.read(); // read data byte by byte and store itq
  }
  Serial.print(val);
  /*Serial.print("   ");
  Serial.println(millis());*/

  delay(10);

  /*float n = val.toFloat(); // converts the input to a number (returns 0 if input was a string)
  if (n != 0) {
    Serial.println(100000+n);
  }*/
}
