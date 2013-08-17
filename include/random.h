#ifndef RANDOM_H
#define RANDOM_H

#include "wdt_random.h"
#include "sram_random.h"

#define get_random_bytes(buf,len) sram_get_random_bytes(buf,len)

#endif
