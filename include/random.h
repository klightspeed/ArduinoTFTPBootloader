#ifndef RANDOM_H
#define RANDOM_H

#include "wdt_random.h"
#include "sram_random.h"
#include "pseudorandom.h"

#ifdef RANDOM_USE_WDT
#define get_random_bytes(buf,len) wdt_get_random_bytes(buf,len)
#define seed_pseudorandom() wdt_get_random_bytes(&pseudorandom_state, sizeof(pseudorandom_state))
#else
#define get_random_bytes(buf,len) get_pseudorandom_bytes(buf,len)
#define seed_pseudorandom() pseudorandom_set_seed(sram_get_random_seed())
#endif

#endif
