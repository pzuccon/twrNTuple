#include "twrRichQuality.h"

ClassImp(twrRichQuality)

void twrRichQuality::Clear()
{
	IsGood = false;
	IsClean = false;
	IsNaF = false;
	prob = 0.;
	PMTs = -1;
	photoElectrons = 0.;
	collectedPhotoElectrons = 0.;
	PMTChargeConsistency = 0.;
	for (int i=0; i<5; i++) trackEmPt[i] = 0.;
	expectedPhotoElectrons = 0.;
	betaConsistency = 0.;
	tileIndex = 0;
	distanceTileBorder = 0.;
	richHit_PMTs = 0;
	richHit_PMTs_false = 0;
}
