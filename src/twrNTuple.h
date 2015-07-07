#ifndef twrNTuple_h
#define twrNTuple_h

#include "TObject.h"
#include "twrLevel1R.h"
#include "twrRTI.h"
#include "twrTrdK.h"
#include "twrRichQuality.h"


// Variables fetched by ProcessEvent(), put into a class
class twrNTuple: public TObject
{
public:
  
  twrNTuple(): TObject(), lvl1(), fStatus(0) {}
  ~twrNTuple()
  	{
  	}
  
  // === Associated classes ===
  twrLevel1R lvl1;
  twrRTI     RTI;
  twrRichQuality richQual; // Values used for RICH quality cuts
   twrTrdK   trdk;
  
  // === Basics about event ===
  unsigned int Event;
  unsigned int Run;
  unsigned long long fStatus;
  int nParticle_withTrack;
  
// The highest rigidity particle is picked from each event.  All other quantities relate
// to this chosen particle.
  
// === Quantities used in processing data ===
  	int iBH; // Index of BetaHR object
	int trFitCode; // Return from iTrTrackPar(1,3,0) -- Choutko fit
	int trFitCode_K; // Return from iTrTrackPar(3,3,0) -- chiKanian fit

// === Related to BetaHR class ===
	bool betaHGood; // BetaHR::IsGoodBeta()
	float chi2TBetaH; // BetaHR::GetNormChi2T()
	float chi2CBetaH; // BetaHR::GetNormChi2C()

// === Tracker quantities ===
	// Charge according to inner tracker
	float qInnerTr;
	float qInnerTr_RMS;
	float qTrLayer1; // Charge from tracker layer 1
	// Tracker (XY) hit pattern, bit mask
 	unsigned short int trPattern;
 	unsigned short int trPatternXY;
	
	// Choutko tracker fit parameters
	float rigInnerTr; // Rigidity from inner tracker only
	float rrErrInnerTr; // Error on 1/R, inner tracker
	float chiSqInnerTr; // Overall chi-squared associated with track fit
	float normChisqY; // Chi-squared for Y tracker fit
	float normChisqX;
	float bcorr;
	short int bcorrFlag;
	// chiKanian tracker fit parameters
	float rigInnerTr_K;
	float rrErrInnerTr_K;
	float chiSqInnerTr_K;
	float normChisqY_K;
	float normChisqX_K;
	float bcorr_K;
	short int bcorrFlag_K;
	
// === RICH quantities ===
	int flagRich; // flagRich==1 iff RichRingR passes set of cuts supplied by J.Berdugo
	float betaRich;
	float betaRich_Err;
// 	float richTrackEmPt[5]; // RICH Track Emission Point
// 	float richDistanceTileBorder; // Distance from the nearest RICH radiator tile border
	
// === TOF quantities ===
	// Charge, upper and lower TOF:
	float qUTof;
	float qLTof;
	// Beta:
	float betaTof;
	float betaTof_Err;
	float betaSTof;
	float betaCTof;
	float betaCTof_Err;
	
// === DATA-only quantities ===
	// Magnetic cutoff
	//float cutoff_IGRF;
	float cutoff_Stoermer;
	// Position in orbit (altitude, lat, long in spherical coordinates)
	float gtod_r;
	float gtod_theta;
	float gtod_phi;

// === MC quantities ===
	bool isMC; // Flag for whether not event is an MC event
	float qMC, rigMC, mMC; // True values of Q,R,m for incoming particle
	
// === Derived quantities ===	
	float betaMC(); // True beta of incoming particle, from other MC quantities
	float mNaive(int z); // Mass calculated using measured R, beta, and Z in analytic formula
	float mNaive_K(int z); // Same, for rigidity from chiKanian tracker fit
	bool hasTrHit(int lNo); // Presence of tracker hit on layer lNo using trPattern
	bool hasTrHitXY(int lNo); // Same, for XY hits
	int nTrLayersHit(); // # of tracker layers hit, from trPattern
	int nTrLayersHitXY(); // # of XY tracker hits, from trPatternXY
	
	float qTof() {return 0.5*(qUTof+qLTof);} // Overall Q_TOF is the average of upper & lower charges
	
	// Quantities extracted from fStatus bitmask (see AMS documentation for more info)
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

	ClassDef(twrNTuple, 6);
};

#endif
