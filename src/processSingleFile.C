
#include <utility>	// pair
#include <vector>	// vector
#include <cmath>	// sqrt(double)
#include <cstdio>	// sprintf, printf
#include <iostream>	// cout

#include "resClassCommon.h"
#include "twrNTupleFiller.h"

// #include "TMath.h"
// #include "TCanvas.h"
// #include "TFile.h"
// #include "TH1F.h"


int main(int argc, char** argv)
{
	if ((argc < 3)||(argc > 4))
	{
		printf("[TWR] ERROR: Improper number of arguments:\n %s <in_rootFile> <out_ntupleFile> [maxEvents]\n",argv[0]);
		return -1;
	}

	twrNTupleFiller tnf;
	
	int nMax = -1;
	if (argc>3) nMax = atoi(argv[3]);
	
	int ret = tnf.doPreselect_single_file(argv[1],argv[2],nMax);
	return ret;
}

