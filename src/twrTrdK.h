#ifndef mtrdk_h
#define mtrdk_h
#include "Rtypes.h"

class twrTrdKHit{

public:  
  double 	Time;
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
  double 	TRDTube_Center[3];
  double 	TRDTube_Dir[3];

  double tube_length;
  twrTrdKHit(){Clear();}
  virtual ~twrTrdKHit(){}

  void Clear();
  ClassDef(twrTrdKHit,1);
};



class twrTrdK{
public:
  int fid;
  double trd_lik[3];
  double trd_likr[3];
  double trd_likre[3];

  int NHits;
  int IsReadAlignmentOK; ///< 0: No Alignment, 1: Static Alignment Layer level,  2: Dynamic
  int IsReadCalibOK;  ///< 0: No Gain Calibration, 1: Gain Calibration Succeeded
  int trd_statk;
  int trd_onhitk;
  float trd_oampk;

  int ntrdseg; ///< number of trd segments
  int nlay;///< Number of layers in the trd track
  double distrd[3];///< dist Tr TRD track [0]=x [1]=y [2]= acos(tr_dir _dot_ tr_dir) in deg
  double trd_rz;
  double trd_rq;
  double trd_rpz;


  std::vector<twrTrdKHit> hits;

  double trd_qk[3];/*
		     trd_qk[0]=trdcluster.GetTRDCharge();
		     trd_qk[1]=   trdcluster.GetTRDChargeUpper();
		     trd_qk[2]=   trdcluster.GetTRDChargeLower();
		   */
  double trd_qrmsk[3];///<=trdcluster.GetTRDChargeError();
  double  trd_qnhk[3];/*
			trd_qnhk[0]= trdcluster.GetQNHit();
			trd_qnhk[1]= trdcluster.GetQNHitUpper();
			trd_qnhk[2]= trdcluster.GetQNHitLower();
		      */
  double trd_ipch;///<    trdcluster.GetIPChi2();//impact Chis

  twrTrdK(){Clear();}
  virtual ~twrTrdK(){}
  void Clear();

  ClassDef(twrTrdK,1);
};


#endif


#ifdef mtrdk_cxx


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



void twrTrdK::Clear(){

  hits.clear();
  fid=0;
 
  NHits=0;
  IsReadAlignmentOK=0; 
  IsReadCalibOK=0;  
  trd_statk=0;
  trd_onhitk=0;
  trd_oampk=0;


  trd_ipch=0;
  ntrdseg=0;
   nlay=0;
  
  trd_rz=0;
  trd_rq=0;
  trd_rpz=0;


  for (int ii=0;ii<3;ii++){
    trd_qk[ii]=0;
    trd_qnhk[ii]=0;
    distrd[ii]=0;
    trd_qrmsk[ii]=0;    
 trd_lik[ii]=0;
    trd_likr[ii]=0;
    trd_likre[ii]=0;
  }

}



#endif
