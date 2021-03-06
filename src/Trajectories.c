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
*****************************************************************************
 *                                                                    *
 *    Author: Marcello Bonfe'                                         *
 *                                                                    *
 *    Filename:       Trajectories.c                                  *
 *    Date:           2/1/2011                                        *
 *    File Version:   0.1                                             *
 *    Compiler:       MPLAB C30 v3.23                                 *
 *                                                                    *
 **********************************************************************
 *    Code Description
 *  
 *  This file contains the trajectory generation functions.
 *
 **********************************************************************/

#include "Trajectories.h"
#include "Controls.h"
#include "extern_globals.h"
#include "my_fractmath.h" // for all math stuffs
#include "Comms.h"

/************************************************
* Init TRAJ: resets, output and flags
************************************************/
void InitTRAJ(tTRAJParm *pParm, tTRAJflags *pFlags)
{
}

/************************************************
* Calculate JOG mode trajectory:
* endless feed w/piecewise velocity
************************************************/
void JogTRAJ(tTRAJParm *pParm, tTRAJflags *pFlags)
{
    if(pFlags->enable)
    {
        if(pParm->qdVelocity.i[1] < pParm->qVelCOM)
        {
            // commanded velocity
              pParm->qdVelocity.l += ((int32_t)pParm->qACC << pParm->qACCshift); // accelerate
              if(pParm->qdVelocity.i[1] > pParm->qVelCOM) // don't exceed commanded velocity
                pParm->qdVelocity.i[1] = pParm->qVelCOM;
              if(pParm->qdVelocity.i[1] > pParm->qVLIM) // Don't exceed velocity limit parameter
                pParm->qdVelocity.i[1] = pParm->qVLIM;
        }
        else // Is velact1 >= velcom1
              if(pParm->qdVelocity.i[1] > pParm->qVelCOM) // If current velocity is more than
              { // commanded velocity
                  pParm->qdVelocity.l -= ((int32_t)pParm->qACC << pParm->qACCshift); // decelerate
                  if(pParm->qdVelocity.i[1] < pParm->qVelCOM) // don't exceed commanded velocity
                    pParm->qdVelocity.i[1] = pParm->qVelCOM;
                  if( -pParm->qdVelocity.i[1] > pParm->qVLIM) // Don't exceed velocity limit parameter
                    pParm->qdVelocity.i[1] = -pParm->qVLIM;
              } 

        //pParm->qdPosition += (int32_t)(pParm->qdVelocity.i[1] >> pParm->qVELshift);
        pParm->qdPosition += (int32_t)RSH(pParm->qdVelocity.i[1],pParm->qVELshift);
    }
    else
    {
        pParm->qdVelocity.l = 0;
    }
    
    pFlags->active = pFlags->enable;
}

/************************************************
* Calculate POSITION mode trajectory:
* Simple Trapezoidal velocity "move-to-position"
************************************************/
void PosTRAJ(tTRAJParm *pParm, tTRAJflags *pFlags)
{
  if(pFlags->enable)							//if enable
  {
    if(pFlags->exec && !pFlags->done)			//it's executing and the motion is not complete
    {
      // get rising edge of exec flag
      if(!pFlags->busy)							//if free
      { 
        // compute half distance
        pParm->qdHalfDIST = (pParm->qdPosCOM - pParm->qdPosition)>>1;
        
        if(pParm->qdHalfDIST < 0)				
        {
           pParm->qdHalfDIST = - pParm->qdHalfDIST;
           pFlags->neg_move = 1;
        }
        else
           pFlags->neg_move = 0;				

        pParm->qdVelocity.l = 0;				//reset velocity
        pParm->qFlatCOUNT = 0;					//reset flatcount
        pFlags->half_move = 0;					//first half of motion
        pFlags->busy = 1;
      }//END IF(!..busy)
      else //motion in execution (busy flag active)	//trapezoidal motion implementation
      {
        if(!pFlags->half_move)						//first half of motion
        {
          if(pParm->qdVelocity.i[1] < pParm->qVLIM)	//if speed is smaller than max speed
          {
            // increase commanded velocity
            pParm->qdVelocity.l += ((int32_t)pParm->qACC << pParm->qACCshift); // accelerate
            if(pParm->qdVelocity.i[1] > pParm->qVLIM) // Don't exceed velocity limit parameter
               pParm->qdVelocity.i[1] = pParm->qVLIM;
          }
          else 										//velocity limit has been reached, increment flatcount
            pParm->qFlatCOUNT++;

          // decrement half distance
          //pParm->qdHalfDIST -= (int32_t)(pParm->qdVelocity.i[1] >> pParm->qVELshift);
          pParm->qdHalfDIST -= (int32_t)RSH(pParm->qdVelocity.i[1],pParm->qVELshift);

          if(pFlags->neg_move)
             //pParm->qdPosition -= (int32_t)(pParm->qdVelocity.i[1] >> pParm->qVELshift);
            pParm->qdPosition -= (int32_t)RSH(pParm->qdVelocity.i[1], pParm->qVELshift);
          else
             //pParm->qdPosition += (int32_t)(pParm->qdVelocity.i[1] >> pParm->qVELshift);
            pParm->qdPosition += (int32_t)RSH(pParm->qdVelocity.i[1], pParm->qVELshift);

          //if half distance is negative, first half of the move is completed
          if(pParm->qdHalfDIST <= 0)
             pFlags->half_move = 1;
            
        }//end IF(!pFlags->half_move)
        else //half_move flag is active(second half of motion)
        {
          if(pParm->qFlatCOUNT)
             pParm->qFlatCOUNT--; //decrement flatcount until deceleration phase should start
          else
          {
             if(pParm->qdVelocity.l) //if velocity is not zero
             {
                pParm->qdVelocity.l -= ((int32_t)pParm->qACC << pParm->qACCshift); // decelerate
                if(pParm->qdVelocity.i[1] < 0) // Don't exceed velocity limit parameter
                    pParm->qdVelocity.i[1] = 0;
             }
             else // velocity IS zero, motion completed
             {
               pFlags->done = 1;
			   pFlags->exec = 0;			
               pParm->qdPosition = pParm->qdPosCOM;
             }
          }//end ELSE (for deceleration phase)
          
          if(pFlags->neg_move)
             //pParm->qdPosition -= (int32_t)(pParm->qdVelocity.i[1] >> pParm->qVELshift);
            pParm->qdPosition -= (int32_t)RSH(pParm->qdVelocity.i[1], pParm->qVELshift);
          else
             //pParm->qdPosition += (int32_t)(pParm->qdVelocity.i[1] >> pParm->qVELshift);
            pParm->qdPosition += (int32_t)RSH(pParm->qdVelocity.i[1], pParm->qVELshift);

        }//END else (half_move flag is active)
      }//END motion is execution (busy flag active)
    }//END IF(...exec)
    else
    {
        pFlags->busy = 0;
		//if(pFlags->exec)
        pFlags->done = 0;
    }
    
  }//END IF(.. enable)

  pFlags->active = pFlags->enable;  
}


/************************************************
* SECOND-ORDER NONLINEAR FILTER:
* implementation of theory Zanasi-et-al.
************************************************/
void InitNLFilter2Fx(tNLFOut *NLFOut,tNLFStatus *NLFStatus)
{
    //RESETS THE STATUS
    NLFStatus->qdXint = 0;
    NLFStatus->qdXdot_int = 0;
    NLFStatus->qdRprev = 0;
    NLFStatus->qdRcommand = 0;
    
    // MODE MUST BE 1 or 2, set by user..
    NLFStatus->MODE = 0;

    //RESETS THE OUTPUTS
    NLFOut->qdX = 0;
    NLFOut->qdXdot = 0;
    NLFOut->qdXddot = 0;

} // END InitNLFilter2Fx

// NLFilter calculation
void NLFilter2Fx(tNLFOut *NLFOut,tNLFStatus *NLFStatus, // DATA STRUCTURES
                 uint32_t Xdot_max, uint8_t Umax_SHIFT, // DYNAMIC LIMITS
                 uint8_t Fc_SHIFT)                      // SAMPLING FREQUENCY
{
    // LOCALS
    int32_t tempX1, tempDTF, tempU, sigma, S, abs_sigma, Y, m;
    int64_t beta, longY;
    
    if((NLFStatus->MODE != 1)&&(NLFStatus->MODE != 2)) return;

    // PREPARE temp to calculate velocity and final integration
    tempX1 = NLFStatus->qdXdot_int;
    NLFOut->qdX = tempX1;
    
    if(NLFStatus->MODE == 1)
    {
        // UPDATE COMMAND and its derivative
        tempDTF =  (NLFStatus->qdRcommand << Fc_SHIFT);
        tempDTF -= (NLFStatus->qdRprev << Fc_SHIFT); 
    }
    else // MODE = 2
    {
        // REFERENCE DERIVATIVE calculated outside
        tempDTF = NLFStatus->qdRprev;
    }

    // integrates Xint with tempX1*(Tc/2)
//NLFStatus->qdXint += (tempX1 >> (Fc_SHIFT + 1));
    NLFStatus->qdXint += RSH(tempX1,(Fc_SHIFT + 1));
    
////SLIDING CONTROL LAW
    // tracking error derivative
    //longY = (int64_t)(tempX1 - tempDTF);
    Y = tempX1 - tempDTF;
    
    // calculate sliding surface
    if(NLFStatus->MODE == 1)
    {
        // tracking error
        beta =  (int64_t)(NLFStatus->qdXint - NLFStatus->qdRcommand) << Fc_SHIFT;
    }
    else // MODE = 2
    {
        beta = (int64_t)NLFStatus->qdRcommand << Fc_SHIFT;
    }

//beta += (int64_t)(Y >> 1);
    beta += (int64_t) RSH(Y,1);
    
 //beta = (beta >> Umax_SHIFT);
    beta = RSH(beta,Umax_SHIFT);
    sigma = (int32_t)(beta << Fc_SHIFT);
    
    abs_sigma = FxAbs(sigma);
    
 //m = (1 + iSqrt(1 + (abs_sigma << 3))) >> 1; // (1 + sqrt(1 + 8*abs_sigma)) / 2
    m = RSH((1 + iSqrt(1 + (abs_sigma << 3))), 1);
    
    // THIS ONE IS ACTUALLY sign(z_n)
    S = 0;
    if(sigma > 0)
        S = 1;
    else if(sigma < 0)
            S = -1;
            
    sigma = sigma / m; // CAN WE AVOID THIS DIVISION????
    
    //(Y/Tc)/amax + sigma  + ((m - 1)/2) *S;
//longY = ((int64_t)Y << Fc_SHIFT) >> Umax_SHIFT;
    longY = RSH(((int64_t)Y << Fc_SHIFT), Umax_SHIFT);
    Y = (int32_t)longY;
//sigma += Y + ((m - 1) >> 1) * S; 
    sigma += Y + RSH((m - 1),1) * S; 
    
    // THIS ONE IS sign(sigma_n)
    Y = 0;
    if(sigma > 0)
        Y = 1;
    else if(sigma < 0)
            Y = -1;
    
    // LAST PART OF CONTROL LAW EQUATION
    //(tempX1 * Y + Xdot_max) - (Umax/Fc) ;
    Y = (tempX1 * Y + Xdot_max) - ((1L << Umax_SHIFT) >> Fc_SHIFT);    
    
    S = 0;
    if(Y > 0)
        S = 1;
    else if(Y < 0)
        S = -1;
    
    //sat(sigma), invert sign since final control output is negated..
    //FOR FIXED-POINT MUST REVIEW THIS PART!!!
    if(sigma >= 1)
        Y = -1;
    else if(sigma <= -1)
            Y = 1;
         else
             Y = -sigma;
    
    // FINAL CONTROL OUTPUT, sign already inverted for sat(sigma)
    tempU = (Y << (Umax_SHIFT - 1)) * (1 + S);
    
    // UPDATE OUTPUTS
    NLFOut->qdXddot = tempU;
    NLFOut->qdXdot  = NLFStatus->qdXdot_int;
    NLFOut->qdX     = NLFStatus->qdXint;
    
    // UPDATE STATES FOR NEXT CYCLE
//NLFStatus->qdXdot_int += (tempU >> Fc_SHIFT);
    NLFStatus->qdXdot_int += RSH(tempU,Fc_SHIFT);
    NLFStatus->qdRprev = NLFStatus->qdRcommand; // IF MODE = 2 will be overwritten outside..
//tempX1 = (tempX1 >> (Fc_SHIFT + 1));
    tempX1 = RSH(tempX1,(Fc_SHIFT + 1));
    NLFStatus->qdXint += tempX1 ;
    
}// END NLFilter2Fx

