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
 *    Filename:       extern_globals.h                                *
 *    Date:           20/04/2011                                      *
 *    File Version:   0.2                                             *
 *    Compiler:       MPLAB C30 v3.23                                 *
 *                                                                    *
 **********************************************************************
 *    Code Description
 *  
 *  This file contains the extern declarations for global variables.
 *
 **********************************************************************/
#ifndef EXTERN_GLOBALS_H
#define EXTERN_GLOBALS_H 

#include "generic_defs.h" // for data types
#include "PID.h" //for PID parm/flags data-types
#include "Trajectories.h" //for TRAJ parm/flags data-type
#include "Controls.h" //for Homing parm/flags data-type

// FOR ADC samplings 
extern volatile int16_t mcurrent1,mcurrent2,mcurrent3;
extern volatile int16_t mcurrent1_filt,mcurrent2_filt,mcurrent3_filt;
extern volatile int16_t rcurrent1, rcurrent2,rcurrent3;
extern volatile int16_t rcurrent1_req,rcurrent2_req,rcurrent3_req;
extern int16_t mcurrent1_offset,mcurrent2_offset,mcurrent3_offset;

// Current limit
extern int16_t max_current;

// For BASIC real-time scheduling
extern volatile uint16_t slow_event_count;
extern volatile uint16_t medium_event_count;
extern uint16_t slow_ticks_limit;
extern uint16_t medium_ticks_limit;
#define SLOW_RATE  100 // in ms
#define MEDIUM_RATE 20 // in ms

// CONTROL flags
typedef union{
    struct {
    unsigned first_scan          : 1;
    unsigned current_loop_active : 1;
    unsigned pos_loop_active     : 1;
    unsigned track_loop_active   : 1;
    unsigned EE_update_req       : 1;
    unsigned PAR_update_req      : 1;
    unsigned UNUSED              : 2;
    };
    uint8_t b;
} t_control_flags;

extern t_control_flags control_flags;

// STATUS flags
typedef union{
    struct {
    // FIRST BYTE: motor faults
    unsigned overvoltage         : 1;
    unsigned undervoltage        : 1;
    unsigned overcurrent1        : 1; 
    unsigned overcurrent2        : 1; 
    unsigned overcurrent3        : 1;
    unsigned track_error1        : 1;
    unsigned track_error2        : 1;
    unsigned track_error3        : 1;
    // SECOND BYTE: config status
    unsigned homing_done         : 1;
    unsigned UNUSED2             : 7;
    // THIRD BYTE: comm errors
    unsigned comm_err_bad_cmd    : 1;
    unsigned comm_err_CRC        : 1;
    unsigned comm_err_parm_num   : 1;
    unsigned comm_err_parm_range : 1;
    unsigned comm_err_wrong_mode : 1;
    unsigned UNUSED3             : 3;
    // FOURTH BYTE
    unsigned UNUSED4             : 8;
    };
    uint32_t dword;
} t_status_flags;

extern t_status_flags status_flags;

// CONTROL MODE state and transitions
typedef struct{
    union {
    struct {
    unsigned torque_mode_req  : 1; // torque mode
    unsigned ax_pos_mode_req  : 1; // axis position mode
    unsigned cart_mode_req    : 1; 
    unsigned track_mode_req   : 1; 
    unsigned off_mode_req     : 1;
    unsigned UNUSED           : 3;
    };
    uint8_t trxs;
    };
    uint8_t state;
} t_control_mode;

// CONTROL MODE state values
#define OFF_MODE        0
#define TORQUE_MODE     1
#define AX_POS_MODE     2
#define CART_MODE       3
#define TRACK_MODE      4

extern t_control_mode control_mode;

// DIRECTION flags
typedef union{
    struct {
    unsigned motor1_dir          : 1;
    unsigned motor2_dir          : 1;
    unsigned motor3_dir          : 1;
    unsigned encoder1_chB_lead   : 1;
    unsigned encoder2_chB_lead   : 1;
    unsigned encoder3_chB_lead   : 1;
    unsigned UNUSED              : 10;
    };
    uint16_t word;
} t_direction_flags;

extern t_direction_flags direction_flags;
extern uint16_t direction_flags_prev;

// PID parameters and flags structures
// definitions are in the Controls.c source file
// NOT in globals.c
extern tPIDParm PIDCurrent1;
extern tPIDParm PIDCurrent2;
extern tPIDParm PIDCurrent3;
extern tPIDParm PIDPos1;
extern tPIDParm PIDPos2;
extern tPIDParm PIDPos3;

extern tPIDflags PIDCurrent1_f;
extern tPIDflags PIDCurrent2_f;
extern tPIDflags PIDCurrent3_f;
extern tPIDflags PIDPos1_f;
extern tPIDflags PIDPos2_f;
extern tPIDflags PIDPos3_f;

// Trajectory parameters and flags structures
// definitions are in the Controls.c source file
// NOT in globals.c
extern tTRAJParm TRAJMotor1;
extern tTRAJParm TRAJMotor2;
extern tTRAJParm TRAJMotor3;

extern tTRAJflags TRAJMotor1_f;
extern tTRAJflags TRAJMotor2_f;
extern tTRAJflags TRAJMotor3_f;

// nonlinear filter smoothing
extern tNLFStatus Joint1NLFStatus;
extern tNLFStatus Joint2NLFStatus;
extern tNLFStatus Joint3NLFStatus;

extern tNLFOut Joint1NLFOut;
extern tNLFOut Joint2NLFOut;
extern tNLFOut Joint3NLFOut;

// LIMITS for nonlinear filter
extern uint32_t NLF_vel_max;
extern uint32_t NLF_acc_max_shift;

//Homing
extern tHome home;

extern tHomeflags home_f;

// FOR POSITION feedback
extern volatile int16_t mvelocity1,mvelocity2,mvelocity3;
extern volatile int32_t mposition1,mposition2,mposition3;

//FOR SPEED MEASURE (rpm)
extern volatile int16_t velocity1RPM, velocity2RPM;
extern int32_t kvel;
//extern volatile int16_t velocity1RPM_temp, velocity2RPM_temp;

//INITIALIZED IN CONTROLS.C
extern volatile int32_t IC1PeriodTmp, IC2PeriodTmp;
extern volatile int16_t IC1PulseTmp, IC2PulseTmp;

typedef struct {
float x;
float y;
float z;
}delta_EE;//questa struttura contiene le coordinate cartesiane finali dell'end_effector in metri

extern delta_EE coordinates_actual;//memorizza l'attuale posizione effetiva del robot
extern delta_EE coordinates_temp;//ausilio per eseguire i calcoli delle posizioni richieste

typedef struct {
float theta1;
float theta2;
float theta3;
}delta_joints;//questa struttura contiene gli angoli di giunto  finali in radianti

extern delta_joints angleJoints_actual;
extern delta_joints angleJoints_temp;

// FOR CARTESIAN POSITION 
//extern int16_t x_cart,y_cart,z_cart;

// FOR ANGLE POSITION [decimal degrees]
//extern int16_t theta1, theta2, theta3;

//MISURED ANGLE POSITION [degrees]
//extern int16_t mtheta1, mtheta2, mtheta3;

extern int32_t encoder_ticks;

// SYSTEM-WIDE PARAMETERS
// defined in SACT_Protocol.c,
// stored in EEPROM, can be updated by
// the user with SACT commands
extern uint16_t parameters_RAM[];

// VARIABLES FOR RUN-TIME USE OF PARAMETERS
extern int32_t encoder_counts_rev;

extern uint16_t decdeg_to_ticks_int;

#ifdef DEVELOP_MODE 
// DATALOG buffers
#define MAXLOG 100
#define LOGDECIM 1
#ifdef LOG_LONG
extern int32_t dataLOG1[];
extern int32_t dataLOG2[];
#else
extern int16_t dataLOG1[];
extern int16_t dataLOG2[];
extern int16_t dataLOG3[];
extern int16_t dataLOG4[];
#endif
extern uint16_t dataLOGIdx;
extern uint8_t dataLOGdecim;
#endif

#endif
