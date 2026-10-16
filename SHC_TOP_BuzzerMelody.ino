/*SHC - The One Plane: Buzzer Melody; Plays A Melody On The Buzzer
Created: 28 September 2026
Modified: 01 October 2026
Running Test
By Arwyn Carlson
Notation Source: https://docs.arduino.cc/built-in-examples/digital/toneMelody/
*/

#include "NoteVoltage.h"
#define BUZZER_PIN 21

//Define BPM, Notes, & Durations - Mr. Sandman
const int BPM = 142;
float notesArr[]{
  NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_G3, NOTE_A3, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, NOTE_A3, NOTE_E3, NOTE_E3, NOTE_D3, NOTE_E3, NOTE_G3, REST, NOTE_D4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_DS4, NOTE_DS4, NOTE_E4, NOTE_D4, REST, NOTE_B3, NOTE_A3, REST, REST, NOTE_B3, NOTE_B3, NOTE_A3, NOTE_B3, REST, REST, REST, NOTE_C4, NOTE_C4, NOTE_B3, NOTE_C4, NOTE_C4, REST, NOTE_B3, NOTE_F3, NOTE_F3, NOTE_E3, NOTE_F3, REST, REST, NOTE_D3, NOTE_F3, NOTE_A3, NOTE_C4, NOTE_C4, NOTE_C4, NOTE_D4, REST, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_C5
};
float durations[]{
  8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 8, 8, 4, 8, 2, 8, 8, 8, 4, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 4, 4, 8, 4, 8, 8, 4, 8, 2, 8, 8, 8, 4, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 8, 4, 8, 8, 8, 2, 4, 8, 8, 4, 4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 2, 1, 8
};

//Basic Set-Up
void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  int size = sizeof(notesArr) / sizeof(int);  //Length Of Array
  for(int noteSeq = 0; noteSeq < size; noteSeq++) { //Iterate Over Notes Of Melody
    float duration = 60 / BPM / (durations[noteSeq]/4); //Duration(sec) Of Given Note; Can Be Function
    tone(BUZZER_PIN, notesArr[noteSeq], duration);
    delay(int(duration * 1.30));  //As Included In Arduino's Documentation, Pause To Distinguish Notes
    noTone(BUZZER_PIN);           //Stop Playing On The Buzzer
  }
  delay(10000); //Delay For Repeat (Testing)
}

/*//Define Note -> Voltage - Enter Sandman
const int BPM = 130;
float notesArr[] {
  NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_E3, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A4, REST, NOTE_E3, NOTE_G3, NOTE_AS3, NOTE_A3, REST, NOTE_G3, REST, NOTE_E3
};
float durations[] {
  4, 8, 8, 4, 8, 8, 8, 8, 8, 8, 4, 4, 4, 8, 8, 4, 8, 8, 8, 2
};*/

//Define Note -> Voltage - Freebird
/*int notesArr[] = {
  NOTE_B4, NOTE_D4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G3, 0, NOTE_A4, NOTE_A4, NOTE_G3, NOTE_G3, NOTE_G3, NOTE_F3, NOTE_D3, 0, NOTE_G3, NOTE_B4, NOTE_D4, NOTE_B4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G3, NOTE_C3, NOTE_C3, NOTE_A4, NOTE_A4, NOTE_G3, NOTE_G3, NOTE_G3, NOTE_E3, NOTE_D3, 0, NOTE_G3, NOTE_B4, NOTE_D4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_G3, 0, NOTE_A4, NOTE_A4, NOTE_G3, NOTE_G3, NOTE_G3, NOTE_E4
};
float durations[] = {
  8, 8, 8, 8, 8, 8, 4, 1.5, 4, 8, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 8, 8, 8, 4, 1.5, 8, 8, 8, 8, 8, 8, 8, 8, 1, 8, 8, 8, 8, 8, 8, 8, 8, 4, 1.5, 4, 8, 8, 8, 8, 8, 8
};*/