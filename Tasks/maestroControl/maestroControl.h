/*
 * maestroControl.h
 *
 *  Created on: 4 Apr 2014
 *      Author: chris.barlow
 */

#ifndef maestroControl_H_
#define maestroControl_H_

#define MAESTRO_SET_TARGET (0x84)
#define MAESTRO_SET_MULTI_TARGET (0x9F)

volatile void maestroControl_Init(void);
volatile void maestroControl_update(void);


#endif /* maestroControl_H_ */
