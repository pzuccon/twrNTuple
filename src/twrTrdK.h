#ifndef twrtrdk_h
#define twrtrdk_h
#include "Rtypes.h"
#include "twrTrdKHit.h"
#include <vector>

class twrTrdK{
public:
  int fid;
  Double32_t trd_lik[3];
  Double32_t trd_likr[3];
  Double32_t trd_likre[3];

  int NHits;
  int IsReadAlignmentOK; ///< 0: No Alignment, 1: Static Alignment Layer level,  2: Dynamic
  int IsReadCalibOK;  ///< 0: No Gain Calibration, 1: Gain Calibration Succeeded
  int trd_statk;
  int trd_onhitk;
  float trd_oampk;

  int ntrdseg; ///< number of trd segments
  int nlay;///< Number of layers in the trd track
  Double32_t distrd[3];///< dist Tr TRD track [0]=x [1]=y [2]= acos(tr_dir _dot_ tr_dir) in deg
  Double32_t trd_rz;
  Double32_t trd_rq;
  Double32_t trd_rpz;


  std::vector<twrTrdKHit*> mhits;

  Double32_t trd_qk[3];/*
		     trd_qk[0]=trdcluster.GetTRDCharge();
		     trd_qk[1]=   trdcluster.GetTRDChargeUpper();
		     trd_qk[2]=   trdcluster.GetTRDChargeLower();
		   */
  Double32_t trd_qrmsk[3];///<=trdcluster.GetTRDChargeError();
  Double32_t  trd_qnhk[3];/*
			trd_qnhk[0]= trdcluster.GetQNHit();
			trd_qnhk[1]= trdcluster.GetQNHitUpper();
			trd_qnhk[2]= trdcluster.GetQNHitLower();
		      */
  Double32_t trd_ipch;///<    trdcluster.GetIPChi2();//impact Chis

  twrTrdK();
  twrTrdK(const twrTrdK& orig);
  virtual ~twrTrdK();
  int mmhits();
  int size() {return mhits.size();}

  twrTrdKHit* GetHit(int ii) {return mhits.at(ii);}
  void Clear();

  ClassDef(twrTrdK,2);
};


#endif



