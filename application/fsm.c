#include "fsm.h"

static void fsm_set_mode();
static void fsm_set_speed();
static void fsm_data_loop();

void fsm_init();
void fsm_loop();

car_t car;