#include <ECE3.h>
#include <stdio.h>




const int left_nslp_pin = 31;   // nslp HIGH ==> awake & ready for PWM
const int right_nslp_pin = 11;  // nslp HIGH ==> awake & ready for PWM
const int left_dir_pin = 29;
const int right_dir_pin = 30;
const int left_pwm_pin = 40;
const int right_pwm_pin = 39;
const int RED_LED2 = 57;




const int allDarkthreshold = 16000;
const int allDarkthreshold_2=8000;
uint16_t sensorValues[8];
uint16_t MAX[8] = { 2300, 1728, 2036, 1434, 1577, 1643, 1884, 2300 };
// int weights[8] = { -15, -14, -12, -8,8,12, 14,15};  //remember to change the denum if you changed this!
int weights[8] = { -15, -14, -12, -8, 8, 12, 14, 15 };
int DENOM = 8;








uint16_t base_speed;  //base speed (pwm)
float K_P;
float K_D;




int err_val;
int prev_err;
int delta_err;
bool donut;
bool allDark;
int sumAllSensors;
int enc_count;
int rotation_enc_count;
bool turn_back;




const int second_turn = 550;
const int second_turn_2 = 800;
const int black_bar_1 = 1300;
const int black_bar_2 = 1550;
const int middle_gap = 1650;
const int last_gap = 1850;
const int last_bar = 1950;
const int last_bar_2 = 2100;




const int return_first_bar = 850;
const int return_first_bar_2 = 900;
const int return_zig_zag = 1050;
// const int return_zig_zag=1100;








const int return_second_bar = 1350;
const int return_second_bar_2 = 1550;








int readSensors();
int average();




void setup() {








 ECE3_Init();




 turn_back = false;
 base_speed = 20;  //FIX
 err_val = 0;
 K_P = 0.03;
 K_D = 0;
 sumAllSensors = 0;
 donut = false;
 allDark = false;
 rotation_enc_count = 341;




 pinMode(left_nslp_pin, OUTPUT);
 pinMode(left_dir_pin, OUTPUT);
 pinMode(left_pwm_pin, OUTPUT);
 pinMode(right_nslp_pin, OUTPUT);
 pinMode(right_dir_pin, OUTPUT);
 pinMode(right_pwm_pin, OUTPUT);








 pinMode(RED_LED2, OUTPUT);
















 digitalWrite(left_nslp_pin, HIGH);  //ready for be on the path
 digitalWrite(right_nslp_pin, HIGH);












 //going forward
 digitalWrite(left_dir_pin, LOW);
 digitalWrite(right_dir_pin, LOW);








 resetEncoderCount_left();
 resetEncoderCount_right();
}




















void loop() {
 prev_err = err_val;
 err_val = readSensors();
 delta_err = err_val - prev_err;
 int error_correction = (-K_P) * err_val + (-K_D) * delta_err;


 if (!turn_back) {
   int count = average();




   if (count >= second_turn && count <= second_turn_2) {
     K_P = 0.05;


     weights[0] = 0;
     weights[7] = 0;
     // weights[5]=0;
     // weights[4]=0;
     // analogWrite(left_pwm_pin, 0);
     //       analogWrite(right_pwm_pin, 0);
     //       delay(5000);




   } else if ((count > second_turn_2 && count < black_bar_1)) {




     weights[0] = -15;
     weights[7] = 15;
     K_P = 0.03;
   } else if (count >= black_bar_1 && count <= black_bar_2) {
     weights[0] = 0;
     weights[7] = 0;
     weights[1] = 0;
     weights[6] = 0;
     // analogWrite(left_pwm_pin, 0);
     //     analogWrite(right_pwm_pin, 0);
     //     delay(2000);
     K_D = 0.15;
     K_P = 0.05;








   } else if ((count > black_bar_2 && count < middle_gap)) {
     //  base_speed=20;
     weights[0] = -15;
     weights[7] = 15;
     weights[1] = -14;
     weights[6] = 14;
     K_D = 0.15;




   } else if (count >= middle_gap && count <= last_gap) {
     K_P = 0.03;
     K_D = 5 * K_P;
     //  analogWrite(left_pwm_pin,0);
     // analogWrite(right_pwm_pin,0);
     // delay(2000);
   }
   //  else if(count>1800&&count<=last_gap){
   //  weights[0]=0;
   //   weights[1]=0;
   //    weights[6]=0;
   //   weights[7]=0;
   //  }
   else if (count >= last_gap && count < last_bar) {
     K_P = 0.05;
     K_D = 0;  //0.35;
               // analogWrite(left_pwm_pin,0);
               // analogWrite(right_pwm_pin,0);
               // delay(2000);
     //  weights[0]=0;
     //   weights[1]=0;
     //    weights[6]=14;
     //   weights[7]=15;
   } else if (count >= last_bar && count <= last_bar_2) {
     K_P = 0.03;
     weights[0] = 0;
     weights[1] = 0;
     weights[6] = 0;
     weights[7] = 0;
     // analogWrite(right_pwm_pin,0);
     // analogWrite(left_pwm_pin,0);
     // delay(2000);








   } else if (count > last_bar_2) {
     weights[0] = -15;
     weights[1] = -14;
     weights[6] = 14;
     weights[7] = 15;
     // base_speed=30;
   }












   if (sumAllSensors > allDarkthreshold) {
     if (!allDark) {
       allDark = true;
     } else if (allDark == true) {
       resetEncoderCount_left();
       resetEncoderCount_right();
       while (average() < rotation_enc_count) {
         digitalWrite(left_dir_pin, HIGH);
         analogWrite(left_pwm_pin, base_speed);
         analogWrite(right_pwm_pin, base_speed);
       }
       digitalWrite(left_dir_pin, LOW);
       allDark = false;
       turn_back = true;
       resetEncoderCount_left();
       resetEncoderCount_right();
     }
   } else {


     analogWrite(left_pwm_pin, constrain(base_speed + error_correction, 0, 50));
     analogWrite(right_pwm_pin, constrain(base_speed - error_correction, 0, 50));
   }




   sumAllSensors = 0;
 } else if (turn_back == true) {
   int count = average();
   base_speed=30;
   if (count >= return_first_bar && count <= return_first_bar_2) {


     K_P = 0.03;
     weights[0] = 0;
     weights[1] = 0;
     weights[6] = 0;
     weights[7] = 0;
     allDark = false;
   }


   if (sumAllSensors > allDarkthreshold_2) {
     if (!allDark) {
       allDark = true;
     } else if (allDark == true) {


       analogWrite(left_pwm_pin, 0);
       analogWrite(right_pwm_pin, 0);
     }
   } else {


     analogWrite(left_pwm_pin, constrain(base_speed + error_correction,0,60));
     analogWrite(right_pwm_pin, constrain(base_speed -error_correction,0,60));
   }




   sumAllSensors = 0;
 }
}




















int sensorVals[8];
int readSensors() {
 int sum = 0;
 ECE3_read_IR(sensorValues);
 for (unsigned char i = 0; i < 8; i++) {
   sensorVals[i] = sensorValues[i];
   sumAllSensors += sensorVals[i];
   // IF THIS sensor is greater than the threshold, set the sensor value to 0;
   // if (sensorVals[i] < MIN[i])
   //   MIN[i] = sensorVals[i];




   // sensorVals[i] -= MIN[i];




   // if (sensorVals[i] > MAX[i])
   //   MAX[i] = sensorVals[i];




   // float ratio = 1000.0 / MAX[i];
   // sensorVals[i] = sensorVals[i] * ratio;
   sum += sensorVals[i] * weights[i];
 }




 return sum / DENOM;
}












int average()  //average pulse count
{
 int getL = getEncoderCount_left();
 int getR = getEncoderCount_right();
 //  Serial.print(getL);Serial.print("\t");Serial.println(getR);
 return ((getEncoderCount_left() + getEncoderCount_right()) / 2);
}



