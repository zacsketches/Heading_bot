/*
 * Bot designed to drive to a particular heading from its
 * HMC5883 derived compass sensor
*/
#include "Memory.h"

#include <Wire.h>
#include <Filter.h>
#include <Heading_controller.h>

#include <compass.h>
#include <compass_fb.h>
#include <compass_msg.h>

#include <Vector.h>

#include <Controller.h>
#include <Direction.h>
#include <Feedback.h>
#include <Message.h>
#include <Motor.h>
#include <Plant.h>
#include <Sensor.h>
#include <State.h>

#include <Rover_plant.h>
#include <Rover_plant_fb.h>
#include <Rover_plant_msg.h>

const int compass_address = 0x1e;  //I2C 7 bit address for HMC5883

/*---------Feedback Object Shared by Controller and Plant----------*/
Rover_plant_fb<Rover_plant_msg> p_fb;

/*---------Feedback Object Shared by Controller and Compass--------*/
Compass_fb<Compass_msg> c_fb;

/*---------Construct Rover_plant and Motors------------------------*/
Rover_plant plant(2, p_fb);       //(2 motors, feedback_object)
Motor* mtr_lt = new Motor("lt", 4, 5);  //(name, dir_pin, pwm_pin)
Motor* mtr_rt = new Motor("rt", 2, 3);

/*---------Construct Compass -------------------------------------*/
Compass compass(c_fb, compass_address, 10);

/*---------Construct Heading Controller --------------------------*/
Heading_controller controller(1, 1, 250, 5);  //(# of sensors, # of plants, target heading, tolerance)

void setup() {
    Serial.begin(57600);
    Serial.println();    //blank line
    
    //attach motors to the Rover_plant
    plant.attach(mtr_lt);
    plant.attach(mtr_rt);
    
    //always run the attach method for a sensor
    compass.attach();
    
    //Inspect the default bumper state
    compass.config();
    
    //Attach the sensors and plants to the controller.
    //Exit if unable to attach. We can't continue without a properly configured system.
    if (controller.attach(&compass)) Serial.println(F("\nCompass attached"));
    else exit(1);
    if (controller.attach(&plant)) Serial.println(F("\nRover_plant attached"));
    else exit(1);

    //Inspect the default controller state
    controller.config();
    
    //Inspect the amount of free memory
    Serial.print(F("Free RAM after the build: "));
    Serial.println(memory_free());
}

void loop() {
  while(millis() < 3500) {
    //wait a little bit after reset
    delay(10);
  }
  
  while(millis() > 3500) {  //run the whole process via the controller for the rest of the time
    controller.update_state();
    
    plant.take_feedback();

    plant.drive();
  } 
}



