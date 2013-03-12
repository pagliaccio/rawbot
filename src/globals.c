/*****************************************************************************
*                     Copyright (c) 2011, Marcello Bonfe'                     
*                   ENDIF - ENgineering Department In Ferrara,
*                           University of Ferrara.
*                            All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions 
*  are met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of the University of Ferrara nor the
*      names of its contributors may be used to endorse or promote products
*      derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
*  ARE DISCLAIMED. IN NO EVENT SHALL MARCELLO BONFE' BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
*  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
* 
******************************************************************************
 *                                                                    *
 *    Author: Marcello Bonfe'                                         *
 *                                                                    *
 *    Filename:       globals.c                                       *
 *    Date:           28/12/2010                                      *
 *    File Version:   0.1                                             *
 *    Compiler:       MPLAB C30 v3.23                                 *
 *                                                                    *
 ***********************************************************************
 *    Code Description
 *  
 *  This file contains the definition of global variables.
 *
 **********************************************************************/

#include "sys_hw.h" //defines DEVELOP_MODE
#include "generic_defs.h"
#include "extern_globals.h"
#include "geometry.h"

motor MOTOR[N_MOTOR];

// Current/velocity limits
int16_t max_current;

// For BASIC real-time scheduling
// As these are incremented in PWM ISR they are declared as volatile (???)
volatile uint16_t slow_event_count=0;
volatile uint16_t medium_event_count=0;
uint16_t slow_ticks_limit;
uint16_t medium_ticks_limit;

// flags
t_control_flags control_flags;
t_status_flags status_flags;
t_control_mode control_mode;
uint16_t direction_flags_word;
uint16_t direction_flags_prev;

// FOR POSITION feedback

//FOR SPEED MEASURE (rpm)
int32_t kvel;

delta_EE coordinates_actual;
delta_EE coordinates_temp;

float angleJoints_actual[3];
float angleJoints_temp[3];

// FOR CARTESIAN POSITION
//int16_t x_cart,y_cart,z_cart;

//FOR ANGLE POSITION
//int16_t theta1, theta2, theta3;

//MISURED ANGLE POSITION
//int16_t mtheta1, mtheta2, mtheta3;

//FOR ROBOT DIMENSION
float f;
float e;
float lf;
float le;

//FOR ROBOT LIMIT
float sphJLim;
float posJLim;
float negJLim;

//int32_t encoder_counts_rev = 102000; //TODO init with EEPROM

//uint16_t decdeg_to_ticks_int;// = (uint16_t)((encoder_counts_rev << 8)/3600); // in 8.8 fixed point

int32_t encoder_ticks;
//@XXX possibile imprcisione nella divisione
float ticks_to_deg;

#ifdef DEVELOP_MODE 
#ifdef SERIAL_LOG
unsigned logIndex=0;
LOG datalog[MAXLOG];
#endif
#endif
