#ifndef _MOTISRH_
#define _MOTISRH_

// "|= DIR_n_CW"  = towards min
// "&= ~DIR_n_CW" = towards max

#define DO_STEP_X       0x01
#define DIR_X_CW        0x02
#define DO_STEP_Y       0x04
#define DIR_Y_CW        0x08
#define STEP_DONE       0x10
#define Z_UP            0x20
#define Z_DOWN          0x40
#define MOTOR_CTRL      0x80
#define ERASER_UP       0x01
#define ERASER_DOWN     0x02
#define ERASER_PRESS    0x04
#define Z_PRESS         0x08
#define VELOCITY_RATE   0x10
#define DO_STEP_X2      0x20
#define SET_VELOCITY    0x80

#define ACCELLERATE 0x01
#define DECELLERATE 0x02
#define SPEED_HOLD  0x03

#ifdef STEP_X_CLOCK_NEG
  #define STEP_X_OFF PORTD |=  STEP_X
  #define STEP_X_ON  PORTD &= ~STEP_X
#else    
  #define STEP_X_OFF PORTD &= ~STEP_X
 #define  STEP_X_ON  PORTD |=  STEP_X
#endif

#ifdef STEP_X2_CLOCK_NEG
  #define STEP_X2_OFF PORTB |=  STEP_X2
  #define STEP_X2_ON  PORTB &= ~STEP_X2
#else    
  #define STEP_X2_OFF PORTB &= ~STEP_X2
 #define  STEP_X2_ON  PORTB |=  STEP_X2
#endif

#ifdef STEP_Y_CLOCK_NEG
  #define STEP_Y_OFF PORTD |=  STEP_Y
  #define STEP_Y_ON  PORTD &= ~STEP_Y
#else    
  #define STEP_Y_OFF PORTD &= ~STEP_Y
  #define STEP_Y_ON  PORTD |=  STEP_Y
#endif

#ifdef STEP_X_DIR_NEG
  #define STEP_X_CW  PORTD &= ~DIR_X
  #define STEP_X_CCW PORTD |=  DIR_X
#else
  #define STEP_X_CW  PORTD |=  DIR_X
  #define STEP_X_CCW PORTD &= ~DIR_X
#endif  
  
#ifdef STEP_Y_DIR_NEG
  #define STEP_Y_CW  PORTD &= ~DIR_Y
  #define STEP_Y_CCW PORTD |=  DIR_Y
#else
  #define STEP_Y_CW  PORTD |=  DIR_Y
  #define STEP_Y_CCW PORTD &= ~DIR_Y 
#endif  

void set_motor(uint8_t);
void set_servo(uint8_t);
void pen_up(void);
void pen_down(void);
void set_eraser(uint8_t);
void eraser_up(void);
void eraser_down(void);
void init_mot_isr(void);
void stop_mot_isr(void);
void start_mot_isr(void);

#endif

