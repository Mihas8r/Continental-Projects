//#include <LiquidCrystal.h>
//const int rs = 5, en = 6, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#include <avr/io.h>
#include <avr/interrupt.h>

//timer2 variqbles
volatile byte count;
byte reload = 0x9C; 

volatile int play;
//timer2 interrupt

ISR(TIMER2_COMPA_vect){
    play=1;
    OCR2A=reload;
  }




int LED1=8;    // led1 pin
int LED2=10;     //led2 pin   9
int SW1=2;       // switch1 pin
int SW2=12;        //switch2 pin 3
int SW3=13;        //switch3 pin  4
int potPin=A0;    // potentiometer pin


int SW1state=0;
int SW2state=0;
int SW3state=0;
int lastSW1state=0;
int lastSW2state=0;
int lastSW3state=0;


int counter=0;
bool laneSwitch=false;
bool laneSwitchLeft=false;
bool laneSwitchRight=false;
bool left=false;    //  directions laneSwitchMode;
bool right=false;
int blinks=0;  //  blink counter laneSwitchMode;


bool lastLaneSwitch=false;
bool lastLaneSwitchLeft=false;
bool lastLaneSwitchRight=false;

bool lastNormalLeft=false;
bool lastNormalRight=false;

bool lastHazard=false;
int hazardCounter=0;

int normalCounter=0;
bool normalLeft=false;
bool normalRight=false;
int volum=0;   // potentiometer value
int times=0;   // blink counter normal mode

bool hazardFlag=false;

bool ONphase=false;
int phaseCounter=0;

bool ON=true;
String s1;


int test;
void setup() {

  Serial.begin(9600);
  pinMode(LED1,OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(SW1,INPUT);
  pinMode(SW2,INPUT);
  pinMode(SW3,INPUT);
//  lcd.begin(16, 2);

//timer2 config
 cli();
  TCCR0B =0;    
  OCR2A=reload;   //start count
  TCCR2A = 1<<WGM21;  //select CTC mode 
  TCCR2B= (1<<CS22) | (1<<CS21) | (1<<CS20);    //set prescaler
  TIMSK2 = (1<<OCIE2A);   //output compare match enable ????*
  sei();
}
void loop() {
  Serial.println(play);
/*    if(Serial.available() > 0)
    {
      s1= Serial.readStringUntil('\n');  
    }
  if(s1=="start"){
    ON=1;
    }else if(s1=="stop"){
      ON=0;
      } */  
   while(play==1){
    task();} 
}


void task(){
  if(ON==1){

SW1state=digitalRead(SW1); //         read switch
SW2state=digitalRead(SW2);
SW3state=digitalRead(SW3);
 /*   if(SW3state==HIGH && SW3state != lastSW3state){
           lcd.setCursor(0,1);
           lcd.print("Hazard switch       ");
      }
    else if(SW2state==HIGH && SW2state != lastSW2state){
           lcd.setCursor(0,1);
           lcd.print("Right switch      ");
      }
    else if(SW1state==HIGH && SW1state != lastSW1state){
           lcd.setCursor(0,1);
           lcd.print("Left switch       ");
      }
      if(hazardFlag==0 && normalLeft==false && normalRight==false && laneSwitchLeft== false && laneSwitchRight==false){
            lcd.setCursor(0,0);
            lcd.print("NO MODE     ");
        } */
if(hazardFlag==1){
  if(hazardFlag != lastHazard){
      //  lcd.setCursor(0,0);
      //  lcd.print("HAZARD MODE       ");
    }
  counter=0;
  normalCounter=0;
  left=false;
  right=false;
  laneSwitchLeft=false;
  laneSwitchRight=false;
  normalLeft=false;
  normalRight=false;
    hazardMode();
  }
    if(hazardFlag==0 && hazardFlag != lastHazard){
        resetAll();
//        lcd.clear();
      }
    


  if(SW3state==HIGH){
    hazardCounter++;
    }else{
      if(hazardCounter>=20){
        hazardCounter=0;
         if(hazardFlag==false){
      hazardFlag=true;
      }else{
        hazardFlag=false;
       }
      }
     }
   
     

      

  
if(SW2state==HIGH){
  counter++;
  right=true;
  left=false;
  blinks=0;
  }

 if(SW1state==HIGH){
  counter++;
  right=false;
  left=true;
  blinks=0;
 }
  if(SW1state==LOW && lastSW1state  != SW1state){
  
   if( counter<50 && counter !=0){
    
    ONphase=false;
    phaseCounter=0;
    digitalWrite(LED2,LOW);
     laneSwitch=true;
    }
     counter=0;
    }

      
  if(SW2state==LOW && lastSW2state  != SW2state){
  
       if( counter<50 && counter !=0){
        ONphase=false;
        phaseCounter=0;
        digitalWrite(LED1,LOW);
           laneSwitch=true;
           }
        counter=0;
      }
      
       // Serial.println(counter);
    
if( laneSwitch ==true && SW1state==LOW && SW2state==LOW && hazardFlag==LOW &&
    normalLeft ==false && normalRight==false)
  {
    if(left==true && right==false){
      laneSwitchLeft=true;
      laneSwitchRight=false;   
    }
    if(left==false && right== true){
      laneSwitchLeft=false;
      laneSwitchRight=true;
      }
  }

  if(laneSwitchLeft==true && hazardFlag == false && normalLeft == false && normalRight== false && blinks<6){
   BlingBling(LED1,30,70);
      if(blinks == 4){
        digitalWrite(LED1,LOW);
      resetSwitchLane();
    }
  }



 if(laneSwitchRight==true && hazardFlag == false && normalLeft == false && normalRight== false && blinks<6){
    BlingBling(LED2,30,70);
      if(blinks == 4){
        digitalWrite(LED2,LOW);
    resetSwitchLane();
    }
 }


  if(counter>50 && hazardFlag ==false){
    if(SW1state){
      if(SW1state != lastSW1state){
   //     lcd.setCursor(0,0);
   //     lcd.print("normal left     " ); 
        }
    resetSwitchLane();
        normalLeft=true;

         
      volum=analogRead(potPin);
      times=map(volum,0,1023,10,100 );
        BlingBling(LED1,times/2,times/2); 
        digitalWrite(LED2,LOW);       
      }
    if(SW2state){
            if(SW2state != lastSW2state){
     //   lcd.setCursor(0,0);
     //   lcd.print("normal left     " ); 
        }
    resetSwitchLane();
        normalRight=true;

          
      volum=analogRead(potPin);
      times=map(volum,0,1023,10,100);
        BlingBling(LED2,times/2,times/2);
        digitalWrite(LED1,LOW);
       }
    }
    
  if(normalLeft==true && hazardFlag==false && normalCounter<10){   //     stop normal mode
    if(SW1state==LOW){
      normalCounter++;  
      BlingBling(LED1,times/2,times/2);     
      }
    }
  if(normalRight==true && hazardFlag==false && normalCounter<10){     //  stop normal mode
    if(SW2state==LOW){
      normalCounter++;
      BlingBling(LED2,times/2,times/2);     
      }
    }
   if(normalCounter==10){
    normalLeft=0;
    normalRight=0;
    normalCounter=0;
    counter=0;
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    }
    if(hazardFlag==0 && laneSwitch==false && laneSwitchLeft==false && laneSwitchRight==false && normalLeft == false && normalRight == false){
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
  }

    
    /*  if(laneSwitchLeft == true && laneSwitchLeft != lastLaneSwitchLeft && SW1state == LOW && normalLeft==false){
            lcd.setCursor(0,0);
            lcd.print("ls  left       ");
        }else if(laneSwitchRight == true && laneSwitchRight != lastLaneSwitchRight && SW2state == LOW && normalRight==false){
          lcd.setCursor(0,0);
          lcd.print("ls right      ");
        }
*/
     lastSW1state=SW1state;
     lastSW2state=SW2state;
     lastSW3state=SW3state;

     lastLaneSwitch=laneSwitch;
     lastLaneSwitchLeft=laneSwitchLeft;
     lastLaneSwitchRight=laneSwitchRight;

     lastNormalLeft=normalLeft;
     lastNormalRight=normalRight;

     lastHazard=false;
     play=0;
}else{
  resetAll();
  }
  }

void hazardMode(){
  int onPhaseTime=50;
  int offPhaseTime=50;
  if(phaseCounter>onPhaseTime){
    phaseCounter=0;
    }
        if(phaseCounter < onPhaseTime && ONphase==true){
        phaseCounter++;
        if(phaseCounter==onPhaseTime){
          ONphase=false;
          phaseCounter=0;
          }
        }
      if(phaseCounter<offPhaseTime && ONphase==false){
        phaseCounter++;
        if(phaseCounter==offPhaseTime){
          ONphase=true;
          phaseCounter=0;
          }
        }
        if(ONphase==true){
            digitalWrite(LED1,HIGH);
            digitalWrite(LED2,HIGH);
          }if(ONphase==false){
            digitalWrite(LED2,LOW);
            digitalWrite(LED1,LOW);
          }
  }



  void BlingBling(int led,int onPhaseTime, int offPhaseTime){
     if(ONphase==true && phaseCounter>onPhaseTime){
        phaseCounter=0;
      }else if(ONphase==false && phaseCounter >= offPhaseTime){
        phaseCounter=0;
      }
      if(phaseCounter < onPhaseTime && ONphase==true){
        phaseCounter++;
        if(phaseCounter>=onPhaseTime){
          ONphase=false;
          phaseCounter=0;
          }
        }else if(phaseCounter<=offPhaseTime && ONphase==false){
         phaseCounter++;
         if(phaseCounter>=offPhaseTime){
          ONphase=true;
          phaseCounter=0;
          blinks++;
          }
        }
        if(ONphase==true){
            digitalWrite(led,HIGH);
          }else{
            digitalWrite(led,LOW);
          }
    }
void resetSwitchLane(){
          laneSwitch=false;
          laneSwitchLeft=false;
          laneSwitchRight=false;
          left=false;
          right=false;
         
          blinks=0;
         //     digitalWrite(LED1,LOW);
         //     digitalWrite(LED2,LOW);
  }

  void resetAll(){
SW1state=0;
SW2state=0;
SW3state=0;
lastSW1state=0;
lastSW2state=0;
lastSW3state=0;
counter=0;
laneSwitch=false;
laneSwitchLeft=false;
laneSwitchRight=false;
left=false;  //    directions laneSwitchMode;
right=false;
blinks=0;  //  blink counter laneSwitchMode;
normalCounter=0;
normalLeft=false;
normalRight=false;
volum=0;   /// potentiometer value
times=0;    //blink counter normal mode
 hazardFlag=false;
 ONphase=false;
phaseCounter=0;
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  //    lcd.setCursor(0,0);
   //   lcd.print("NOT WORKING   ");
    }
