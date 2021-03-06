#ifndef twrNTupleFiller_h
#define twrNTupleFiller_h

// #include <signal.h>
// #include <cstdio>
// #include <string>
// #include <fstream>
//#include <cmath>
#include <climits>
#include <csignal>

#include "root_RVSP.h"
#include "amschain.h"
#include "HistoMan.h"
#include "FrameTrans.h"
#include "root_setup.h"
#include "TofTrack.h"
#include "TNtuple.h"
#include "Tofrec02_ihep.h"
#include "TrdSCalib.h"

#include "twrNTuple.h"
#include "resClassCommon.h"


class twrNTupleFiller
{
private:
	bool isFileType(char* filename, char* ext);
	// Early termination signal handler
	static int _stop;
	static void _sigHandler(int sig) {twrNTupleFiller::_stop=1; return;}

public:
// 	char inDataPath[256];
// 	char outNTuplePath[256];
	AMSSetupR::RTI rti;
	twrNTuple nt;
	
public:
// 	twrNTupleFiller(): TObject(), nMaxToProcess(INT_MAX)
	twrNTupleFiller()
	{
// 		sprintf(inDataPath,"root://eosams.cern.ch//eos/ams/Data/AMS02/2011B/ISS.B620/pass4/");
// 		sprintf(outNTuplePath,"/afs/cern.ch/user/b/behlmann/twr/resClass/ntuples/");
//		AMSSetupR::RTI::UseLatest(6);
		RichRingR::setBetaCorrection( RichRingR::fullUniformityCorrection );
	}
	~twrNTupleFiller() {}
	
// 	int nMaxToProcess;
	
	int doPreselect_single_file(char* rootFile, char* outNTupleFile, int maxEvents = -1);
	
	
	
// 	int doPreselect(char* rootListFile, char* outNTupleFile = 0);
// 	int doPreselect_separate_files(char* listFile, int nMaxFiles = -1);
	int RichQC(twrRichQuality *vals);
	int fillNTuple_preselect(twrNTuple &twrNT, AMSEventR* ev);
	int highestRigParticle(AMSEventR* ev, int& nPar_wTrack);
	
  //	ClassDef(twrNTupleFiller, 0);
};

int associateTrMCCluster_TkId(AMSEventR* ev, int tkid);
int associateTrMCCluster_Pt(AMSEventR* ev, AMSPoint pt);

#endif /* twrNTupleFiller_h */
