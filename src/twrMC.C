#include "twrMC.h"

ClassImp(twrMC)

float twrMC::betaMC()
{
	if (!isMC) return 0.;
	float xx=mMC/(qMC*rigMC);
	return 1./sqrt(xx*xx + 1.);
}

void twrMC::Clear()
{
  qMC=rigMC=mMC=0.;
}

twrMC::twrMC(const twrMC& orig)
{
  Clear();
  
  qMC=orig.qMC;
  rigMC=orig.rigMC;
  mMC=orig.rigMC;
}

