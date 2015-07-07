
#include "twrTrdKHit.h"



void twrTrdKHit::Clear(){
  Time=0;
  TRDHit_Amp=0;
  TRDHit_GainCorrection=0;
  TRDHit_Layer=0;
  TRDHit_Ladder=0;
  tubeid=0;
  TRDHit_x=0;
  TRDHit_y=0;
  TRDHit_z=0;
  TRDHit_TRDTrack_x=0;
  TRDHit_TRDTrack_y=0;
  TRDHit_TRDTrack_z=0;
  for (int ii=0;ii<3;ii++){
    TRDTube_Center[ii]=0;
    TRDTube_Dir[ii]=0;
  }
  tube_length=0;
}




