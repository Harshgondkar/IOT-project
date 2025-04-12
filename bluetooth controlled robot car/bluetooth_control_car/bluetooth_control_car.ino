int in1 = 10;
int in2 = 9;
int in3 = 8;
int in4 = 7;
int ena = 11;
int enb = 6;
 
char command;
 
void setup()
{      
  Serial.begin(9600);  //Set the baud rate to your Bluetooth module.
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(ena, OUTPUT);
  pinMode(enb, OUTPUT);
}
 
void loop(){
  if(Serial.available() > 0){
    command = Serial.read();
    Stop();
    switch(command){
    case 'F':  
      forward();
      break;
    case 'B':  
       back();
      break;
    case 'L':  
      left();
      break;
    case 'R':
      right();
      break;
    }
  }
}
 
void forward()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
    analogWrite(ena, 180);
    analogWrite(enb, 180);
}
 
void back()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
    analogWrite(ena, 180);
    analogWrite(enb, 180);
}
 
void left()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
    analogWrite(ena, 180);
    analogWrite(enb, 180);
}
 
void right()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
    analogWrite(ena, 180);
    analogWrite(enb, 180);
}
 
void Stop()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}