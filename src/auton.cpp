#include "main.h"
#include "pros/motors.h"
#include <algorithm>
#define akp 2
#define akd 2
#define akt 3.00196850394 //Degrees of wheel turn to turn base 1 degree
#define akm 22.9183118053 //Degrees of wheel turn to 1 inch

Motor leftFront (leftFrontPort, MOTOR_GEAR_GREEN, false, MOTOR_ENCODER_DEGREES);
Motor leftMid (leftMidPort, MOTOR_GEAR_GREEN, false, MOTOR_ENCODER_DEGREES);
Motor leftBack (leftBackPort, MOTOR_GEAR_GREEN, false, MOTOR_ENCODER_DEGREES);
Motor rightFront (rightFrontPort, MOTOR_GEAR_GREEN, true, MOTOR_ENCODER_DEGREES);
Motor rightMid (rightMidPort, MOTOR_GEAR_GREEN, true, MOTOR_ENCODER_DEGREES);
Motor rightBack (rightBackPort, MOTOR_GEAR_GREEN, true, MOTOR_ENCODER_DEGREES);
Motor intake (intakePort, MOTOR_GEAR_GREEN, true, MOTOR_ENCODER_DEGREES);

bool voltControl = false, targReach = false;
double targLeft = 0, targRight = 0, errLeft, errRight, prevErrLeft = 0, prevErrRight = 0, derivLeft, derivRight, left, right;

void autonPID(void *ignore){
    leftMid.tare_position();
    rightBack.tare_position();

    while (true) {
        if (!voltControl){
            errLeft = targLeft - leftMid.get_position();
            errRight = targRight - rightBack.get_position();

            derivLeft = errLeft - prevErrLeft;
            derivRight = errRight - prevErrRight;

            left = errLeft*akp + derivLeft*akd;
            if (left < 0){
                left = std::max(left, (double)-70)*1.02;
            } else {
                left = std::min(left, (double)70)*1.02;
            }

            right = errRight*akp + derivRight*akd;
            if (right < 0){
                right = std::max(right, (double)-70);
            } else {
                right = std::min(right, (double)70);
            }

            leftFront.move(left);
            leftMid.move(left);
            leftBack.move(left);
            rightFront.move(right);
            rightMid.move(right);
            rightBack.move(right);

            prevErrLeft = errLeft;
            prevErrRight = errRight;
        } else {
            delay(5);
        }

        targReach = -7.5 < errLeft && errLeft < 7.5 && -7.5 < errRight && errRight < 7.5;

        printf("posLeft: %.2f posRight: %f targLeft: %f targRight: %f, errLeft: %f, errRight: %f\n", leftFront.get_position(), rightFront.get_position(), targLeft, targRight, errLeft, errRight);
        delay(15);
    }
}

void move(double inches, int time = 0){
    targLeft += inches*akm;
    targRight += inches*akm;
    printf("Move: %f\n", inches);

    delay(500);

    if (time != 0){
        printf("Delaying %d", time);
        delay(time - 1500);
    } else {
        while (!targReach) {
            printf("Moving\n");
            delay(20);
        }
    }

    delay(1000);
}

void turn(double degrees, int time = 0){
    targLeft += degrees*akt;
    targRight -= degrees*akt;
    printf("Turning: %f\n", degrees);

    delay(500);

    if (time != 0){
        printf("Delaying %d", time);
        delay(time - 1500);
    } else {
        while (!targReach) {
            printf("Turning\n");
            delay(20);
        }
    }

    delay(1000);
}

void calibration(pathEnumT path){
    Task autonPIDTask (autonPID, (void*)"BALLS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "autonPIDTask");
    
    switch(path){
        case pathEnum_MOVE:
            move(24);
            break;
        case pathEnum_TURN:
            turn(90);
            break;
        case pathEnum_All:
            move(24);
            turn(90);
            break;
    }

    autonPIDTask.remove();
}

void path1(){
    Task autonPIDTask (autonPID, (void*)"BALLS", TASK_PRIORITY_DEFAULT, TASK_STACK_DEPTH_DEFAULT, "autonPIDTask");
    
    move(-40, 3);
    move(15, 2);
    move(-30, 3);

    autonPIDTask.remove();
}
