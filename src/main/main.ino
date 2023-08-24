/*
    +------------------------------------------------------------------+
    |    TITLE: COMBAT ROBOT (ORTON)                                   |
    |    COURSE CODE: EEE 509 | GROUP 6                                |
    |    DATE: 15TH AUG, 2023                                          |
    +------------------------------------------------------------------+
    +------------------------------------------------------------------+
    | ** SUMMARY **                                                    |
    | The combat robot is a specialized designed to wrestle            |
    | other robot in a confined area or arena. The Robot is designed   |
    | to operated by itself, that is, I can make decisions such as     |
    | when to move, attack, roam or defend.                            |
    |                                                                  |
    | ** MAJOR COMPONENTS/SENSORS **                                   |
    |  1. Ultrasonic sensor,                                           |
    |  2. IR sensor                                                    |
    |                                                                  |
    | ** MOTION MOVEMENT **                                            |
    | 1. Front -> 1                                                    |
    | 2. Left -> 2                                                     |
    | 3. Right -> 3                                                    |
    | 4. Rear -> 4                                                     |
    |                                                                  |
    | ** ROBOT ACTIONS **                                              |
    | 1. Scanning Process:                                             |
    | SENSOR RESPONSIBLE: IR Sensor                                    |
    | FUNCTION CALL: check_arena_boundary                              |
    | DESCRIPTION: At the time the robot is on, it checks if it is in  |
    | close on the edge of the arena so it knows if it can possibly    |
    | move forward or backwards. The first scan would be done by the   |
    | IR sensor, after the check, it would be reported to Scanning log |
    |                                                                  |
    | 2. Detect Attack:
    | SENSOR RESPONSIBLE: Ultrasonic sensor
    | FUNCTION CALL: detect_attack
    | DESCRIPTION: The robot would detect attack using ultrasonic      |
    | sensor. Four ultrasonic sensor would be used, and they would be  |
    | place at the front, rear, and sides (left and right). All the    |
    | sensors would detect attack them same way, and the result would  |
    | be reported to a Detect Attack log. The Algorithm behind detect  |
    | attack is simply identifying a different pattern in the          |
    | ultrasonic sensor reading. When the sensors reads, the values are|
    | usually not different but if an object approach, the values      |
    | begins to change rapidly and it is this rapid movement that can  |
    | tell when an object is coming. An alternate solution would be to |
    | use the PIR sensor which detects motion, but ultrasonic sensor   |
    | was choosen because of the flexibility to perform certain action |
    | as certain range.                                                |
    |
    | 3. Dispatch Action:
    | SENSOR RESPONSIBLE: (can work with any sensor)
    | FUNCTION CALL: dispatch_action
    | DESCRIPTION: Actions can be dispatched based on the current
    | reading from a sensor. The sensor logged value is passed in to
    | the function call and the actions to take as well. The logged
    | value and action should be indexed the same value
    +------------------------------------------------------------------+
*/

// libraries
#include <Servo.h>

typedef struct SCANNING_LOG_STRUCT SCANNING_LOG_DEF;
typedef struct ATTACK_STATUS_LOG_STRUCT ATTACK_STATUS_LOG_DEF;
typedef struct ULTRASONIC_SENSOR_STRUCT ULTRASONIC_SENSOR_DEF;

/* structs */
struct ULTRASONIC_SENSOR_STRUCT
{
    int TRIG;
    int ECHO;
};
struct SCANNING_LOG_STRUCT
{
    int IR_SENSOR_FRONT_REPORT;
    int IR_SENSOR_REAR_REPORT;
};
struct ATTACK_STATUS_LOG_STRUCT
{
    int ULTRASONIC_SENSOR_1_STATUS;
    int ULTRASONIC_SENSOR_2_STATUS;
};

/* DEFINING PINS
   NOTE: If the input or output components are increased, they
   must also be an increase in the number_of_input_pins or number_of_output_pins
   (You can search for the variable)
*/

// max number of pins to connect 4
#define STARTING_INPUT_PIN 2
#define NUMBER_OF_INPUT_PINS 3 // 4 - 1

// max number of pins to connect 8
#define STARTING_OUTPUT_PIN 6
#define NUMBER_OF_OUTPUT_PINS 7 // 8 - 1

#define ULTRASONIC_SENSOR_STARTING_PIN 5

/* INPUT PINS */

// defined ULTRASONIC_TRIG in setup
#define IR_SENSOR_FRONT 3
#define IR_SENSOR_REAR 4

/* OUPUT PINS*/

// Input 1 & 2 controls Right, Input 3 & 4 controls Left
#define MOTOR_ENABLE_1_PIN 6
#define MOTOR_ENABLE_2_PIN 7
#define MOTOR_INPUT_1_PIN 8
#define MOTOR_INPUT_2_PIN 9
#define MOTOR_INPUT_3_PIN 10
#define MOTOR_INPUT_4_PIN 11

// defined pin for testing
// #define MOTION_INDICATOR 7
// #define NO_DETECTION_INDICATOR 11

/* VARIABLES */

#define ULTRASONIC_SENSOR_READ_TIME 10 // milliseconds
#define IR_SENSOR_NUMBER 2
#define ULTRASONIC_SENSOR_NUMBER 4
#define DETECT_ATTACK_SENSOR_SENSITIVITY 0.3 // sensitivity reduces by increasing value
#define DETECT_ATTACK_SENSOR_SENSITIVITY_STEP 2
#define DETECT_ATTACK_SENSOR_READ_DELAY_TIME 100 //! in milliseconds
#define DETECT_NO_ATTACK_ENDURANCE_LEVEL 7
#define DETECT_ATTACK_ENDURANCE_LEVEL 9
#define ATTACK_COVERAGE_AREA 30

// Delay or Duration variables -> in milliseconds
#define DELAY_TIME_TO_STOP_ATTACK 3000
#define DELAY_TIME_INTERVAL_FOR_SERVO_ROTATION 3000
#define DELAY_TIME_BEFORE_STOPPING_TURN 2000
#define ROBOT_ATTACK_DAMAGE_PERIOD 2500

Servo servo_1;
Servo servo_2;

/* struct global variables */
SCANNING_LOG_DEF SCANNING_LOG;
ATTACK_STATUS_LOG_DEF ATTACK_STATUS_LOG;
ULTRASONIC_SENSOR_DEF ULTRASONIC_SENSOR_1;
ULTRASONIC_SENSOR_DEF ULTRASONIC_SENSOR_2;

ULTRASONIC_SENSOR_DEF CONNECTED_ULTRASONIC_SENSOR[] = {
    ULTRASONIC_SENSOR_1,
    ULTRASONIC_SENSOR_2,
};

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting Robot program...");

    // servo motor setup
    servo_1.attach(A0);
    servo_2.attach(A1);

    // motor setup

    /* Set ultrasonic sensor pins. The reason the having the setup as struct is for easy call for both the echo and trig pins. Due to the number of ultrasonic sensor use, it would be redundant to keep calling the trip and echo pins for each ultrasonic sensor over and over again.
     */
    ULTRASONIC_SENSOR_1.ECHO = 5;  // input pin
    ULTRASONIC_SENSOR_1.TRIG = 13; // output pin

    ULTRASONIC_SENSOR_2.ECHO = 2;
    ULTRASONIC_SENSOR_2.TRIG = 12; // output pin

    /* SETTING PIN MODES */

    // input pins
    const int starting_input_pin = (int)STARTING_INPUT_PIN;
    const int number_of_input_pins = (int)NUMBER_OF_INPUT_PINS;
    const int starting_output_pin = (int)STARTING_OUTPUT_PIN;
    const int number_of_output_pins = (int)NUMBER_OF_OUTPUT_PINS;

    for (int i = starting_input_pin; i <= (starting_input_pin + number_of_input_pins); i++)
        pinMode(i, INPUT);

    // output pins
    for (int o = starting_output_pin; o <= (starting_output_pin + number_of_output_pins); o++)
        pinMode(o, OUTPUT);

    digitalWrite(MOTOR_ENABLE_1_PIN, LOW); // controls the right motor
    digitalWrite(MOTOR_ENABLE_2_PIN, LOW); // controls the left motor
}

void loop()
{
    // TEST 1: detect attack
    // int res = detect_attack(ULTRASONIC_SENSOR_1);
    // if (res)
    //     Serial.println("Attack is detected");
    // else
    //     Serial.println("Attack is not detected");

    // TEST 2
    // int res = detect_attack(ULTRASONIC_SENSOR_1);
    // if (res)
    // {
    //     digitalWrite(NO_DETECTION_INDICATOR, LOW);
    //     Serial.println("Attack has been detected");
    //     activate_attack(1);
    // }
    // else
    // {
    //     Serial.println("Attack was not been detected");
    //     digitalWrite(NO_DETECTION_INDICATOR, HIGH);
    //     deactivate_attack();
    // };

    // TEST 3: Testing scanning operation
    // clear_logs();
    // activate_scan_operation();

    // TEST 4: Servo Motor Movement
    // servo_1.write(0);
    // delay(2000);
    // servo_1.write(90);
    // delay(2000);
    // servo_1.write(180);
    // delay(2000);
    // servo_1.write(180);
    // delay(2000);
    // servo_1.write(90);
    // delay(2000);
    // servo_1.write(0);
    // delay(2000);

    // TEST 5: MOTOR MOVEMENT
    // forward_motion();
    // delay(4000);
    // backward_motion();
    // delay(4000);
    // right_motion();
    // delay(4000);

    // TEST 6: IR SENSOR TEST
    // check_area_boundary_operation();
    // delay(4000);

    /* MAIN PROGRAM */
    // clear log
    // clear_logs();
    // // 1. Check area border
    // if (check_area_boundary_operation())
    // {
    //     activate_scan_operation();
    // }

    // check_area_boundary_operation();
    activate_servo_motion_0();
    delay(3000);
    activate_servo_motion_90();
    right_motion();
    delay(5000);
    check_area_boundary_operation();
    delay(3000);
    right_motion();
    delay(3000);
    activate_servo_motion_180();
}

/*==========================================+
 *                                          *
 *           UTILITY FUNCTIONS              *
 *                                          *
 *==========================================*/

int read_ultrasonic_sensor(ULTRASONIC_SENSOR_DEF ultrasonic_sensor)
{
    /* read values from the ultrasonic sensor */
    digitalWrite(ultrasonic_sensor.TRIG, LOW); // clear the trip
    delayMicroseconds(2);                      // random delay
    digitalWrite(ultrasonic_sensor.TRIG, HIGH);
    delayMicroseconds(ULTRASONIC_SENSOR_READ_TIME);
    digitalWrite(ultrasonic_sensor.TRIG, LOW);

    // Read the echoPin
    long int duration = pulseIn(ultrasonic_sensor.ECHO, HIGH);
    return (int)(duration * 0.034) / 2; // in cm
}

void clear_logs()
{
    Serial.println("Clear Logs...");
    // attack log
    ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_1_STATUS = 0;
    ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_2_STATUS = 0;

    // scanning log
    SCANNING_LOG.IR_SENSOR_FRONT_REPORT = 0;
    SCANNING_LOG.IR_SENSOR_REAR_REPORT = 0;
}

/* =============================+

    ROBOT BASIC MOVEMENTS

+  ============================*/

// Servo movement
void deactivate_servo_motion()
{
    servo_1.detach();
    servo_2.detach();
}
void activate_servo_motion_0()
{
    int angle = 45;
    servo_1.write(angle);
    servo_2.write(angle);
}
void activate_servo_motion_90()
{
    int angle = 82;
    servo_1.write(angle);
    servo_2.write(angle);
}
void activate_servo_motion_180()
{
    int angle = 115;
    servo_1.write(angle);
    servo_2.write(angle);
}

// motor movements
void stop_robot_motion()
{
    digitalWrite(MOTOR_ENABLE_1_PIN, LOW);
    digitalWrite(MOTOR_ENABLE_2_PIN, LOW);

    digitalWrite(MOTOR_INPUT_1_PIN, LOW);
    digitalWrite(MOTOR_INPUT_2_PIN, LOW);
    digitalWrite(MOTOR_INPUT_3_PIN, LOW);
    digitalWrite(MOTOR_INPUT_4_PIN, LOW);

    Serial.println("Stopped motion");
}
void forward_motion()
{
    digitalWrite(MOTOR_ENABLE_1_PIN, HIGH); // controls the right motor
    digitalWrite(MOTOR_ENABLE_2_PIN, HIGH); // controls the left motor

    digitalWrite(MOTOR_INPUT_1_PIN, HIGH);
    digitalWrite(MOTOR_INPUT_2_PIN, LOW);
    digitalWrite(MOTOR_INPUT_3_PIN, HIGH);
    digitalWrite(MOTOR_INPUT_4_PIN, LOW);

    check_area_boundary_operation();
    Serial.println("Moving Forward");
}
void backward_motion()
{
    // controls the left motor

    digitalWrite(MOTOR_ENABLE_1_PIN, HIGH); // controls the right motor
    digitalWrite(MOTOR_ENABLE_2_PIN, HIGH);

    digitalWrite(MOTOR_INPUT_1_PIN, LOW);
    digitalWrite(MOTOR_INPUT_2_PIN, HIGH);
    digitalWrite(MOTOR_INPUT_3_PIN, LOW);
    digitalWrite(MOTOR_INPUT_4_PIN, HIGH);

    check_area_boundary_operation();
    Serial.println("Moving Backward");
}
void right_motion()
{
    digitalWrite(MOTOR_ENABLE_1_PIN, HIGH); // controls the right motor
    digitalWrite(MOTOR_ENABLE_2_PIN, HIGH); // controls the left motor

    digitalWrite(MOTOR_INPUT_1_PIN, LOW);
    digitalWrite(MOTOR_INPUT_2_PIN, LOW);
    digitalWrite(MOTOR_INPUT_3_PIN, HIGH);
    digitalWrite(MOTOR_INPUT_4_PIN, LOW);

    check_area_boundary_operation();
    Serial.println("Moving Right");
}
void left_motion()
{
    digitalWrite(MOTOR_ENABLE_1_PIN, HIGH); // controls the right motor
    digitalWrite(MOTOR_ENABLE_2_PIN, LOW);  // controls the left motor

    digitalWrite(MOTOR_INPUT_1_PIN, HIGH);
    digitalWrite(MOTOR_INPUT_2_PIN, LOW);
    digitalWrite(MOTOR_INPUT_3_PIN, LOW);
    digitalWrite(MOTOR_INPUT_4_PIN, LOW);

    check_area_boundary_operation();
}

/* ==============================+
 *                               +
 *      ROBOT ACTIONS            +
 *                               +
+ ===============================*/

void check_area_boundary()
{
    /* check arena boundary to ensure that its not at the end of the line/arena */
    int read_ir_sensor_front = digitalRead(IR_SENSOR_FRONT);
    int read_ir_sensor_rear = digitalRead(IR_SENSOR_REAR);

    // log output
    Serial.println("READ IR FRONT SENSOR: " + String(read_ir_sensor_front));
    Serial.println("READ IR REAR SENSOR: " + String(read_ir_sensor_rear));

    SCANNING_LOG.IR_SENSOR_FRONT_REPORT = read_ir_sensor_front;
    SCANNING_LOG.IR_SENSOR_REAR_REPORT = read_ir_sensor_rear;
}

int check_area_boundary_operation()
{
    /* The robots continues to check area boundary until it senses that both ir sensor are not true */
    while (true)
    {
        check_area_boundary();
        if (SCANNING_LOG.IR_SENSOR_FRONT_REPORT)
        {
            // stop_robot_motion();
            backward_motion(); // moves backward
            delay(900);
            break;
        }
        else if (SCANNING_LOG.IR_SENSOR_REAR_REPORT)
        {
            // stop_robot_motion();
            forward_motion(); // moves forward
            delay(900);
            break;
        }
        else
        {
            Serial.println("Exiting checking area...");
            stop_robot_motion();
            return 1;
        }
    }
}

void check_area_boundary_in_motion()
{
    check_area_boundary();

    if (SCANNING_LOG.IR_SENSOR_FRONT_REPORT || SCANNING_LOG.IR_SENSOR_REAR_REPORT)
    {
        stop_robot_motion();
    }
}

int detect_attack(ULTRASONIC_SENSOR_DEF ultrasonic_sensor)
{
    int detect_no_attack_endurance_level = DETECT_NO_ATTACK_ENDURANCE_LEVEL;
    int detect_attack_endurance_level = DETECT_ATTACK_ENDURANCE_LEVEL;

    int detect_no_attack_tracker = 0;
    int detect_attack_tracker = 0;

    /* Read from sensor */
    int init_sensor_reading = read_ultrasonic_sensor(ultrasonic_sensor);
    /* check the values from the sensor */
    while (true)
    {
        /* read current sensor value */
        int current_sensor_reading = read_ultrasonic_sensor(ultrasonic_sensor);

        Serial.println("\n-----------------------------------------------------------------\n");
        Serial.println("Initial Sensor Reading: " + String(init_sensor_reading));
        Serial.println("Current Sensor Reading: " + String(current_sensor_reading));
        Serial.println("\n-----------------------------------------------------------------\n");

        /* checks if difference between is more than sensitivity */
        if ((init_sensor_reading - current_sensor_reading) > DETECT_ATTACK_SENSOR_SENSITIVITY || current_sensor_reading <= ATTACK_COVERAGE_AREA)
            detect_attack_tracker++;
        else
            detect_no_attack_tracker++;

        // check boundary condition
        check_area_boundary_operation();

        /* reset the inital sensor reading and wait so reading can skip by certain steps */
        init_sensor_reading = current_sensor_reading;
        delay((int)DETECT_ATTACK_SENSOR_READ_DELAY_TIME * DETECT_ATTACK_SENSOR_SENSITIVITY_STEP);

        /* check proximity consistency */
        if (detect_no_attack_tracker >= detect_no_attack_endurance_level)
            return 0;
        if (detect_attack_tracker >= detect_attack_endurance_level)
            return 1;
    }
}

void check_attack_from_sensor(int query)
{
    /* checks the status of attack from any number of sensor depending of the query type. */

    int response;

    int sensor_starting_pin = (int)ULTRASONIC_SENSOR_STARTING_PIN;
    int sensor_number = ULTRASONIC_SENSOR_NUMBER;

    ULTRASONIC_SENSOR_DEF connected_sensor[] = {
        ULTRASONIC_SENSOR_1,
        ULTRASONIC_SENSOR_2,
    };

    switch (query)
    {
    case 1:
        response = detect_attack(connected_sensor[0]); // first sensor ie front
        ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_1_STATUS = response;
        break;
    case 2:
        response = detect_attack(connected_sensor[1]); // second sensor ie left
        ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_2_STATUS = response;
        break;
    case 3:
        Serial.println("Detect Attack for ultra sensor 1");
        int ultra_sensor_1_response = detect_attack(connected_sensor[0]); // first sensor ie front
        ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_1_STATUS = ultra_sensor_1_response;

        Serial.println("Detect Attack for ultra sensor 2");
        int ultra_sensor_2_response = detect_attack(connected_sensor[1]); // second sensor ie left
        ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_2_STATUS = ultra_sensor_2_response;
        break;
    }
}

void check_attack_from_sensor_handler()
{
    Serial.println("Check attack from sensor handler working");

    if (ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_1_STATUS && ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_2_STATUS)
    {
        Serial.println("Detected Attack from sensor 1");
        right_motion();
        check_area_boundary_operation();
        delay(DELAY_TIME_BEFORE_STOPPING_TURN);
    }
    if (ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_1_STATUS)
    {
        activate_attack(1);
        check_area_boundary_in_motion();
    }
    else if (ATTACK_STATUS_LOG.ULTRASONIC_SENSOR_2_STATUS)
    {
        activate_attack(2);
        check_area_boundary_in_motion();
    }
    else
    {
        deactivate_attack();
    }
}

/* ACTIVATION FUNCTIONS */

void activate_scan_operation()
{
    /* The scan operation involves the servo motor moving around for a scan and the ultrasonic sensor detect for an attack. This function also handles deactivating the attack with the check_attack_from_sensor function. */

    Serial.println("Activating Scan Operation");

    int detect_attack_response;
    int scan_counter = 0;
    int scan_cycle = 60;

    while (true)
    {
        // move from 0 to 90 then detect attack
        activate_servo_motion_0();
        check_attack_from_sensor(1); // 9999 is nonsense
        check_attack_from_sensor_handler();

        delay(DELAY_TIME_INTERVAL_FOR_SERVO_ROTATION);

        activate_servo_motion_90();
        check_attack_from_sensor(1); // 9999 is nonsense
        check_attack_from_sensor_handler();

        delay(DELAY_TIME_INTERVAL_FOR_SERVO_ROTATION);

        // move from 90 to 180 then detect attack
        activate_servo_motion_180();
        check_attack_from_sensor(1);
        check_attack_from_sensor_handler();

        delay(DELAY_TIME_INTERVAL_FOR_SERVO_ROTATION);

        if (scan_counter >= scan_cycle)
        {
            // do something stupid
            Serial.println(" \n Complete scan counter");
            break;
        }
        scan_counter++;
    }
}

void activate_attack(int attack_direction)
{
    /* call the attack actions, that is, to move towards the direction based on the sensor and
    turn on attack mechanism */
    int activate_robot_attack_counter = 0;
    int max_number_of_attack_cycle = 29;

    Serial.println("Activating attack");

    while (true)
    {
        // continue attack until it supposedly finish attack

        check_area_boundary_in_motion();

        activate_robot_motion(attack_direction); // move forward
        activate_attack_mechanism();

        check_area_boundary_in_motion();

        if (activate_robot_attack_counter >= max_number_of_attack_cycle)
        {
            check_area_boundary_in_motion();
            delay(ROBOT_ATTACK_DAMAGE_PERIOD);
            deactivate_attack();
            break;
        }
        activate_robot_attack_counter++;
    }
}

void activate_attack_mechanism(void)
{
    Serial.println("Starting attack mechanism");
}

void activate_robot_motion(int sensor_position)
{
    /* Read the Doc at the top of file to understand the mapping of direction
        to the number */
    switch (sensor_position)
    {
    case 1: // front sensor
        forward_motion();
        break;
    case 2: // rear sensor
        backward_motion();
        break;
    case 3: // two sensor detect at same time
        right_motion();
        break;
    }
}

void deactivate_attack()
{
    stop_robot_motion();
}
