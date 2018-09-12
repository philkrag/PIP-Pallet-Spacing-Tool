/*
// /////////////////////////////////////////////////////////////////////// COPYRIGHT NOTICE
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with Pip-Project.  If not, see <http://www.gnu.org/licenses/>.
// /////////////////////////////////////////////////////////////////////// COPYRIGHT NOTICE
// /////////////////////////////////////////////////////////////////////// VERSION CONTROL
// PAGE CREATED BY: Phillip Kraguljac
// PAGE CREATED DATE: 2018-09-08
// DATE       || NAME           || MODIFICATION
// 2018-09-08   || Phillip Kraguljac    || Created.
// /////////////////////////////////////////////////////////////////////// VERSION CONTROL
*/

#define US_Trigger 5                                // Ultra-sonic trigger sensor output pin.
#define US_Echo 6                                   // Ultra-sonic echo sensor input pin.
#define RLY_Inhibit 2                               // Voltage free relay drive stop output pin.
#define RLY_Visual_Error 7                          // Voltage free relay warning light output pin. - CANNOT BE PWM PIN
#define BTN_Reset 4                                 // Voltage free reset input pin.

int Timer_Counter = 0;                              // Used for mode timings.
int Button_State = 0;                               // The default reset button state.
int Error_Tolerance_Counter = 0;                    // Used for allowing errors.
String Operation_State = "Waiting_Pallet";          // The mode indexer.
bool Pallet_Present = 0;                            // Memory for pallet presence.
bool Error_Tolerance_Active = false;

// USER DEFINED VARIABLES
int Timer_Trigger = 85;                             // The expected time in which a pallet will pass + required space. (default 80 - no space)
int Timer_Inhibit = 10;                             // The amount of time in which the drive will be stoppped.
int Timer_ReTrigger = 4;                            // The amount of time before the system can be reset.
int Error_Tolerance_Trigger = 5;                   // The amount of allowable errors (vibrations etc).

void setup() {

Serial.begin (9600);                                // Set up serial connection.
pinMode(US_Trigger, OUTPUT);                        // Declare pin mode(s).
pinMode(US_Echo, INPUT);
pinMode(RLY_Inhibit, OUTPUT);
pinMode(RLY_Visual_Error, OUTPUT);
pinMode(BTN_Reset, INPUT);

digitalWrite(RLY_Inhibit,HIGH);                     // Set default starting state for drive.
digitalWrite(RLY_Visual_Error,HIGH);                // Set default starting state for warning light.

}

void loop() {

Button_State = digitalRead(BTN_Reset);              // Read reset button and load to memory.

// ///////////////////////////////// ULTRA SONIC SENSOR OPERATION - BELOW

// Coding for ultra sonic sensor obtained from internet - unsure of original author.
// Will update attritibutes upon discovering original author.

long duration, distance;
digitalWrite(US_Trigger, LOW);
delayMicroseconds(2);
digitalWrite(US_Trigger, HIGH);
delayMicroseconds(10);
digitalWrite(US_Trigger, LOW);
duration = pulseIn(US_Echo, HIGH);
distance = (duration/2) / 29.1;
if (distance < 4) {
}
else {
}

// ///////////////////////////////// ULTRA SONIC SENSOR OPERATION - ABOVE

// ///////////////////////////////// DETERMINE PALLET PRESENCE - ABOVE

if (distance >= 40 || distance <= 0){             // Check for pallet presence.
Pallet_Present = 0;                               // Load pallet presence into memory.
Serial.print("P0 ||");                            // Display pallet presence in serial terminal.
}else{
Pallet_Present = 1;                               // Load pallet presence into memory.
Serial.print("P1 ||");                            // Display pallet presence in serial terminal.
}

// ///////////////////////////////// DETERMINE PALLET PRESENCE - BELOW

// ///////////////////////////////// MAIN OPERATION - BELOW

// [MODE] => WAITING FOR PALLET
if(Operation_State == "Waiting_Pallet"){
Serial.print("WP ||");                            // Provide update in serial terminal.

digitalWrite(RLY_Inhibit,HIGH);
digitalWrite(RLY_Visual_Error,HIGH);
  
if (Pallet_Present){
Operation_State = "Monitoring_Pallet";
Error_Tolerance_Active = true;
}
}


// [MODE] => MONITORING PALLET
if(Operation_State == "Monitoring_Pallet"){
Serial.print("MP ||");                            // Provide update in serial terminal.
Serial.print(Timer_Counter);

digitalWrite(RLY_Inhibit,HIGH);                   // Update outputs.
digitalWrite(RLY_Visual_Error,HIGH);              // Update outputs.

if(Error_Tolerance_Counter>Error_Tolerance_Trigger){
Error_Tolerance_Active = false;                   // Switches OFF any error tolerance.
}

if (!Pallet_Present){                             // When no pallet is detected undertake check.

if(Error_Tolerance_Active){
Error_Tolerance_Counter = Error_Tolerance_Counter + 1;
Serial.print("|| ET ACTIVE");
}

else{
Timer_Counter = 0;
Error_Tolerance_Counter = 0;
Operation_State = "Waiting_Pallet";
}
}

if(Timer_Counter>Timer_Trigger){
Operation_State = "Inhibit_Drive";
}

Timer_Counter = Timer_Counter + 1;  
}


// [MODE] => STOP DOWNSTREAM DRIVE
if(Operation_State == "Inhibit_Drive"){
Serial.print("ID ||");                            // Provide update in serial terminal.
Serial.print(Timer_Counter);                      // Provide update in serial terminal.
  
digitalWrite(RLY_Inhibit,LOW);                    // Update outputs.
digitalWrite(RLY_Visual_Error,LOW);               // Update outputs.

if(Timer_Counter>(Timer_Trigger+Timer_Inhibit)){  // When sensor expects to see nothing.
Operation_State = "ReTrigger_Delay";              // Change mode.
}
  Timer_Counter = Timer_Counter + 1;              // Increment counter.
}


// [MODE] => PREVENT RE-TRIGGER
if(Operation_State == "ReTrigger_Delay"){
Serial.print("RTD ||");                           // Provide update in serial terminal.
Serial.print(Timer_Counter);                      // Provide update in serial terminal.
  
digitalWrite(RLY_Inhibit,HIGH);                   // Update outputs.
digitalWrite(RLY_Visual_Error,LOW);               // Update outputs.


if(Timer_Counter>(Timer_Trigger+Timer_Inhibit+Timer_ReTrigger)){        // When total time elapses.
Timer_Counter = 0; // Reset Counter
Operation_State = "Waiting_Reset";                // Change mode.
}
  Timer_Counter = Timer_Counter + 1;              // Increment counter.
}


// [MODE] => NEED TO RESET SYSTEM
if(Operation_State == "Waiting_Reset"){
  
Serial.print("WR ||");                            // Provide update in serial terminal.
  
digitalWrite(RLY_Inhibit,HIGH);                   // Update outputs.
digitalWrite(RLY_Visual_Error,LOW);               // Update outputs.

if(!Pallet_Present||Button_State){
Operation_State = "Waiting_Pallet";
}
}

if (Button_State == HIGH) {Serial.print("high ||");} else {Serial.print("low ||");}

Serial.println(":");

delay(50);
}
