#ifndef _HPGLH_
#define _HPGLH_

/*
   Pins:
   0  PD0  RX
   1  PD1  TX
   2  PD2        DIR_X
   3  PD3        STEP_X
   4  PD4        DIR_Y
   5  PD5        ERASER
   6  PD6        PEN
   7  PD7        STEP_Y

   8  PB0        HOME_X 
   9  PB1  PWM   HOME_Y
  10  PB2  PWM   HOME_X2
  11  PB3  PWM   STEP_X2
  12  PB4        MOTOR_CTRL
  13  PB5        STATUS_LED

  A0  PC0        PEN_DISTANCE
  A1  PC1        ERASER DISTANCE
  A2  PC2        BUTTON A
  A3  PC3        BUTTON B
  A4  PC4        BUTTON C
  A5  PC5        BUTTON D
*/

/*****************************************************
**                                                  **
** user config variables here, change as needed     **
**                                                  **
*****************************************************/

#define BAUDRATE          38400UL
#define USE_ARDUINO
#define HOME_SPEED_START  0xB000  // Start Speed for Homing-Procedure
#define HOME_SPEED_END    0xF900 // Maximum Speed for Homing Procedure
#define DEFAULT_SPEED     0xE000  // Regular Speed after Homing
#define XMAX              22000     // Maximum available steps on the x-axis
#define YMAX              10000     // Maximum available steps on the y-axis
#define XORG                 0     // Origin of X on medium
#define YORG                 0     // Origin of Y on medium
#define XSCALE               1.0   // Default scaling of incomming coordinates, x direction
#define YSCALE               1.0   // Default scaling of incomming coordinates, y direction

//#define USE_Z_PWM                  // Pen uses simple PWM for up/down/pressure
#define USE_Z_SERVO                // Pen uses a servo for up/down/pressure

//#define PWM_INVERT                 // Invert PWM Output
//#define SERVO_REVERSE              // Reverse direction of pen servo
//#define SERVO_INV_POL              // Revert polarity of servo-pwm
//#define ERASER_PWM_INVERT                 // Invert PWM Output
//#define ERASER_REVERSE              // Reverse direction of pen servo
//#define ERASER_INV_POL              // Revert polarity of servo-pwm
#define DEFAULT_PRESSURE   100


#define HOME_X_POS                 // Home switch for x-axis is GND when idle, + when trippe
//#define HOME_X_NEG                   // Home switch for x-axis is + when idle, GND when tripped

//#define HOME_X2_POS                 // Home switch for x-axis is GND when idle, + when tripped
//#define HOME_X2_NEG                 // Home switch for x-axis is + when idle, GND when tripped

#define HOME_Y_POS                 // Home switch for y-axis is GND when idle, + when tripped
//#define HOME_Y_NEG                 // Home switch for y-axis is + when idle, GND when tripped

//#define STEP_X_CLOCK_NEG           // Reverse polarity of stepper clock pulses
//#define STEP_X2_CLOCK_NEG           // Reverse polarity of stepper clock pulses
//#define STEP_X_DIR_NEG             // Reverse polarity of stepper direction control
//#define STEP_Y_CLOCK_NEG           // Reverse polarity of stepper clock pulses
//#define STEP_Y_DIR_NEG             // Reverse polarity of stepper direction control
#ifdef USE_Z_PWM
#define DEFAULT_UPDOWN_PAUSE    24 // default pause time before/after pen/eraser up/down command, using PWM
#endif
#ifdef USE_Z_SERVO
#define DEFAULT_UPDOWN_PAUSE    80 // default pause time before/after pen/eraser up/down command, using SERVO
#endif
#define DEFAULT_SPEED_RATE      16 // accelleration/decelleration rate
//#define USE_SENSOR_A                // apply correction of pen-servo using distance sensor on ADC0
//#define INVERT_SENSOR_A           // invert value os distance sensor on ADC0
//#define USE_SENSOR_B                // apply correction of eraser-servo using distance sensor on ADC1
//#define INVERT_SENSOR_B           // invert value os distance sensor on ADC1

#define PEN_MIN                    60 // minimum width of servo-pulse for pen
#define PEN_MAX                   80 // minimum width of servo-pulse for pen
#define ERASER_MIN                 50 // maximum width of servo-pulse for eraser
#define ERASER_MAX                130 // maximum width of servo-pulse for erase
#define SENSOR_A_MIN              320 // minimum sensor value for pen distance
#define SENSOR_A_MAX              630 // maximum sensor value for pen distance
#define SENSOR_A_DIV                5 // sensor divider, higher value = less influence
#define SENSOR_B_MIN              320 // minimum sensor value for eraser distance
#define SENSOR_B_MAX              630 // maximum sensor value for eraser distance
#define SENSOR_B_DIV                5 // sensor divider, higher value = less influence
#define SENSOR_OVSAMPLE            16 // number of times oversampling for sensor values

//#define USE_X2                        // use additional 2nd stepper for x-axis

//#define USE_CHALK                     // use chalk holder
#define CHALK_MIN               150    // minimum servo pulse to move chalk servo in one dir
#define CHALK_MAX               10   // maximum servo pulse to move chalk servo the other dir
#define CHALK_STOP              0   // servo pulse to make chalk servp stop
#define CHALK_EMPTY             565   // adc value signalling that the chalk is empty
#define CHALK_PUSH               0   // extra push after ir sensor signals contact, for PD
#define CHALK_PUSH_UP            42   // extra push after ir sensor signals lost contact, for PU
#define CHALK_UP                830   // adc value for chalk up
#define INVERT_CHALK_IR
//define INVERT_CHALK_SERVO
//define INVERT_CHALK_ADC
//define INVERT_CHALK_BUTTON
//define INVERT_CHALK_PUSH
//define INVERT_CHALK_UP
//define INVERT_CHALK_EMPTY
//#define CHECK_CHALK_WHILE_DRAWING 1

/*****************************************************
**                                                  **
** DO NOT CHANGE ANYTHING BELOW                     **
**                                                  **
*****************************************************/

// Pins as Arduino Numbers
#define    DIR_X_PIN     2
#define    STEP_X_PIN    3
#define    DIR_Y_PIN     4
#define    ERASER_PIN    5
#define    PEN_PIN       6
#define    STEP_Y_PIN    7
#define    HOME_X_PIN    8
#define    HOME_Y_PIN    9
#define    HOME_X2_PIN  10
#define    STEP_X2_PIN  11
#define    MOTOR_PIN    12

// Stepper Control Pins on PORTD
#define    DIR_X        (1<<2)
#define    STEP_X       (1<<3)
#define    DIR_Y        (1<<4)
#define    ERASER_SERVO (1<<5)
#define    PEN_SERVO    (1<<6)
#define    STEP_Y       (1<<7)

// Pen Control Pins on PORTB
#define    HOME_X       (1<<0)
#define    HOME_Y       (1<<1)
#define    HOME_X2      (1<<2)
#define    STEP_X2      (1<<3)
#define    MOTOR_IO     (1<<4)

#ifdef USE_CHALK
#define    CHALK_BUTTON (1<<4)
#define    CHALK_IR     (1<<5)
#endif

// Status-LED on PORTB
#define    STATUS_LED (1<<5)

union longbytes
{
  int32_t number;
  struct {
    uint8_t low;
    uint8_t mid1;
    uint8_t mid2;
    uint8_t high;
  } byte;
};

union ulongbytes
{
  uint32_t number;
  struct {
    uint8_t low;
    uint8_t mid1;
    uint8_t mid2;
    uint8_t high;
  } byte;
};

union intbytes
{
  int16_t number;
  struct {
    uint8_t low;
    uint8_t high;
  } byte;
};

union uintbytes
{
  uint16_t number;
  struct {
    uint8_t low;
    uint8_t high;
  } byte;
};

union charbits
{
  int8_t number;
  struct {
    unsigned a:1;
    unsigned b:1;
    unsigned c:1;
    unsigned d:1;
    unsigned e:1;
    unsigned f:1;
    unsigned g:1;
    unsigned h:1;
  } bit;
};

union ucharbits
{
  uint8_t number;
  struct {
    unsigned a:1;
    unsigned b:1;
    unsigned c:1;
    unsigned d:1;
    unsigned e:1;
    unsigned f:1;
    unsigned g:1;
    unsigned h:1;
  } bit;
};

void init_plotter(void);

#endif
