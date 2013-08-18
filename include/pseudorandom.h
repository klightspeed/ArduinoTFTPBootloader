#ifndef PSEUDORANDOM_H
#define PSEUDORANDOM_H

extern uint32_t pseudorandom_state;
void get_pesudorandom_bytes(void *buf, int len);
#define pseudorandom_set_seed(v) do { pseudorandom_state = (v); } while(0)

#endif
