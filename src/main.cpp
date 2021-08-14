#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h>

#define delayMS 200

unsigned long lastTime = millis();

// IR
#define PIN_IR 5
IRrecv irSensor(PIN_IR);
decode_results irResult;
unsigned long currentCode = 0xFFFFFFFFF;

// RİNG
#define PIN_RING 3
#define NUM_LEDS 24
CRGB ring[NUM_LEDS];
int colors[][3] = {
    {255,255,255},
    {255,0,0},
    {0,255,0},
    {0,0,255},
    {255,255,0},
    {0,255,255},
    {255,0,255},
    {192,192,192},
    {128,128,128},
    {128,0,0},
    {128,128,0},
    {0,128,0},
    {128,0,128},
    {0,128,128},
    {0,0,128},
    {165,42,42},
    {220,20,60},
    {255,165,0},
    {255,215,0},
    {240,230,140},
    {107,142,35},
    {32,178,170},
    {70,130,180},
    {135,206,235},
    {148,0,211},
    {139,0,139},
    {205,133,63}
};

boolean ringStatus = false;
int  ringColors[3] = {255, 255, 255};
CRGB ringColor = CRGB(ringColors[0], ringColors[1], ringColors[2]);
int ringBrightness = 17;
long int ringCounter = 15000000;
String ringMode = "";
boolean ringChange = false;
boolean colorChange = false;

void setRingColor(int r, int g, int b){
    ringMode = "CHANGE_COLOR";
    ringColors[0] = r;
    ringColors[1] = g;
    ringColors[2] = b;
    ringColor = CRGB(r, g, b);
    colorChange = true;
}

// CUSTOMM METHODS
void kumandaTuslari(unsigned long int code){
    switch (code) {
        // ON
        case 0xFF827D:  ringStatus=true;  ringMode="ON"; ringCounter = 15000000; break;
        //OFF
        case 0xFF02FD:  ringStatus=false;  ringMode="OFF";  break;
        //R
        case 0xFF1AE5:  setRingColor(255, 0, 0);         break;
        //G
        case 0xFF9A65:  setRingColor(0, 255, 0);         break;
        //B
        case 0xFFA25D:  setRingColor(0, 0, 255);         break;
        //W
        case 0xFF22DD:  setRingColor(255, 255, 255);     break;
        //SARI
        case 0xFF2AD5:  setRingColor(255, 255, 0);       break;
        //AÇIK MAVİ
        case 0xFFAA55:  setRingColor(153, 204, 255);     break;
        //TURQUAZ
        case 0xFF926D:  setRingColor(153, 255, 204);     break;
        //PEMBE
        case 0xFF12ED:  setRingColor(255, 153, 204);      break;
        //TURUNCU
        case 0xFF0AF5:  setRingColor(255,127,80);        break;
        //MAT YEŞİL
        case 0xFF8A75:  setRingColor(60,179,113);       break;
        //MOR
        case 0xFFB24D:  setRingColor(128, 0, 128);       break;
        //PUDRA
        case 0xFF32CD:  setRingColor(90, 71, 69);       break;
        // IŞIK YÜKSELT
        case 0xFF3AC5: if(ringBrightness<255){ ringBrightness += 17; }  ringMode = "CHANGE_COLOR"; break;
        // IŞIK AZALT
        case 0xFFBA45: if(ringBrightness>17){ ringBrightness -= 17; }  ringMode = "CHANGE_COLOR"; break;
        // 1H
        case 0xFF18E7: FastLED.showColor(CRGB(0,0,0)); delay(500); ringCounter = (1000*60)*30;  break;
        // 2H
        case 0xFF9867: FastLED.showColor(CRGB(0,0,0)); delay(500); ringCounter = (1000*60)*60;  break;
        // 3H
        case 0xFF58A7: FastLED.showColor(CRGB(0,0,0)); delay(500); ringCounter = (1000*60)*90;  break;
        // 4H
        case 0xFFD827: FastLED.showColor(CRGB(0,0,0)); delay(500); ringCounter = (1000*60)*120;  break;
        // R YUKARI
        case 0xFF28D7: if(ringColors[0]<255){ ringColors[0] += 15; }  ringMode = "CHANGE_COLOR";    break;
        // R AŞAĞI
        case 0xFF08F7: if(ringColors[0]>0){ ringColors[0] -= 15; }  ringMode = "CHANGE_COLOR";  break;
        // G YUKARI
        case 0xFFA857: if(ringColors[1]<255){ ringColors[1] += 15; }  ringMode = "CHANGE_COLOR";    break;
        // G AŞAĞI
        case 0xFF8877: if(ringColors[1]>0){ ringColors[1] -= 15; }  ringMode = "CHANGE_COLOR";  break;
        // B YUKARI
        case 0xFF6897: if(ringColors[2]<255){ ringColors[2] += 15; }  ringMode = "CHANGE_COLOR";    break;
        // B AŞAĞI
        case 0xFF48B7: if(ringColors[2]>0){ ringColors[2] -= 15; }  ringMode = "CHANGE_COLOR";  break;
        // DIY1
        case 0xFF30CF: ringMode="DIY1";  break;
        // DIY2
        case 0xFFB04F: return false;  break;
        // DIY3
        case 0xFF708F: return false;  break;
        // DIY4
        case 0xFF10EF: return false;  break;
        // DIY5
        case 0xFF906F: return false;  break;
        // DIY6
        case 0xFF50AF: return false;  break;
        // QUICK
        case 0xFFE817: return false;  break;
        // SLOW
        case 0xFFC837: return false;  break;
        // JUMP3
        case 0xFF20DF: return false;  break;
        // JUMP7
        case 0xFFA05F: return false;  break;
        // FADE3
        case 0xFF609F: return false;  break;
        // FADE7
        case 0xFFE01F: return false;  break;
        // AUTO
        case 0xFFF00F: ringMode="AUTO"; colorChange=true;  break;
        // FLASH
        case 0xFFD02F: return false;  break;
    }
    ringChange = true;

}// kumandaTuslari

// RING MODE ringModeAuto
void ringModeAuto(){
    for(byte i = 0; i<NUM_LEDS; i++){
        int rnd = random(0, 27);
        ring[i].setRGB(colors[rnd][0], colors[rnd][1], colors[rnd][2]);
    }
    FastLED.show();
}//ringModeAuto


// Ring Mode Diy 1
byte rmd1LedKey = 0;
unsigned long rmd1LastTime = millis() ;
void ringModeDiy1(){
    long elapsedTime = millis() - rmd1LastTime;
    if(elapsedTime>150){
        rmd1LastTime=millis();
        FastLED.clear();
        ring[rmd1LedKey].setRGB(ringColors[0], ringColors[1], ringColors[2]);
        FastLED.show();
        rmd1LedKey++;
        if(rmd1LedKey>=NUM_LEDS){ rmd1LedKey=0; }
    }
}


// RING CONTROL
void ringControl(long elapsedTime){
        if(!ringStatus){
            FastLED.clear(true);
        }else{
            FastLED.setBrightness(ringBrightness);
            if(ringCounter>0 && ringCounter<15000000){ ringCounter -= delayMS; }
            if(ringCounter<=0){ ringStatus=false; }

                if(ringMode=="ON"){
                    FastLED.showColor(CRGB( ringColors[0], ringColors[1], ringColors[2] ));
                    ringMode=="";
                }else if(ringMode=="OFF"){
                    FastLED.clear(true);
                    ringMode=="";
                }else if(ringMode=="AUTO"){
                    ringModeAuto();
                    ringMode=="";
                }else if(ringMode=="CHANGE_COLOR"){
                    FastLED.showColor(CRGB( ringColors[0], ringColors[1], ringColors[2] ));
                    ringMode=="";
                }else if(ringMode=="DIY1"){
                    ringModeDiy1();
                }else{

                }

        }//else
}//ringControl



// IRSensor
void IRSensor(){
    if(irSensor.decode(&irResult)){
            if(irResult.value!=0xFFFFFFFF){
                currentCode = irResult.value;
            }
            kumandaTuslari(currentCode);
            irSensor.resume(); // Receive the next value
    } // if(irSensor.decode(&irResult))
}






//SETUP
void setup() {
    Serial.begin(9600);
    irSensor.enableIRIn();
    FastLED.addLeds<NEOPIXEL, PIN_RING>(ring, NUM_LEDS);

}


//LOOP
void loop() {
    long elapsedTime = millis() - lastTime;
    lastTime = lastTime + elapsedTime;



    IRSensor();
    ringControl(elapsedTime);




/*
    for(byte i = 0; i < NUM_LEDS; i++){
        byte rKey = random(0, 27);
        ring[i].setRGB(colors[rKey][0], colors[rKey][1], colors[rKey][2]); delay(50);
        FastLED.show();
    }
*/
    delay(150);
}
