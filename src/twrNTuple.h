#ifndef twrNTuple_h
#define twrNTuple_h

#include "TObject.h"
#include "twrLevel1R.h"
#include "twrRTI.h"

// Variables fetched by ProcessEvent(), put into a class
class twrNTuple: public TObject
{
  public:
  	
	twrNTuple(): TObject(), fStatus(0), lvl1() {}
  	~twrNTuple()
  	{
  	}
  	
	twrLevel1R lvl1;
	twrRTI     RTI;

	unsigned int Event;
	unsigned int Run;
  	unsigned long long fStatus;
  	int nParticle_withTrack;
  	int iBH;
	int trFitCode;
	int trFitCode_K;
	bool betaHGood;
	float chi2TBetaH;
	float chi2CBetaH;
	float qUTof;
	float qLTof;
	float qInnerTr;
	float qInnerTr_RMS;
	// Tracker hit pattern
 	unsigned short int trPattern;
 	unsigned short int trPatternXY;
	
	// Choutko tracker fit parameters
	float rigInnerTr;
	float rrErrInnerTr;
	float chiSqInnerTr;
	float normChisqY;
	float normChisqX;
	// chiKanian tracker fit parameters
	float rigInnerTr_K;
	float rrErrInnerTr_K;
	float chiSqInnerTr_K;
	float normChisqY_K;
	float normChisqX_K;
	// RICH parameters
	int flagRich;
	float betaRich;
	float betaRich_Err;
	float betaTof;
	float betaTof_Err;
	float betaSTof;
	float betaCTof;
	float betaCTof_Err;
	// RICH Track Emission Point
	float richTrackEmPt[5];
	float richDistanceTileBorder;
	
	float qTrLayer1;
	
// DATA-only quantities
	// Magnetic cutoff
	//float cutoff_IGRF;
	float cutoff_Stoermer;
	// Position in orbit (lat, long, angular orientation)
	float gtod_r;
	float gtod_theta;
	float gtod_phi;

// MC quantities
	float qMC, rigMC, mMC;
	bool isMC;
	
	float betaMC();
	float mNaive(int z);
	float mNaive_K(int z);
	bool hasTrHit(int lNo);
	bool hasTrHitXY(int lNo);
	int nTrLayersHit();
	int nTrLayersHitXY();
	
	float qTof() {return 0.5*(qUTof+qLTof);}
	
	int nParticle() {return (fStatus & 3);}
	bool iTrdTrack() {return bool(fStatus & (1<<2));}
	bool iBeta() {return bool(fStatus & (1<<3));}
	bool iTrTrack() {return bool(fStatus & (1<<4));}
	bool iRichRing() {return bool(fStatus & (1<<5));}
	bool iEcalShower() {return bool(fStatus & (1<<6));}
	bool iVertex() {return bool(fStatus & (1<<7));}
	int nTrdTrack() {return 3 & (fStatus>>8);}
	int nTofCluster() {return 7 & (fStatus>>10);}
	int nTrTrack() {return 3 & (fStatus>>13);}
	int nRichRing() {return 3 & (fStatus>>15);}
	int nEcalShower() {return 3 & (fStatus>>17);}
	int nVertex() {return 3 & (fStatus>>19);}
	int nAntiCluster() {return 3 & (fStatus>>21);}
	int charge() {return 7 & (fStatus>>23);}
	bool hasErrors() {return bool(1 & (fStatus>>30));}
	bool statusNotFound() {return bool(1 & (fStatus>>31));}
	bool isPrescaled() {return bool(1 & (fStatus>>32));}
	bool posVelocity() {return bool(1 & (fStatus>>33));}
	bool posMomentum() {return bool(1 & (fStatus>>34));}
	bool isNaF() {return bool(1 & (fStatus>>52));}

	ClassDef(twrNTuple, 3);
};

#endif
