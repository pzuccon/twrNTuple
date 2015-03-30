#ifndef resClassCommon_h
#define resClassCommon_h

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"

// masses in GeV/c^2
const double mHe4 = 3.7273792;
const double mHe3 = 2.8094134;

double mass_from_BR(double betaInv, double rigInv, int z=2);

//void histFromFile(char* filename, TH1F* hh);

int twoGausFit(TH1* hh, double fwfm = 0.04, double* retParams = 0);
//TF1* threeGausFit(TH1* hh, double fwfm = 0.02, double* retParams = 0);
void getRange_FWfM(TH1* hh, double* range_out, double frac_f = 0.5, double* maxVal_out = 0);
double chi2IndependenceTest_TH2(TH2* hh);
double getChiSquare(TH1F* hh, TF1* ff);



#endif /* resClassCommon_h */

