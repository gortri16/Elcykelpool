
#define motor1a 2
#define motorpwm 3
#define motor1b 4
#define relay 5
#define greenled 6
#define redled 7
#define buttonup 8
#define buttondown 9
#define buttonservice 10
#define liftup 11
#define liftdown 12
#define currentsens A0
#define doorclosed A1
#define dooropen A2
#define chargercurrent A3
int speed = 255;
int current = 0;
int fault = 0;



void setup() {
  // Motor
  pinMode(motor1a,OUTPUT);
  pinMode(motor1b,OUTPUT);
  pinMode(motorpwm,OUTPUT);
   pinMode(relay,OUTPUT);
  //Leds
  pinMode(greenled,OUTPUT);
  pinMode(redled,OUTPUT);
  //Buttons digital input
  pinMode(buttonup,INPUT_PULLUP);
  pinMode(buttondown,INPUT_PULLUP);
  pinMode(buttonservice,INPUT_PULLUP);
  //Lift location inputs
  pinMode(liftup,INPUT_PULLUP);
  pinMode(buttonup,INPUT_PULLUP);
  pinMode(buttondown,INPUT_PULLUP);
  //Door state
   pinMode(doorclosed,INPUT_PULLUP);
    pinMode(dooropen,INPUT_PULLUP);
   //currentsens

   
    
    Serial.begin(9600);
    

}

void loop() {
 while(fault == 0){ 
  
  if((digitalRead(buttonup) == HIGH) && (digitalRead(buttondown) == LOW)){motorUp();greenLedOn();}
  else{motorStop();greenLedOf();}
  
   if((digitalRead(buttonup) == LOW)&& (digitalRead(buttondown) == HIGH)){motorDown();redLedOn();}
  else{motorStop;redLedOf();}
 
 if(digitalRead(buttonservice)== LOW){
    if(digitalRead(relay)==HIGH){relayOf();redLedOf();greenLedOf();}
    else{relayOn();redLedOn();greenLedOn();}
    delay(1000);
  }
  //getCurrent();
  delay(1000);
  Serial.println((((analogRead(chargercurrent))*0.0049)-2.5)/0.2);
  if(current > 100000000){motorBreak();fault = 1;}
 }
   while(fault == 1){
    greenLedOn();
    redLedOf();
    delay(1000);                      
    greenLedOf();
    redLedOn();
    delay(1000);
    if((digitalRead(buttonup) == HIGH)&& (digitalRead(buttondown) == HIGH)){fault = 0;}
   }
  
  
  }


void motorUp(){
  digitalWrite(motor1a,HIGH);
  digitalWrite(motor1b,LOW);
  analogWrite(motorpwm,speed);
  }

void motorDown(){
  digitalWrite(motor1a,LOW);
  digitalWrite(motor1b,HIGH);
  analogWrite(motorpwm,speed);
  }
void motorStop(){
  digitalWrite(motor1a,LOW);
  digitalWrite(motor1b,LOW);
  analogWrite(motorpwm,0);
  }
void motorBreak(){
  digitalWrite(motor1a,HIGH);
  digitalWrite(motor1b,HIGH);
  analogWrite(motorpwm,0);
  }
void speedUp(){
  speed = speed +10;
  if(speed > 255){speed = 255;delay(500);}
  }
void speedDown(){
  speed = speed -10;
  if(speed < 0){speed = 0;}
  }
void getCurrent(){
    current = analogRead(currentsens);
    Serial.println(current);
  }
void greenLedOn(){
  digitalWrite(greenled,HIGH);
  }
void greenLedOf(){
  digitalWrite(greenled,LOW);
  }
void redLedOn(){
  digitalWrite(redled,HIGH);
  }
void redLedOf(){
  digitalWrite(redled,LOW);
  }
void relayOn(){
  digitalWrite(relay,HIGH);
  }
void relayOf(){
  digitalWrite(relay,LOW);
  }



