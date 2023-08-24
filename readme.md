# AUTONOMOUS COMBAT ROBOT

# COURSE CODE: EEE 509 (ELECTRONIC SYSTEM DESIGN)

# LECTURER: DR BRENDA UBOCHI

# DEPT: ELECTRICAL AND ELECTRONICS ENGINEERING

# FACULTY: SCHOOL OF ENGINEERING AND ENGINEERING TECHNOLOGY

# SCHOOL: FEDERAL UNIVERSITY OF TECHNOLOGY, AKURE

<br>

Orton is an autonomous combat robot that can perform fighting action such as attacking, defending and basic movement operations. The functionality of the autonomous combat robot is reliant on two pivotal components: hardware and software.

## SOFTWARE ARCHITECTURE, PROGRAMMING, AND CONSTRUCTION

The program for the autonomous combat robot was developed using the Arduino C programming language. This specialized robot is designed for engaging in battles within confined arenas. Its self-sufficiency allows it to make independent decisions regarding movement, attacks, and defense.

The robot's design incorporates key elements: an IR sensor, an ultrasonic sensor, a servo motor, and a 4WD chassis. The IR sensor is responsible for detecting the arena's borders to prevent the robot from exiting. Configured as an input pin, the IR sensor's state is read using Arduino's "digitalRead" function. A value of 1 indicates no detection, while 0 indicates sensing the arena's border, facilitating subsequent actions based on this input.

Two ultrasonic sensors ensure comprehensive attack detection from various directions. One ultrasonic sensor is attached to the front, mounted on a servo motor for a 180-degree scanning capability. This sensor detects potential threats from the robot's forward movement. Another ultrasonic sensor at the rear detects attacks from behind even during forward motion. This setup guarantees the robot's ability to sense threats from nearly all angles. The ultrasonic sensors feature Vcc, Gnd, Trig, and Echo pins for power, ground, sound emission, and sound reception, respectively.

```c++

    // pinmode configuration for the ultrasonic sensor. ln 165-169
    ULTRASONIC_SENSOR_1.ECHO = 5;  // input pin
    ULTRASONIC_SENSOR_1.TRIG = 13; // output pin

    ULTRASONIC_SENSOR_2.ECHO = 2;
    ULTRASONIC_SENSOR_2.TRIG = 12; // output pin
```

The servo motor's role is to enhance the ultrasonic sensor's coverage area. The sensor alone cannot cover a wide range effectively, limiting its potential for threat detection. The servo motor's involvement enables a broader coverage area, detecting threats that might otherwise be out of range for the sensor alone.

The robot's core strategy involves defense followed by attack. It patiently waits for an opponent's threat before initiating an attack. Detection occurs via the ultrasonic sensor, with built-in computations verifying the legitimacy of the threat before launching an attack. After attacking, the robot ceases its assault after a predefined duration, adjustable in the program.

Defensive actions are possible when both front and rear sensors detect an attack simultaneously. The robot defends by rotating at a specific angle, counter to the attacking robot's trajectory. This angle, around 45 degrees, aims to keep the defending robot within the arena while potentially forcing the attacker out.

### ATTACK DETECTION ALGORITHM

A dedicated algorithm addresses the effective and accurate detection of attacks using ultrasonic sensors. This algorithm involves fundamental mathematical operations that yield a result surpassing a defined threshold.

Figure 0.2 showcases the algorithm's implementation in the program.

The algorithm functions by calculating the difference between the initial and current sensor readings. This difference is compared against a predefined attack sensor sensitivity value. If the difference meets the sensitivity threshold, an attack is considered detected.

$$ i = 0 \ge x \le 1059 $$
$$ c = 0 \ge x \le 1059 $$
$$ \Delta x = i - c $$
$$ \Delta x \ge dass $$

$$
    where: x = Real \, values from \, 0 - 1059 \\
    i = initial \, reading \, from \, sensor \\
    c = current \, reading \, from \, sensor \\
    dass = detect \, attack \, sensor \, sensitivity (0.3) \\
$$

<!-- ![algorithm flowchart](<img/Blank%20diagram%20(1).png>) -->

<img src="img/Blank diagram (1).png" width=50%>

```c++
// ln 455 - 495
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



```

The algorithm's operational flow is depicted in Figure 0.3.

To facilitate this algorithm, the initial reading is continuously updated with the previous current reading. Patterns emerging from the differences over multiple calculations are tracked. When a predefined pattern threshold is reached, the algorithm concludes whether an attack has been detected.

### ROBOTIC MOVEMENTS

The autonomous combat robot executes various movements: forward, backward, right turns, and left turns. These motions enable subsequent actions like attack, defense, and random movement. Movement prevents the robot from remaining dormant, aligning with its strategy of defense before initiating attacks.

```c++
// motor movements. ln 330 - 394
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


```

### ROBOT FUNCTIONALITIES

The robot undertakes multiple functionalities both independently and dependently. The program utilizes the Singular Responsibility System Design Pattern for modularity and ease of unit testing. Some functionalities include:

1. Check Area Boundary (check_area_boundary. ln 402): Prevents the robot from exiting the arena by verifying the boundary. The IR sensor's response to this check is logged in a scanning log structure.

2. Check Area Boundary Operation (check_area_boundary_operation. ln 416): Executes area boundary checks based on IR sensor responses. It takes actions according to the boundary check results.

3. Detect Attack (detect_attack. ln 455): Identifies potential attacks and immediately reports their status using the previously discussed algorithm.

4. Check Attack From Sensor (check_attack_from_sensor. ln 497): Acts as an intermediary between the detect attack function and specific sensors. It determines which sensor senses the attack and informs decisions like motor direction.

5. Activate Scan Operation (activate_scan_operation. ln 562): Serves as the program's central function, initiating other functions. Scanning the environment is the robot's primary task before any decision-making process.

6. Activate Attack (activate_attack. ln 604): Triggers an attack involving robot motion activation and attack mechanism initiation.

7. Deactivate Attack (deactivate_attack. ln 658): Halts ongoing attacks, enabling continued monitoring for incoming threats. Motion ceases, and the attack mechanism is deactivated.
