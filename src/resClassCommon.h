#ifndef resClassCommon_h
#define resClassCommon_h

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"

// masses in GeV/c^2
const double mHe4 = 3.7273792;
const double mHe3 = 2.8094134;

// Particle IDs are offset by 1; m=geantMass[pid-1]
// Be sure to check that 1<=pid<=50 before calling to avoid segmentation violations
const float geantMass[50]={
.0000E+00,.5110E-03,.5110E-03,.0000E+00,.1057E+00,.1057E+00,.1350E+00,.1396E+00,.1396E+00,.4977E+00,
.4937E+00,.4937E+00,.9396E+00,.9383E+00,.9383E+00,.4977E+00,.5475E+00,.1116E+01,.1189E+01,.1193E+01,
.1197E+01,.1315E+01,.1321E+01,.1672E+01,.9396E+00,.1116E+01,.1189E+01,.1193E+01,.1197E+01,.1315E+01,
.1321E+01,.1672E+01,-1.      ,-1.      ,-1.      ,-1.      ,-1.      ,-1.      ,-1.      ,-1.      ,
-1.      ,-1.      ,-1.      ,-1.      ,.1876E+01,.2809E+01,.3727E+01,.0000E+00,.2809E+01,.0000E+00};

double mass_from_BR(double betaInv, double rigInv, int z=2);

//void histFromFile(char* filename, TH1F* hh);

int twoGausFit(TH1* hh, double fwfm = 0.04, double* retParams = 0);
//TF1* threeGausFit(TH1* hh, double fwfm = 0.02, double* retParams = 0);
void getRange_FWfM(TH1* hh, double* range_out, double frac_f = 0.5, double* maxVal_out = 0);
double chi2IndependenceTest_TH2(TH2* hh);
double getChiSquare(TH1F* hh, TF1* ff);



#endif /* resClassCommon_h */

