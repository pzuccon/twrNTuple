
#include "twrTrdK.h"

twrTrdK::twrTrdK(){
  Clear();
}

twrTrdK::~twrTrdK(){
  Clear();
}

int twrTrdK::mmhits(){
  return mhits.size();
}

void twrTrdK::Clear(){

  for (int ii=0;ii<mhits.size();ii++){
    if(mhits[ii]!=0) delete mhits[ii];
    mhits[ii]=0;
  }
  mhits.clear();
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



twrTrdK::twrTrdK(const twrTrdK& orig){
  Clear();
  for (int ii=0;ii<orig.mhits.size();ii++){
    mhits.push_back( new twrTrdKHit(*(orig.mhits[ii])));
  }
 
  fid=orig.fid;
 
  NHits=orig.NHits;
  IsReadAlignmentOK=orig.IsReadAlignmentOK; 
  IsReadCalibOK=orig.IsReadCalibOK;  
  trd_statk=orig.trd_statk;
  trd_onhitk=orig.trd_onhitk;
  trd_oampk=orig.trd_oampk;


  trd_ipch=orig.trd_ipch;
  ntrdseg=orig.ntrdseg;
   nlay=orig.nlay;
  
  trd_rz=orig.trd_rz;
  trd_rq=orig.trd_rq;
  trd_rpz=orig.trd_rpz;


  for (int ii=0;ii<3;ii++){
    trd_qk[ii]=orig. trd_qk[ii];
    trd_qnhk[ii]=orig.trd_qnhk[ii];
    distrd[ii]=orig.distrd[ii];
    trd_qrmsk[ii]=orig.trd_qrmsk[ii];    
    trd_lik[ii]=orig. trd_lik[ii];
    trd_likr[ii]=orig.trd_likr[ii];
    trd_likre[ii]=orig.trd_likre[ii];
  }
  
}



