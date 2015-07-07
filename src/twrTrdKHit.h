#ifndef twrtrdkhit_h
#define twrtrdkhit_h
#include "Rtypes.h"

class twrTrdKHit {

public:  
  Double32_t 	Time;
  float 	TRDHit_Amp;
  float 	TRDHit_GainCorrection;
  int 	TRDHit_Layer;
  int 	TRDHit_Ladder;
  int 	tubeid;
  float 	TRDHit_x;
  float 	TRDHit_y;
  float 	TRDHit_z;
  float 	TRDHit_TRDTrack_x;
  float 	TRDHit_TRDTrack_y;
  float 	TRDHit_TRDTrack_z;
  Double32_t 	TRDTube_Center[3];
  Double32_t 	TRDTube_Dir[3];

  Double32_t tube_length;
  twrTrdKHit(){Clear();}
  virtual ~twrTrdKHit(){}

  void Clear();
  ClassDef(twrTrdKHit,2);
};



#endif



