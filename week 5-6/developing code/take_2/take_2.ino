#include "remote_x.h"
#include "motor_run.h"
#include "mpu_header.h"
//######################################################-------------------------------------------declarations

//float Kp = -180, Ki = -100, Kd = -0.5;
float Kp = -90, Ki = -70, Kd = -1;//-0.6;
//float Kp = -27014.7125538481, Ki = -21183.1159387893, Kd = -5137.85556983136;
//float Kp = -27000, Ki = 0, Kd = -5137.85556983136;
//######################################################---------------------------------------------declarations

float set_point = -3.32;
float error = 0, error_d = 0,  error_i = 0 , prev_error = 0;
float Rmotor_torque ;
//######################################################---------------------------------------------SETUP

void setup() {
  Serial1.begin(115200);
  Serial.begin(250000);
  Wire.begin();
  Wire.setClock(400000UL);
  Adjust_rmt_Data();
  resetMPU();

  timer = micros();
}
//######################################################-------------------------------------------LOOP

void loop() {
  //timer1 = micros();
  read_remote();
  run_backwheel();
  run_front_servo();
  //Serial.println((micros()- timer1));

  float bot_inclination = dataFusion();
  //Serial.println((micros()- timer1));
  //######################################################---------------------------------------------PID controler here

  error = set_point - bot_inclination;
  if ((Ki * error_i <255 & Ki*error_i > -255))
  {
    error_i = error_i + error * Ts;
  }
  else if (Ki * error_i > 255)
  {
    error_i = float(int(255 / Ki));
  }
  else if ( Ki * error_i < -255)
  {
    error_i = float(int(-255 / Ki));
  }
  error_d = (error - prev_error) / Ts;
  prev_error = error;

  Rmotor_torque = (Kp * error) + (Ki * error_i) + (Kd * error_d);
  run_Rwheel(Rmotor_torque);

  //######################################################
  Serial.print(" ------- ");
  Serial.print(255);
  Serial.print(" ------- ");
  Serial.print(-255);
  Serial.print(" ------- ");
  Serial.print((Kp * error));
  Serial.print(" ------- ");
  Serial.print((Ki * error_i));
  Serial.print(" ------- ");
  Serial.print((Kd* error_d));
  Serial.print(" ------- ");
  Serial.println(error);
  //######################################################

  while (micros() - timer < Ts * 1000000); ////minimum time gap remains to burn is 1.2 ms so we can afford Fs = 200 Hz ~ Ts = 5 ms is optimum
  timer = micros();
}
