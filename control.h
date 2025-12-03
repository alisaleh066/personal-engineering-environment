#ifndef CONTROL_H
#define CONTROL_H

#define MAX_POINTS 200

typedef struct {
    double zeta;
    double omega_n;
    double rise_time;
    double peak_time;
    double settle_time;
    double overshoot;
} SystemParams;

typedef struct {
    double Kp;
    double Ki;
    double Kd;
    char name[30];
} PIDConfig;

void control_suite(void);
void second_order_designer(void);
void step_response_calc(void);
void pid_tuner(void);
void equivalence_bridge(void);

#endif