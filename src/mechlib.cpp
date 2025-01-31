#include "main.h"
#include "pros/misc.h"
#define kp .04
#define kd .08
//Note that values are set for degrees not centidegrees, must change when swapping to rotation sensor

bool shoot = false;
double cataTarg = 6000;

void cataPID(void *ignore){
    Motor cata (cataPort, MOTOR_GEAR_RED, true, MOTOR_ENCODER_DEGREES);
    Rotation rotation (rotationPort);
    Controller master (CONTROLLER_MASTER);

    double power, error, preverror, deriv;
    while (true) {
        if (master.get_digital_new_press(DIGITAL_R1) || shoot == true){
            cata.move(127);
            delay(250);

            shoot = false;
        } else if (master.get_digital(DIGITAL_R2)){
            cata.move(50);
        } else {
            error = cataTarg - rotation.get_position();
            deriv = preverror - error;

            power = error*kp + deriv*kd;
            if (power < 15){
                power = 0;
            }

            cata.move(power);
            printf("error: %f power: %f rotationSensor: %d\n", error, power, rotation.get_position());

            preverror = error;
        }

        // if (master.get_digital_new_press(DIGITAL_R1) || fire == true){
        //     cataTarg += 1080;
        //     fire = false;
        // }
        
        // if (master.get_digital(DIGITAL_R2)){
        //     cata.move(70);
        //     cataTarg = cata.get_position();
        // } else {
        //     error = cataTarg - cata.get_position();
        //     cata.move(error*kp);
        // }

        
        delay(15);
    }
}