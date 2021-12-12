/* In the name of Allah */

// Define some IO pins
#define IN1 7
#define IN2 6
#define EN  5
#define SENSOR 2

// Sensor variables
uint32_t pre_time = 0;
float velocity = 0.0;

// PI controller variables
float Kp = 2.0;
float Ki = 0.3;
int error = 0;
float P_term = 0.0;
float I_term = 0.0;
uint32_t int_error = 0;
float controller_out = 0.0;

void setup() {
  // Srart a serial connection
  Serial.begin(9600);
  Serial.println("Hello!");

  // Define control pins as output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);

  // Define encoder pin as input (interrupt enabled)
  pinMode(SENSOR, INPUT);
  attachInterrupt(digitalPinToInterrupt(SENSOR), edge_found, RISING);

  // Assign value to control pins
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);  
}

void loop() {
  // Reading set point using ADC
  uint16_t raw_input = 0;
  raw_input = analogRead(A0);
  int set_point = map(raw_input, 0, 1023, 0, 250); 

  // Print set point and current velocity
  Serial.print(set_point);
  Serial.print(" - ");
  Serial.println(velocity);

  // Error calculation
  error = set_point - velocity;

  // Proportional term calculation
  P_term = error * Kp;
  // Integrator term calculation
  int_error = int_error + error; 
  I_term = int_error * Ki;

  // Output calculaion
  controller_out = P_term + I_term;

  // Saturation
  if(controller_out >= 255)
    controller_out = 255;
  if(controller_out <= 0)
    controller_out = 0;

  // Assign output value to PWM and motor
  analogWrite(EN, (int)controller_out);

  // Timing
  delay(1);
}

// Interrupt routin function
void edge_found(void){
  uint32_t current_time = 0;
  uint32_t duration = 0;
  current_time = micros();
  duration = current_time - pre_time;
  pre_time = current_time;
  velocity = 60000000/((float)(24*duration)); // RPM = 60 / (PulsPerRevoloution * PulseDuration);
}
