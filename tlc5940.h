/*
 * tlc5940.h
 *
 * Created: 12.10.2012 13:52:21
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 */ 


#ifndef TLC5940_H_
#define TLC5940_H_

extern volatile uint8_t* ptr_back;
extern volatile uint8_t* ptr_front;

extern void TLC5940_Init();
extern void TLC5940_ClockInGS();

#endif /* TLC5940_H_ */