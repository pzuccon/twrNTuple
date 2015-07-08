#ifndef twrRichQuality_h
#define twrRichQuality_h

#include "Rtypes.h"

class twrRichQuality
{
public:
	// Default values indicate that this instance of the class has not been
	// filled and should not be used.
	twrRichQuality()
	{
		Clear();
	}
	~twrRichQuality() {}

	void Clear();

// --- Variables ---
	float prob; // Kolmogorov test probability for ring
	int PMTs; // Number of PMTs
	float photoElectrons; // # of PEs in ring
	float collectedPhotoElectrons; // total # of PEs in event (from RichHitR)
	float PMTChargeConsistency; // An index that indicates the level of consistency for
								// hit/PMT charge consistency test (?)
	float trackEmPt[5]; // coordinates of track extrapolation to RICH radiator,
						// as used in reconstruction
	float expectedPhotoElectrons; // Expected number of PEs ...
	float betaConsistency; // Consistency between beta_LIP and beta_CIEMAT
	int tileIndex; // Index of the tile the particle passed through
	float distanceTileBorder;
	
	// RichHitR::getPMTs(), RichHitR::getPMTs(false)
	// true is the default value of this function's argument
	int richHit_PMTs, richHit_PMTs_false;
	
	bool IsGood; // RichRingR::IsGood()
	bool IsClean; // RichRingR::IsClean()
	bool IsNaF; // RichRingR::IsNaF()
	
	ClassDef(twrRichQuality,1);
};

#endif
