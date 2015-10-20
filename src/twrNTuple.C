#include "twrNTuple.h"
#include <cmath>
#include <cstring>

ClassImp(twrNTuple)

void twrNTuple::Clear()
{
	richQual.Clear();

	Event=0;
	Run=0;
	fStatus=0;
	nParticle_withTrack=0;
  
  	iBH=trFitCode=trFitCode_K=0;

	bool betaHGood=false;
	chi2TBetaH=chi2CBetaH=0.;

	qInnerTr=qInnerTr_RMS=qTrLayer1=0.;
	trPattern=trPatternXY=0;
	
	rigInnerTr=rrErrInnerTr=chiSqInnerTr=normChisqX=normChisqY=bcorr=0.;
	rigInnerTr_K=rrErrInnerTr_K=chiSqInnerTr_K=normChisqX_K=normChisqY_K=bcorr_K=0.;
	bcorrFlag=bcorrFlag_K=0;
	
	flagRich=0;
	betaRich=betaRich_Err=0.;
	
	qUTof=qLTof=betaTof=betaTof_Err=betaSTof=betaCTof=betaCTof_Err=0.;
	
	cutoff_Stoermer=gtod_r=gtod_theta=gtod_phi=qMC=rigMC=mMC=0.;
	
//	for (int i=0; i<20; i++) {pidTrdL_MC[i]=0; momTrdL_MC[i]=0.;}
	for (int i=0; i<9; i++) {pidTrL_MC[i]=0; tkId_Interpolated[i]=0; momTrL_MC[i]=0.; isPrimaryTrL_MC[i]=false;}
	for (int i=0; i<4; i++) {pidTofL_MC[i]=0; betaTofL_MC[i]=0.;}
}

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
