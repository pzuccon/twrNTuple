#include "twrNTuple.h"
#include <cmath>
#include <cstring>

ClassImp(twrNTuple)

float twrNTuple::betaMC()
{
	if (!isMC) return 0.;
	float xx=mMC/(qMC*rigMC);
	return 1./sqrt(xx*xx + 1.);
}

float twrNTuple::mNaive(int z)
{
	return (float)z*rigInnerTr*sqrt(1./(betaRich*betaRich) - 1.);
}

float twrNTuple::mNaive_K(int z)
{
	return (float)z*rigInnerTr_K*sqrt(1./(betaRich*betaRich) - 1.);
}

bool twrNTuple::hasTrHit(int lNo)
{
	unsigned short int lMask = 1<<(lNo-1);
	return bool(lMask & trPattern);
}

bool twrNTuple::hasTrHitXY(int lNo)
{
	unsigned short int lMask = 1<<(lNo-1);
	return bool(lMask & trPatternXY);
}

int twrNTuple::nTrLayersHit()
{
	int nL=0;
	unsigned short int p = trPattern;
	for (int i=0; i<9; i++)
	{
		if (p & 1) nL++;
		p>>=1;
	}
	return nL;
}

int twrNTuple::nTrLayersHitXY()
{
	int nL=0;
	unsigned short int p = trPatternXY;
	for (int i=0; i<9; i++)
	{
		if (p & 1) nL++;
		p>>=1;
	}
	return nL;
}
