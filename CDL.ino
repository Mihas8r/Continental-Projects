//lcd variables  
//#include <LiquidCrystal.h>
//const int rs = 5, en = 6, d4 = 10, d5 =7, d6 = 12, d7 = 13;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#include <avr/io.h>
#include <avr/interrupt.h>

//timer2 variables
volatile byte count;
byte reload = 0x138; // start 312
volatile int play;
//timer2 interrupt
ISR(TIMER1_COMPA_vect){
 //  count++;
   task();
   OCR1A=reload;
  }
//motor variables
const int controlPin1=3;
const int controlPin2=4;
const int enablePin=6;
const int enablePin2=5;
const int potPin = A0;
int motorSpeed=0;
int motorState;
int t0m1;
int tm1;
int t0m2;
int tm2;
bool aprins =false;

//led variables
int LED_pin=8;
int ledState=0;
int lastLedState=0;
int ledCounter=0;
int requiredBlinks=0;
int blinksCounter=0;
int t0l1=0;     //start led counter
int tl1=0;       //stop led counter


//switch variables
int SW1_pin=2;    
int SW1=0;
int t0s1=0;     //start switch counter
int ts1=0;      //stop switch counter
int SWstate=1;
int lastSWstate=0;

int delta=0;

int go=1;
int doua=0;
//door variables
int doorState=0;    //door state
int lastDoorState=0;

void setup() {

    pinMode(SW1_pin,INPUT);
  pinMode(LED_pin,OUTPUT);
  //lcd.begin(16, 2);
  pinMode(controlPin1,OUTPUT);
  pinMode(controlPin2,OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin,LOW);
  Serial.begin(9600);
  
  //timer2 config
  cli();
  TCCR1A=0;
  TCCR1B=0;
  OCR1A=reload;
  TCCR1B = (1<<WGM12) | (1<<CS12) | (1<<CS10); 
  TIMSK1 = (1<<OCIE1A);
  sei();  
}
void loop() {
  /*  if(count==2){    
       count=0;
       play=1;
       }
    while(play==1){*/
     
}

  void task(){
      motorSpeed=analogRead(potPin);
      SM_switch();
      SM_door();
      SM_led();
      SM_motors();
      if(doorState != lastDoorState){
        ledState=2;
        }
      if((lastSWstate !=SWstate) | (lastLedState != ledState)) {
      Serial.print("Switch State: "); Serial.print(SWstate);
      Serial.print(" | LED State: "); Serial.print(ledState);
      Serial.print(" | Doors State : ");Serial.print(doorState);
      Serial.print(" | Motor State : ");Serial.print(motorState);

      if(doorState==0){Serial.print(" unlocked");
      // lcd.setCursor(0,0);lcd.print("unlocked       ");
          } 
      if(doorState==1){Serial.print(" locked");
     // lcd.setCursor(0,0);lcd.print("locked       ");
      } 
      if(doorState==2){Serial.print(" doublelocked");
     // lcd.setCursor(0,0);lcd.print("2x locked    ");
      } 
      Serial.println(" ");
      }
   play=0;
  

    }
  void clockwise(){
          digitalWrite(controlPin1,LOW);
          digitalWrite(controlPin2,HIGH);
  }
  void anticlockwise(){
          digitalWrite(controlPin1,HIGH);
          digitalWrite(controlPin2,LOW);
  }
void SM_motors(){
  switch(motorState){
    case 0:   //IDLE
    tm1=0;
    t0m1=0;
    tm2=0;
    t0m2=0;
    doua=0;
        if(doorState==1 && doorState != lastDoorState){t0m1=millis();clockwise();analogWrite(enablePin,motorSpeed);motorState=1;analogWrite(enablePin2,0); }
        if(doorState==2 && doorState != lastDoorState){t0m1=millis();t0m2=millis();clockwise();analogWrite(enablePin,motorSpeed);analogWrite(enablePin2,motorSpeed);motorState=2;}
        if(doorState==0 && doorState != lastDoorState){t0m1=millis();anticlockwise();analogWrite(enablePin,motorSpeed);
              if(SWstate==6){t0m2=millis();analogWrite(enablePin2,motorSpeed);
              }
              motorState=3; 
}
    break;

    case 1:   //RUN
         tm1=millis();
         if(tm1-t0m1>=1000){analogWrite(enablePin,0);motorState=0;}
    break;
    case 2:  
          tm1=millis();
          tm2=millis();
          if(tm1-t0m1 >= 2000){  analogWrite(enablePin,0);}
          if(tm2-t0m2 >= 3000){analogWrite(enablePin2,0); motorState=0;}
    break;

    case 3:
         tm1=millis();
         tm2=millis();
         if(tm1-t0m1 >= 3000 && go == 1){analogWrite(enablePin,0);}
         if(tm1-t0m1 >= 2000 && go == 2){analogWrite(enablePin,0);}
         if(tm2-t0m2 >= 3000){analogWrite(enablePin2,0);motorState=0; }
    break;
    }
  }    
void SM_door(){
  lastDoorState = doorState;
  switch(doorState){
    case 0:     //UNLOCKED
        if(SWstate==4 && delta >=  800 ){requiredBlinks=1;doorState=1;go=0;}
    break;

    case 1:      //LOCKED
        if(SWstate==6 && delta >= 800){requiredBlinks=2;doorState=0;go=1;}
        if(SWstate==4 && delta >= 100 && delta <=800){doorState=2;requiredBlinks=3;go=1;}
    break;

    case 2:       //DOUBLE LOCKED
        if(SWstate==6 && delta >=800){requiredBlinks=2;doorState=0;go=2;}
    break;
    }
  }
void SM_switch(){
  SW1=digitalRead(SW1_pin);
  lastSWstate=SWstate;
  switch(SWstate){
    case 0:     //RESET
        SWstate=1;
          delta=0;
    break;

    case 1:   //WAIT
        if(SW1==HIGH){SWstate=2;  go=1;}
    break;

    case 2:     //ARMING
        t0s1=millis();
        SWstate=3;
    break;

    case 3:     //DRAWN
        ts1=millis();
        delta=ts1-t0s1;
        if(SW1==LOW){SWstate=4;break;}                          //pentru double locked 
        if(ts1-t0s1>=800 && (doorState==1 || doorState==2)){SWstate=5;}     //din locked in unlocked
        if(ts1-t0s1>=800 && doorState==0){SWstate=3;}     //din unlocked in locked 
    break;

    case 4:       //TRIGGERED
        SWstate=7;
    break;

    case 5:        //HOLDING
        SWstate=6;      

    break;

    case 6:       //HIGH wait
        if(SW1==LOW){SWstate=7;delta=ts1-t0s1;}
    break;

    case 7:       //RELEASED
   
      SWstate=0;
      Serial.println(delta);
    break;
    }
  }
void SM_led(){
    lastLedState= ledState;
    switch(ledState){
      case 1 : //  wait
      break;

      case 2:
            t0l1=millis();
            ledState=3;
            digitalWrite(LED_pin,HIGH);
      break;
            
      case 3: //on
      tl1=millis();
        if(tl1-t0l1>=290){ledState=4;digitalWrite(LED_pin,LOW);t0l1=millis();blinksCounter++;}
      break;
      
      case 4: //turn off
        tl1=millis();
        if(tl1-t0l1>=690){ledState=3;digitalWrite(LED_pin,HIGH);t0l1=millis();}
        if(blinksCounter >= requiredBlinks){ledState=1;ledCounter=0;blinksCounter=0;}
      break;
}
}
