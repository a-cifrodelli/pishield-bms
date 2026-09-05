#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <Arduino.h>
#include "config.h"

extern Shudown_Req shutdown_req;

void initRequestShutdown();
void checkShutdownCooldown();
void serial_print_cooldown();
void requestShutdown();
void requestUserShutdown();

#endif
