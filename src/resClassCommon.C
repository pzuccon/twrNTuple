#include "resClassCommon.h"
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

// Eliminate these
#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b

//class twoGausTemplate;
//class twrNTuple;

double mass_from_BR(double betaInv, double rigInv, int z)
{
	if ((rigInv == 0) || (betaInv < 1.))
	{
		return 0.;
	}
	return (double)z*sqrt(betaInv*betaInv - 1.)/rigInv;
}

/*
void histFromFile(char* filename, TH1F* hh)
{
	//TFile* tf_data = TFile::Open(filename);
	//if ((tf_data==0)||(!tf_data->IsOpen())) return;
	//TTree* tt = (TTree*)tf_data->Get("tt");
	
	TFile tf_data(filename);
	if (!tf_data.IsOpen()) return;
	TTree* tt = (TTree*)tf_data.Get("tt");
	if (tt==0) return;
	
	std::cout << "Tree is OK.\n";
	
	twrNTuple* ev;
	TBranch* ev_b;
	
	tt->SetBranchAddress("twrNT",&ev,&ev_b);
	
	std::cout << "Branch address set.\n";
	
	// Create histogram
	//char h_name[100];
	//sprintf(h_name,"h_ret");
	//TH1F* hh = new TH1F("hMakeTemplate", "hMakeTemplate", 200, xMin, xMax);
	
	std::cout << "Created TH1F.\n";
	
	// Loop through entries in TTree
	long nEn = tt->GetEntries();
	for (long i = 0; i < nEn; i++)
	{
		ev_b->GetEntry(i);
		if (i%10000==0) std::cout << i << ',';
		if (ev->passes_cuts())
		{
			float rig=ev->rigInnerTr;
			hh->Fill(1./rig);
		}
	}
	
	//std::cout << "Histogram filled with " << hh->GetEntries() << " entries.\n";
	std::cout << "foo\n";
	
	//tf_data->Close();
	//delete tf_data;
}
*/

int twoGausFit(TH1* hh, double fwfm, double* retParams)
/// Fit two Gaussians to a histogram.  This function works best when the Gaussians are centered in
/// the same place.  The current (hard-coded) initial fit guesses correspond to the approximate
/// distributions found in Tracker single-point Y resolution.
{
	double par[6];
	
	double hhInt = hh->ComputeIntegral();
	double ranA[2];
// Get the range of bins that is above the half-max on either side of the maximum bin.
	getRange_FWfM(hh, ranA, 0.5);
	
//	TF1 *gausA = new TF1("gausA","gaus(0)",ranA[0],ranA[1]);
	TF1 *gausA = new TF1("gausA","[2] * exp(-0.5*((x-[0])/[1])**2) / ([1] * sqrt(2*TMath::Pi()))",ranA[0],ranA[1]);
	gausA->SetParameters(hhInt,(ranA[0]+ranA[1])/2.,(ranA[1]-ranA[0])/2.);
	hh->Fit(gausA,"RN");
	gausA->GetParameters(par);
	
	double ranB[2];
	getRange_FWfM(hh, ranB, fwfm);
	
	TF1 *gausABfix = new TF1("gausABfix","((1-[2])*exp(-0.5*((x-[0])/[1])**2)/[1] + [2]*exp(-0.5*((x-[3])/[4])**2)/[4]) * [5] /sqrt(2*TMath::Pi())",ranB[0],ranB[1]);
	gausABfix->SetParLimits(0,0.,1.);
	TF1 *gausAB = (TF1*)gausABfix->Clone("gausAB");
	//virtual TObject*	TNamed::Clone(const char* newname = "") const
	par[5] = hhInt; par[3] = par[0]; par[4] = par[1]*3.;
	par[2] = 0.25;
	
	gausABfix->SetParameters(par);
	for (int i = 1; i <3; i++) gausABfix->FixParameter(i,par[i]);
	hh->Fit(gausABfix,"RN");
	gausABfix->GetParameters(par);
	
	gausAB->SetParameters(par);
	hh->Fit(gausAB,"RN");
	gausAB->GetParameters(par);
	
	if (retParams != 0) {for (int i=0;i<6;i++) retParams[i] = par[i];}
	
	delete gausA; delete gausABfix;
	//return gausAB;
	delete gausAB;
	
	return 0;
}

void getRange_FWfM(TH1* hh, double* range_out, double frac_f, double* maxVal_out)
/// Find the range of x-values falling within the "full width at f max" around the maximum value
/// in a histogram, where f is some fraction 0<f<1.  The range is selected to include the entire
/// width of a bin satisfying this requirement.  This can be used to identify and fit peak regions.
{
	if ((frac_f < 0.) || (frac_f > 1.)) frac_f = 0.5;
	
	// iMax = index of max. bin, maxHeight = content of maximum bin
	int nBins = hh->GetNbinsX();
	double maxHeight = hh->GetBinContent(1);
	int iMax = 1;
	// Locate max. bin
	for (int i = 2; i <= nBins; i++)
	{
		double bc = hh->GetBinContent(i);
		if (bc > maxHeight) {maxHeight = bc; iMax = i;}
	}
	// Find bins on either side that fall above a set fraction (frac_f) of maxHeight.
	// Check that enough points are included for a sensible fit & that bins indices are within range.
	double maxFrac = frac_f * maxHeight;
	int iL = 1, iR = nBins;
	for (iL = MAX(iMax-1,1); iL >= 1; iL--) {if (hh->GetBinContent(iL) < maxFrac) {iL++; break;}}
	for (iR = MIN(iMax+1,nBins); iR <= nBins; iR++) {if (hh->GetBinContent(iR) < maxFrac) {iR--; break;}}
	
	if ((iR-iL) < 5) {iL = MAX(1,iMax); iR = MIN(nBins,iMax);}
	// Fit the first Gaussian initially in the range [r1L, r1R].
	double r1L = hh->GetBinLowEdge(iL);
	double r1R = hh->GetBinLowEdge(iR) + hh->GetBinWidth(iR);
	
	if (maxVal_out != 0) *maxVal_out = maxHeight;
	range_out[0] = r1L; range_out[1] = r1R;
}

double chi2IndependenceTest_TH2(TH2* hh)
{
	int nx = hh->GetNbinsX();
	int ny = hh->GetNbinsY();
	int ndof = (nx-1)*(ny-1);
	
	double nTot=0;
	const int maxDim=1001;
	double p[maxDim];
	double q[maxDim];
//	vector<int> p(nx);
//	vector<int> q(ny);
	
	for (int i=1; i<=nx; i++)
	{
		p[i]=0;
		for (int a=1; a<=ny; a++)
		{
			double bc=hh->GetBinContent(i,a);
			p[i]+=bc;
			nTot+=bc;
		}	
	}
	for (int j=1; j<=ny; j++)
	{
		q[j]=0;
		for (int b=1; b<=nx; b++) q[j]+=hh->GetBinContent(b,j);
	}
	double chiSq=0.;
	for (int i=1; i<=ny; i++)
	{
		for (int j=1; j<=nx; j++)
		{
			double eij=p[i]*q[j]/nTot;
			if (eij == 0.) continue;  // Avoid division by zero
			chiSq+=pow(hh->GetBinContent(i,j) - eij, 2.) / eij;
		}
	}
	
	return chiSq/double(ndof);
}

double getChiSquare(TH1F* hh, TF1* ff)
{
	int nBins = hh->GetNbinsX();
	
	double css = 0.;
	for (int i=1; i<=nBins; i++)
	{
		double x1, x2, bw;
		x1 = hh->GetBinLowEdge(i);
		bw = hh->GetBinWidth(i);
		x2 = x1 + bw;
		double obs = hh->GetBinContent(i);
		double exp = ff->Integral(x1,x2) / (x2-x1);
		
		if (exp==0) continue;
		double err = obs-exp;
		css += (err*err / exp);
	}
	
	return css;
}

