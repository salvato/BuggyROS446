#pragma once

#define LIBRARY_VERSION	1.1.1

class PID
{
public:
    //Constants used in some of the functions below
    static const int AUTOMATIC = 1;
    static const int MANUAL    = 0;
    static const int DIRECT    = 0;
    static const int REVERSE   = 1;
    static const int P_ON_M    = 0;
    static const int P_ON_E    = 1;

    //commonly used functions **************************************************************************
    PID(double*, double*, double*,        // * Constructor.  links the PID to the Input, Output, and
        double, double, double, int, int);//   Setpoint.  Initial tuning parameters are also set here.
                                          //   (overload for specifying proportional mode)

    PID(double*, double*, double*,        // * Constructor.  links the PID to the Input, Output, and
        double, double, double, int);     //   Setpoint.  Initial tuning parameters are also set here

    void SetMode(int Mode);               // * Sets PID to either Manual (0) or Auto (non-0)

    bool Compute();                       // * Performs the PID calculation.  it should be
                                          //   called every time loop() cycles. ON/OFF and
                                          //   calculation frequency can be set using SetMode
                                          //   SetSampleTime respectively

    void SetOutputLimits(double, double); // * Clamps the output to a specific range. 0-255 by default, but
                                                              //   it's likely the user will want to change this depending on
                                                              //   the application

    // Available but not commonly used functions ********************************************************
    void SetTunings(double, double,       // * While most users will set the tunings once in the
                    double);         	    //   constructor, this function gives the user the option
                                          //   of changing tunings during runtime for Adaptive control
    void SetTunings(double, double,       // * Overload for specifying proportional mode
                    double, int);

    void SetControllerDirection(int);	    // * Sets the Direction, or "Action" of the controller. DIRECT
                                                              //   means the output will increase when error is positive. REVERSE
                                                              //   means the opposite.  it's very unlikely that this will be needed
                                                              //   once it is set in the constructor.

    // Display functions ****************************************************************
    double GetKp();						            //   These functions query the pid for interal values.
    double GetKi();						            //   they were created mainly for the pid front-end,
    double GetKd();						            //   where it's important to know what is actually
    int GetMode();						            //   inside the PID.
    int GetDirection();					          //

private:
    void Initialize();

    double kp;                            // * (P)roportional Tuning Parameter
    double ki;                            // * (I)ntegral Tuning Parameter
    double kd;                            // * (D)erivative Tuning Parameter

    int controllerDirection;
    int pOn;

    double *myInput;                      // * Pointers to the Input, Output, and Setpoint variables
    double *myOutput;                     //   This creates a hard link between the variables and the
    double *mySetpoint;                   //   PID, freeing the user from having to constantly tell us
                                          //   what these values are.  with pointers we'll just know.

    double outputSum, lastInput;

    double outMin, outMax;
    bool inAuto, pOnE;
};
