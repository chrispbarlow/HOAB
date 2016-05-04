/*
 * maestro.h
 *
 *  Created on: 4 Apr 2014
 *      Author: chris.barlow
 */

#ifndef maestro_H_
#define maestro_H_

#define MAESTRO_SET_TARGET			(0x84)
#define MAESTRO_SET_MULTI_TARGET	(0x9F)
#define MAESTRO_SET_SPEED			(0x87)
#define MAESTRO_SET_ACCEL			(0x89)
#define MAESTRO_GET_STATE			(0x93)

void maestro_Init(void);
void maestro_update(void);


#endif /* maestro_H_ */
