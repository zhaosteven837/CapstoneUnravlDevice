#include <Arduino.h>
#include <SparkFun_TB6612.h>
#include <Adafruit_LSM6DSO32.h>
#include <Servo.h>
#include <Encoder.h>
#include <Bounce.h>
#include <elapsedMillis.h>

#define RED 3
#define GREEN 4
#define BLUE 5
#define LIMIT1 1
#define BUTTON 2
#define CLAMP1 6
#define CLAMP2 7
#define POT 14
#define AIN1 19
#define AIN2 20
#define PWMA 18
#define STBY 21

Motor linearMotion = Motor(AIN1, AIN2, PWMA, 1, STBY);
Servo tine;
Servo clampA;
Servo clampB;
Encoder linearMotor(11, 12);
int red = 3;
int green = 4;
int blue = 5;
int limit1 = 0;
int limit2 = 1;
int button = 2;
int pot = 14;
int state = -1;
int maxDuty = 255;
int minDuty = -255;
int encoderSmall [12] = {50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600};
int* currSetting;
int size = 12;
int dutyCycle = 0;
float scale = 0.1;
long count = 0;
int arrayPos = 0;
int accel = 5;
elapsedMillis currTime;
elapsedMillis encoderTime;
elapsedMillis motorTime;
elapsedMillis buttonTime;
Bounce pushbutton = Bounce(BUTTON, 10);


// start servo at 60-70
// put function declarations here:
bool buttonHold(int time);
void setLED(int r, int g, int b);

void setup() {
    Serial.begin(115200);
    tine.attach(23);
    clampA.attach(6);
    clampB.attach(7);
    pinMode(red, OUTPUT);
    pinMode(blue, OUTPUT);
    pinMode(green, OUTPUT);
    pinMode(LIMIT1, INPUT_PULLUP);
    pinMode(pot, INPUT);
    currTime = 0;
    currSetting = encoderSmall;
}

void loop() {
    //int mode = map(analogRead(pot), 0, 1023, 1, 4);
    /*int numIterations;
    int stepSize;
    if (mode < 1) {

    } else if (mode < 0.5) {

    } else {

    }*/
    /*clampA.write(68);
    clampB.write(67);
    delay(5000);
    clampA.write(148);
    clampB.write(147);
    delay(10000);
    Serial.println("What");*/
    if (pushbutton.update()) {
      if (pushbutton.fallingEdge()) {
        buttonTime = 0;
      } else {
        if (buttonTime >= 1000) {
          clampA.write(68);
          clampB.write(67);
          while (digitalRead(limit1) == 1) {
            linearMotion.drive(-10);
          }
          tine.write(10);
          state = 0;
          delay(300);
        } else {
          state = -1;
          while(1);
        }
      }
    }

    switch(state) {
      case 0:
        if (linearMotor.read() > currSetting[arrayPos] || linearMotor.read() < -currSetting[arrayPos]) {
          state = 2;
          currTime = 0;
        }
        if (currTime >= 5) {
          if (dutyCycle < maxDuty) {
            dutyCycle += accel;
          } else {
            state = 1;
          }
          currTime = 0;
        }
        break;
      case 1:
        if (linearMotor.read() > currSetting[arrayPos] || linearMotor.read() < -currSetting[arrayPos]) {
          state = 2;
          currTime = 0;
        }
        break;
      case 2:
        if (currTime >= 1) {
          if (dutyCycle > 0) {
            dutyCycle -= accel;
          } else {
            state = 3;
            tine.write(140);
            currTime = 0;
          }
          currTime = 0;
        }/*
        dutyCycle = 0;
        state = 3;
        currTime = 0;*/
        break;
      case 3: 
        if (currTime >= 300) {
          state = 4;
          currTime = 0;
          encoderTime = 0;
          linearMotor.write(0);
        }
        break;
      case 4: // Bring tine back
        if (linearMotor.read() > currSetting[arrayPos]*0.8 || linearMotor.read() < -currSetting[arrayPos] * 0.8) {
          state = 6;
          currTime = 0;
        }
        if (currTime >= 5) {
          if (dutyCycle > -200) {
            dutyCycle -= accel;
          } else {
            state = 5;
            currTime = 0;
          }
          currTime = 0;
        }
        break;
      case 5:
        if (linearMotor.read() > currSetting[arrayPos] * 0.8 || linearMotor.read() < -currSetting[arrayPos] * 0.8) {
          state = 6;
          currTime = 0;
        }
        break;
      case 6:
        if (digitalRead(limit1) == 0) {
          dutyCycle = 0;
          state = 7;
          tine.write(10);
          currTime = 0;
        } else {
          if (currTime >= 2) {
            if (dutyCycle < -20) {
              dutyCycle -= accel;
            }
            currTime = 0;
          }
        }
        /*
        dutyCycle = 0;
        state = 7;
        currTime = 0;*/
        break;
      case 7:
        if (currTime >= 300) {
          if (arrayPos + 1 < size) {
            arrayPos++;
            state = 0;
            currTime = 0;
            linearMotor.write(0);
          } else {
            arrayPos = 0;
            state = 0;
            currTime = 0;
            linearMotor.write(0);
          }
        }
      default:
        break;
    }

    if (motorTime >= 10) {
      motorTime = 0;
      Serial.print(state);
      Serial.print(", ");
      Serial.print(linearMotor.read());
      Serial.print(", ");
      Serial.println(dutyCycle);
      linearMotion.drive(dutyCycle);
    }



    /*jrk.setTarget(1848);
    delay(50);
    jrk.setTarget(2048);
    delay(2000);
    jrk.setTarget(2248);
    delay(100);
    jrk.setTarget(2048);
    delay(2000);
    jrk.setTarget(1848);
    delay(100);
    jrk.setTarget(2048);
    delay(2000);*/

    /*for(int i = 0; i < 20; ++i) {
      jrk.setTarget(1648);
      delay(10);
    }
    jrk.setTarget(2048);*/
    //jrk.setTarget(1448);
    //delay(50);
    //jrk.setTarget(2048);
    //delay(1000);
    /*for (int i = 0; i < numIterations; ++i) {
        if (digitalRead(button) == 1) {
            jrk.setTarget(2048);
            analogWrite(red, 100);
            while(buttonHold(2) == false);
        }
        for (int j = 0; j < stepSize * i; ++j) {
          jrk.setTarget(2048 + 600 * scale);
          delay(1);
        }
        while(digitalRead(limit2) == 1) {
          jrk.setTarget(2048 - 600 * scale);
          delay(1);
        }
    }*/
    //jrk.setTarget(2048);
    tine.write(10);
    delay(200);
    tine.write(140);
    delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

bool buttonHold(int time) {
  for (int i = 0; i < time * 100; ++i) {
    if (digitalRead(button) == 1) {
      return false;
    }
    delay(10);
  }
  return true;
}

void setLed(int r, int g, int b) {
    analogWrite(red, r);
    analogWrite(green, g);
    analogWrite(blue, b);
}
