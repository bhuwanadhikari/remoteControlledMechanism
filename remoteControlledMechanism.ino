// YELLOW IS FOR EGG------- PIN 7 TO YELLOW PIN
// RED IS FOR TEA---------- PIN 8 TO BLACK LEFTMOST PIN

#include <IRremote.h>
#define RELAYYELLOW 8
#define RELAYRED 7
#define RECV_PIN 6
#define LEDYELLOW 3
#define LEDRED 2
#define MINS_OFF_TIME 15
#define BLINK_TIME_MS 1000

long int numbers[10] = {33480735,33444015,33478695,33486855 , 33435855,  33468495 ,33452175, 33423615, 33484815,33462375 };
long int red = 33441975;
long int mode = 33446055;
long int mute = 33454215;
long int play = 33456255;
long int front = 33472575;
long int back = 33439935 ;
long int eq =33431775 ;
long int minus = 33464415;
long int plus = 33448095;
long int rpt = 33427695;
long int usd = 33460335;

bool redrelay = false;
bool yellowrelay = false;
IRrecv irrecv(RECV_PIN);
decode_results results;

unsigned long delayinmins = 0;
bool timerrelayyellow = false;
bool timerrelayred = false;

bool blinkyellow = false;
bool blinkred = false;
unsigned long nexttoggleyellow = 0;
unsigned long nexttogglered = 0;
bool onred = false;
bool onyellow = false;

unsigned long delayoffrelayred = 0;
unsigned long delayoffrelayyellow = 0;

unsigned long delayinmsrelayred = 0;
unsigned long delayinmsrelayyellow = 0;

void blinkredfn(bool);
void blinkyellowfn(bool);
void relaywrite(int, bool);
void(* resetFunc) (void) = 0;
void setup() {
    pinMode(RELAYRED, OUTPUT);
    pinMode(RELAYYELLOW, OUTPUT);
    pinMode(LEDRED, OUTPUT);
    pinMode(LEDYELLOW, OUTPUT);

    digitalWrite(LEDRED, LOW);
    digitalWrite(LEDYELLOW, LOW);
    digitalWrite(RELAYRED, LOW);  //initially off
    digitalWrite(RELAYYELLOW, LOW);
    
    Serial.begin(9600);
    irrecv.enableIRIn(); // Start the receiver
}


void loop() {
//  Serial.print("millis output");Serial.println(millis());
  if (delayinmsrelayred){
    if (delayinmsrelayred - millis() <= 0){
      relayWrite(RELAYRED, HIGH);
    
    }
  }
  if (delayinmsrelayyellow){
    if (delayinmsrelayyellow - millis() <= 0){
      relayWrite(RELAYYELLOW, HIGH);
    }
  }

  if (timerrelayred){
    if (delayoffrelayred - millis() <= 0){
      relayWrite(RELAYRED, LOW);
    
    }
  }
  if (timerrelayyellow){
    if (delayoffrelayyellow - millis() <= 0){
      relayWrite(RELAYYELLOW, LOW);
    }
  }

 //blinking mechanism

  if(blinkyellow){
      if (nexttoggleyellow - millis() <= 0){
      onyellow = onyellow ? false : true;
      nexttoggleyellow = millis() + BLINK_TIME_MS;
      digitalWrite(LEDYELLOW, onyellow);
    }
  }
  if(blinkred){
      if (nexttogglered - millis() <= 0){
      onred = onred ? false : true;
      nexttogglered = millis() + BLINK_TIME_MS;
      digitalWrite(LEDRED, onred);
    }
  }






















    if (irrecv.decode(&results)) {
        Serial.println(results.value);
        if (results.value == minus) {                // On/Off button for yellowrelay
            redrelay = (redrelay == LOW) ? HIGH : LOW;
            relayWrite(RELAYRED, redrelay );
            delayinmsrelayred = 0;
            if(redrelay){Serial.println("Red Relay is turned ON");}else{Serial.println("Red Relay is turned OFF");}
        }
        else if (results.value == plus) {          // On/Off button
            yellowrelay = (yellowrelay == LOW) ? HIGH : LOW;
            relayWrite(RELAYYELLOW, yellowrelay);
            delayinmsrelayyellow = 0;
            if(yellowrelay){Serial.println("Yellow Relay is turned ON");}else{Serial.println("Yellow Relay is turned OFF");}
        }
        else if (results.value == red){
          resetFunc();
        }
        else if (results.value == mode) { 
            digitalWrite(LEDYELLOW, HIGH);
            irrecv.resume();  
            bool end = false;
            while (true){
            
              if (irrecv.decode(&results)){
                  Serial.println("Input Recieved ");
                  for (int i = 0; i < 10; i++){
                    if (numbers[i] == results.value){delayinmins = i; end = true; 
                      digitalWrite(LEDYELLOW, LOW);
                      delay(100);
                      digitalWrite(LEDYELLOW, HIGH);
                      break;}
                  }
                irrecv.resume();
            }
            
            if (end == true) break;
          }
          end = false;
          while (1){
            
            if (irrecv.decode(&results)){
              Serial.println("Input Recieved ");
              for (int i = 0;  i < 10; i++){
                if (numbers[i] == results.value){delayinmins = 10* delayinmins + i;
                end = true;
                digitalWrite(LEDYELLOW, LOW);
                delay(100);
                digitalWrite(LEDYELLOW, HIGH);
                break;
                }
                
              }
              Serial.println(delayinmins);
              irrecv.resume();
            }
            
            if (end == true) break;
          }

          end = false;
          while (1){
            if (irrecv.decode(&results)){
              
              if(results.value == minus){ 
                end == true; 
                delayinmsrelayred = (delayinmins * 60000) + millis(); 
                Serial.print ("Timer for red relay:");
                Serial.print(delayinmsrelayred-millis());
                Serial.println(" Milliseconds"); 
                blinkredfn(true);
                break;
                }
              else if (results.value == plus){ 
                end == true; 
                delayinmsrelayyellow = (delayinmins * 60 * 1000) + millis();
                Serial.print ("Timer for yellow relay:");
                Serial.print(delayinmsrelayyellow-millis());
                Serial.println(" Milliseconds");
                blinkyellowfn(true);
                break;
                }
                irrecv.resume();
            }

          }
          digitalWrite(LEDYELLOW, LOW);
        }
        irrecv.resume();
    }
}
    


void relayWrite(int relaynumber, bool state){
  if (state == HIGH){
    if (relaynumber == RELAYYELLOW){
      timerrelayyellow = true;
      delayoffrelayyellow = MINS_OFF_TIME * 60000 + millis();
      blinkyellowfn(false);
    }
    else if (relaynumber == RELAYRED){
      timerrelayred = true;
      delayoffrelayred = MINS_OFF_TIME * 60000 + millis();
      blinkredfn(false);
    }
  }
  if (state == LOW){
    if (relaynumber == RELAYYELLOW){
      timerrelayyellow = false;
      delayoffrelayyellow = 0;
      
    }
    else if (relaynumber == RELAYRED){
      timerrelayred = false;
      delayoffrelayred = 0;
      
    }
  }
  
  digitalWrite(relaynumber, state);
}


void blinkyellowfn(bool on){
  if (on){
    blinkyellow = true;
    nexttoggleyellow = millis() + BLINK_TIME_MS;
  }
  if (!on){
    blinkyellow = false;
    nexttoggleyellow = 0;
    digitalWrite(LEDYELLOW, LOW);
  }
}
void blinkredfn(bool on){
  if (on){
    blinkred = true;
    nexttogglered = millis() + BLINK_TIME_MS;
  }
  if (!on){
    blinkred = false;
    nexttogglered = 0;
    digitalWrite(LEDRED, LOW);
  }
}
