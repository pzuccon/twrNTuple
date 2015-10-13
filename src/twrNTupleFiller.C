#include "twrNTupleFiller.h"

#ifdef _IS_MC_
#include "TkSens.h" // TkSens used to match MC Tracker hits to detected hits by ladder
#endif




#ifndef _IS_MC_

//ClassImp(twrNTupleFiller);
static void cloneRTI(twrRTI & dest,  AMSSetupR::RTI& orig){

  dest.IsInSAA=orig.IsInSAA();
  
  dest.run=orig.run;  
  dest.evno=orig.evno;
  dest.evnol=orig.evnol;
  dest.lf=orig.lf;  
  
  for (int ii=0;ii<4;ii++)
    for (int jj=0;jj<2;jj++){
      dest.cf[ii][jj]=orig.cf[ii][jj];  
      dest.cfi[ii][jj]=orig.cfi[ii][jj];  
    }

  dest.mphe=orig.mphe;
  dest.theta=orig.theta;
  dest.phi=orig.phi;
  dest.r=orig.r;    
  dest.zenith=orig.zenith; 
  dest.glat=orig.glat;   
  dest.glong=orig.glong; 
  dest.nev=orig.nev;    
  dest.nerr=orig.nerr;  
  dest.ntrig=orig.ntrig; 
  dest.nhwerr=orig.nhwerr;
  dest.npart=orig.npart;  
  for (int ii=0;ii<2;ii++){
    dest.usec[ii]=orig.usec[ii];
    dest.utctime[ii]=orig.utctime[ii];
    for (int jj=0;jj<2;jj++)
      dest.nl1l9[ii][jj]=orig.nl1l9[ii][jj];
    for (int kk=0;kk<3;kk++)
      dest.dl1l9[ii][kk]=orig.dl1l9[ii][kk]; 
  }
  dest.mtrdh=orig.mtrdh;
  dest.good=orig.good;
  dest.utime=orig.utime;
  dest.Version=orig.Version; 


}

#endif

static void cloneLevel1R( twrLevel1R& dest, const Level1R &copyMe)
{
  dest.PhysBPatt = copyMe.PhysBPatt;
  dest.JMembPatt = copyMe.JMembPatt;
  dest.AuxTrigPatt = copyMe.AuxTrigPatt;
  dest.TofFlag1 = copyMe.TofFlag1;
  dest.TofFlag2 = copyMe.TofFlag2;
  for (int i = 0; i < 4; i++)
    {
      dest.TofPatt1[i] = copyMe.TofPatt1[i];
      dest.TofPatt2[i] = copyMe.TofPatt2[i];
    }
  dest.AntiPatt = copyMe.AntiPatt;
  dest.EcalFlag = copyMe.EcalFlag;
  for (int i = 0; i < 6; i++)
    {
	    for (int j = 0; j < 3; j++) dest.EcalPatt[i][j] = copyMe.EcalPatt[i][j];
    }
  dest.EcalTrSum = copyMe.EcalTrSum;
  dest.LiveTime = copyMe.LiveTime;
  for (int i = 0; i < 19; i++) dest.TrigRates[i] = copyMe.TrigRates[i];
  for (int i = 0; i < 5; i++) dest.TrigTime[i] = copyMe.TrigTime[i];
}


bool twrNTupleFiller::isFileType(char* filename, char* ext)
{
	bool ret=true;
	int extL=strlen(ext);
	int fnL=strlen(filename);
	if (fnL<extL) ret=false;
	else
	{
		int fnOffset=fnL-extL;
		for (int j=0; j<extL; j++) {if (ext[j] != filename[fnOffset+j]) ret=false;}
	}
	return ret;
}

int twrNTupleFiller::doPreselect_single_file(char* rootFile, char* outNTupleFile, int maxEvents)
{
	AMSChain ch;
	
	// Load input AMS data file
	int addOK;
	addOK = ch.Add(rootFile);
	printf("Loading single AMS data file: %s\n  > Status=%d\n",rootFile,addOK);
	
	if (!ch.GetEntries()) {printf("ERROR processing input data file(s)--ABORT doPreselect()\n"); return addOK;}
	printf("Number of entries loaded: %d\n",ch.GetEntries());	
	
	// Open output file and tree for n-tuples
	TFile* tf = new TFile(outNTupleFile, "RECREATE");
	if (!tf->IsOpen())
	{
		printf("ERROR opening output TFile:%s--ABORT doPreselect()\n",outNTupleFile);
		if (tf) {delete tf; tf=0;}
		return -10;
	}
	
	// Create TTree to hold output ntuple events
	TTree* tt = new TTree("tt","Preselected n-tuple events for He isotope analysis.");
	tt->Branch("twrNT","twrNTuple",&nt,0);

	// Execute this once before working with RICH data
	RichRingR::setBetaCorrection( RichRingR::fullUniformityCorrection );

	// Bookkeeping for cut acceptances
	int nCuts = 16;
	nCuts++;
	int accumCuts[nCuts];
	for (int i=0; i<nCuts; i++) accumCuts[i]=0;
	
	// Decide how many events to loop through then start
	int nEv = ch.GetEntries();
	int nToUse;

#ifndef _IS_MC_
// 	AMSSetupR::RTI::UseLatest(); // before pass6
	AMSSetupR::RTI::UseLatest(6); // pass6 onward
#endif
	
	// These lines are recommended for inclusion.  They are in the analysis_amsd30n.C file
	TkDBc::UseFinal();
	TRMCFFKEY_DEF::ReadFromFile = 0;
	TRFITFFKEY_DEF::ReadFromFile = 0;
	TRFITFFKEY.magtemp = 1;  // pass6 only
	
	AMSEventR* ev0=ch.GetEvent();
	if (maxEvents < 0) nToUse = nEv;
	else nToUse = (maxEvents<nEv?maxEvents:nEv);
	long int presel=0;
	for (int i = 0; i < nToUse;i++)
	{
	  if (i%10000==0) printf("Processed %9d out of %9d(max %9d)  preselected %9d \n",i,nToUse,nEv,presel);
		AMSEventR* ev = ch.GetEvent(i);
		int procRet = fillNTuple_preselect(nt, ev);
//		printf("ev%d: %d\n",i,procRet);
		if (procRet==0) {tt->Fill(); presel++;}
		if (procRet<nCuts) accumCuts[procRet]++;
//		hman.Fill("ProcessEventRet",procRet);
//		if (stop==1) break;
	}
	
	printf("FINISHED processing %9d events\n",nToUse);

	// Process histograms that track cuts and acceptances
	char hTit[100];
	sprintf(hTit,"Preselect return value for %d events",nToUse);
	TH1I* h_returnVal = new TH1I("h_returnVal",hTit,nCuts,0,nCuts);
	for (int i=0; i<nCuts; i++) h_returnVal->Fill(i,accumCuts[i]);
	h_returnVal->GetXaxis()->SetTitle("Return (cut # or 0 if all cuts passed)");
	h_returnVal->GetYaxis()->SetTitle("Events");
	h_returnVal->SetStats(false);

	sprintf(hTit,"Preselection acceptance based on %d original events",nToUse);
	TH1I* h_preselect = new TH1I("h_preselect",hTit,nCuts,0,nCuts);
	for (int i=1; i<nCuts; i++)
	{
		for (int j=0; j<i; j++) h_preselect->Fill(j,accumCuts[i]);
	}
	for (int j=0; j<nCuts; j++) h_preselect->Fill(j,accumCuts[0]);
	h_preselect->GetXaxis()->SetTitle("Cut #");
	h_preselect->GetYaxis()->SetTitle("Events remaining");
	h_preselect->SetStats(false);
	tf->cd();
//	gDirectory->pwd();
    printf("Output directed to: %s",gDirectory->GetPath());
    h_returnVal->Write();
	h_preselect->Write();
	
	// Write file; wrap up
	tf->Write();
	printf("Output n-tuple file will be found at %s\n",outNTupleFile);
//	hman.Save();

	return 0;
}

int twrNTupleFiller::fillNTuple_preselect(twrNTuple &twrNT, AMSEventR* ev)
{  
	twrNT.Clear();
	
	int errRet=0;
	
	int hrPart = highestRigParticle(ev, twrNT.nParticle_withTrack);
	// Problem finding the particle with the highest (absolute) rigidity.
	//    (This includes the case that there are no particles with an associated track.)
	errRet++; if (hrPart<0) return errRet; // CUT 1

	twrNT.isMC = ((ev->nMCEventg()>0)?true:false);

	if (!twrNT.isMC)
	{
//		RTI cuts
		int rtiRet = ev->GetRTI(rti);
		errRet++; if (rtiRet != 0) return errRet; // CUT 2
		
		//printf("RTI Status: %d , livetime: %f\n",ev->GetRTIStat(),rti.lf);
		errRet++; if (rti.lf < 0.7) return errRet; // CUT 3
		errRet++; if (rti.IsInSAA()) return errRet; // CUT 4
		errRet++; if ((rti.nhwerr/rti.ntrig)>0.01) return errRet; // CUT 5
		errRet++; if ((rti.ntrig/rti.nev)<0.99) return errRet; // CUT 6
	}
	else errRet+=5;

	twrNT.fStatus=ev->fStatus;
	twrNT.Event=ev->Event();
	twrNT.Run=ev->Run();

	ParticleR* par = ev->pParticle(hrPart);

	TrTrackR* trTr = par->pTrTrack();
	
	// No XY hit on tracker layer 1
	//pz	if (!trTr->TestHitLayerJHasXY(1)) return errRet;
	errRet++; if (!trTr->TestHitLayerJ(1)) return errRet; // CUT 7
	
	// Choutko algo. fit (1); inner tracker only (3); force refit (2);
	//   assume He4 mass; assume charge 2
	twrNT.trFitCode = trTr->iTrTrackPar(1,3,2,mHe4,2);
	// chiKanian algo. fit (3)
	twrNT.trFitCode_K = trTr->iTrTrackPar(3,3,2,mHe4,2);
	// Fit did not succeed.  (SHOULD NOT OCCUR)
	errRet++; if ((twrNT.trFitCode < 0)&&(twrNT.trFitCode_K < 0)) return errRet; // CUT 8
	
	twrNT.iBH=par->iBetaH();
	bool flag_betaH = (twrNT.iBH > -1);
	bool flag_richRing = (twrNT.iRichRing());
	// No source of beta
	errRet++; if (!(flag_betaH || flag_richRing)) return errRet; // CUT 9
//	errRet++; if ((twrNT.iBH == -1)||(!twrNT.iBeta())) return errRet;
//	errRet++; if (!twrNT.iRichRing()) return errRet;
	
//	TofRecH::ReBuild();
	BetaHR* betaH = par->pBetaH();
	errRet++; if (!betaH || (betaH->GetBeta()<=0)) return errRet; // CUT 10
	twrNT.betaHGood = betaH->IsGoodBeta();
	
	twrNT.chi2TBetaH = betaH->GetNormChi2T();
	twrNT.chi2CBetaH = betaH->GetNormChi2C();

	twrNT.qUTof = 0.5*(betaH->GetQL(0) + betaH->GetQL(1));
	// Upper TOF charge far from 2.0
	errRet++; if ((twrNT.qUTof < 1.5)||(twrNT.qUTof > 3.0)) return errRet; // CUT 11
	twrNT.qLTof = 0.5*(betaH->GetQL(2) + betaH->GetQL(3));
	// Lower TOF charge far from 2.0
	errRet++; if ((twrNT.qLTof < 1.5)||(twrNT.qLTof > 3.0)) return errRet; // CUT 12
	
	twrNT.qTrLayer1 = trTr->GetLayerJQ(1,betaH->GetBeta());
	// Tracker layer 1 charge far from 2.0
	errRet++; if ((twrNT.qTrLayer1 < 1.5)||(twrNT.qTrLayer1 > 3.0)) return errRet; // CUT 13
	
	errRet++; if (ev->nLevel1() != 1) return errRet; // CUT 14
	cloneLevel1R(twrNT.lvl1,ev->Level1(0));
#ifndef _IS_MC_
	cloneRTI(twrNT.RTI,rti);
#endif

//	twrNT.lvl1 = new twrLevel1R();

	twrNT.qInnerTr = trTr->GetQ();
	twrNT.qInnerTr_RMS = trTr->GetQ_RMS();
	
	twrNT.trPattern = trTr->GetBitPatternJ();
	twrNT.trPatternXY = trTr->GetBitPatternXYJ();
	
	if (twrNT.trFitCode >= 0)
	{
		TrTrackPar fit_C = trTr->gTrTrackPar(twrNT.trFitCode);
		twrNT.rigInnerTr = float(trTr->GetRigidity(twrNT.trFitCode));
		twrNT.rrErrInnerTr = float(trTr->GetErrRinv(twrNT.trFitCode));
		twrNT.chiSqInnerTr = float(trTr->GetChisq(twrNT.trFitCode));
		twrNT.normChisqY = float(trTr->GetNormChisqY(twrNT.trFitCode));
		twrNT.normChisqX = float(trTr->GetNormChisqX(twrNT.trFitCode));
		twrNT.bcorr = fit_C.Bcorr;
		twrNT.bcorrFlag = fit_C.BcorrFlag;
	}
	
	if (twrNT.trFitCode_K >= 0)
	{
		TrTrackPar fit_K = trTr->gTrTrackPar(twrNT.trFitCode_K);
		twrNT.rigInnerTr_K = float(trTr->GetRigidity(twrNT.trFitCode_K));
		twrNT.rrErrInnerTr_K = float(trTr->GetErrRinv(twrNT.trFitCode_K));
		twrNT.chiSqInnerTr_K = float(trTr->GetChisq(twrNT.trFitCode_K));
		twrNT.normChisqY_K = float(trTr->GetNormChisqY(twrNT.trFitCode_K));
		twrNT.normChisqX_K = float(trTr->GetNormChisqX(twrNT.trFitCode_K));
		twrNT.bcorr_K = fit_K.Bcorr;
		twrNT.bcorrFlag_K = fit_K.BcorrFlag;
	}
	
	float bestRig=(twrNT.trFitCode>=0) ? twrNT.rigInnerTr : twrNT.rigInnerTr_K;
	
	twrNT.betaTof = betaH->GetBeta();
	twrNT.betaTof_Err = betaH->GetEBetaV();
	twrNT.betaSTof = betaH->GetBetaS();
	twrNT.betaCTof = betaH->GetBetaC();
	twrNT.betaCTof_Err = betaH->GetEBetaCV();

	RichRingR* richRing = par->pRichRing();
	if (richRing>0)
	{
		twrNT.betaRich = richRing->getBeta();
		twrNT.betaRich_Err = richRing->getBetaError();
		{
			twrNT.richQual.IsGood = richRing->IsGood();
			twrNT.richQual.IsClean = richRing->IsClean();
			twrNT.richQual.IsNaF = richRing->IsNaF();
			twrNT.richQual.prob = richRing->getProb();
			twrNT.richQual.PMTs = richRing->getPMTs();
			twrNT.richQual.photoElectrons = richRing->getPhotoElectrons();
			twrNT.richQual.collectedPhotoElectrons = RichHitR::getCollectedPhotoElectrons();
			twrNT.richQual.PMTChargeConsistency = richRing->getPMTChargeConsistency();
			const float* rtep = richRing->getTrackEmissionPoint();
			for (int i=0; i<5; i++) twrNT.richQual.trackEmPt[i] = rtep[i];
			twrNT.richQual.expectedPhotoElectrons = richRing->getExpectedPhotoelectrons();
			twrNT.richQual.betaConsistency = richRing->getBetaConsistency();
			twrNT.richQual.tileIndex = richRing->getTileIndex();
			twrNT.richQual.distanceTileBorder = richRing->DistanceTileBorder();
			twrNT.richQual.richHit_PMTs = RichHitR::getPMTs();
			twrNT.richQual.richHit_PMTs_false = RichHitR::getPMTs(false);
		}
		twrNT.flagRich = RichQC(&twrNT.richQual);
		
		// For low rig. NaF events, require consistency with TOF beta
		if (twrNT.flagRich==1 && twrNT.richQual.IsNaF && bestRig<4.)
		{
			float bR = twrNT.betaRich;
			float bT = twrNT.betaTof;
			if (abs((bR-bT)/(bR+bT)) > 0.03) twrNT.flagRich=-15;
		}
	}
	else
	{	
		twrNT.betaRich = 0;
		twrNT.betaRich_Err = 0;
		twrNT.richQual.Clear();
		twrNT.flagRich = 0;
// 		for (int k=0; k<5; k++) twrNT.richTrackEmPt[k] = 0;
// 		twrNT.richDistanceTileBorder = 0;
	}

	twrNT.betaTof = betaH->GetBeta();
	twrNT.betaTof_Err = betaH->GetEBetaV();
	twrNT.betaSTof = betaH->GetBetaS();
	twrNT.betaCTof = betaH->GetBetaC();
	twrNT.betaCTof_Err = betaH->GetEBetaCV();
 	if (twrNT.isMC)
 	{
	  //errRet++; if (ev->nMCEventg() < 1) return errRet;
 		MCEventgR* mcev = ev->pMCEventg(0);
 		twrNT.qMC = mcev->Charge;
 		if (twrNT.qMC != 0.) twrNT.rigMC = (mcev->Momentum / twrNT.qMC);
 		else twrNT.rigMC = mcev->Momentum;
 		twrNT.mMC = mcev->Mass;

#ifdef _IS_MC_
/*		// TRD MC quantities
		{
			float lMass[20];
			for (int il=0; il<20; il++) lMass[il]=-1.;
			
			for (int itrd=0; itrd<ev->nTrdMCCluster(); itrd++)
			{
				TrdMCClusterR* trdmc=ev->pTrdMCCluster(itrd);
				int ilay = trdmc->Layer;
				int pNo = trdmc->ParticleNo;
				float thisM=(pNo<1 || pNo>50)?-1.:geantMass[pNo - 1];
				if (thisM>lMass[ilay] || (thisM==lMass[ilay] && trdmc->Ekin > twrNT.momTrdL_MC[ilay]))
				{
					lMass[ilay]=thisM;
					twrNT.momTrdL_MC[ilay]=trdmc->Ekin;
					twrNT.pidTrdL_MC[ilay]=pNo;
				}
			}
		}*/
		
		// Tracker MC quantities
		int iMCCl[9];
//		int tkidInt[9];
		int bestFitId = (twrNT.trFitCode >= 0) ? twrNT.trFitCode : twrNT.trFitCode_K;
		// Retrieve tracker ladder identifiers (TkId) for interpolated track
		for (int il=0; il<9; il++)
		{
			AMSPoint pt=trTr->InterpolateLayerJ(il+1,bestFitId);
			TkSens tks(pt, true);
//			tkidInt[il]=tks.GetLadTkID(); // 0 if track interpolation does not fall on a ladder			
			twrNT.tkId_Interpolated[il]=tks.GetLadTkID(); // 0 if track interpolation does not fall on a ladder
		}

		// Loop through MC tracker clusters to associate them with interpolated ladders
		for (int icl=0; icl<ev->nTrMCCluster(); icl++)
		{
			TrMCClusterR* trmc = ev->pTrMCCluster(icl);
			for (int il=0; il<9; il++)
			{
				// Skip the layers for which the interpolated track doesn't fall on a ladder
				if (twrNT.tkId_Interpolated[il] == 0) continue;
				
				int thisTkid = trmc->GetTkId();
				if (twrNT.tkId_Interpolated[il] == thisTkid)
				{
					float thisMom=fabs(trmc->GetMomentum()); // Is retaining the sign important?
					if (thisMom > twrNT.momTrL_MC[il]) // momTrL_MC has been initialized to zero
					{
						twrNT.momTrL_MC[il]=thisMom;
						twrNT.pidTrL_MC[il]=trmc->GetPart();
						twrNT.isPrimaryTrL_MC[il]=trmc->IsPrimary();
					}
				}
			} // il
		} // icl
		
		
/*
		{
			float lMass[9];
			for (int il=0; il<9; il++) lMass[il]=-1.;
			
			for (int itr=0; itr<ev->nTrMCCluster(); itr++)
			{
				TrMCClusterR* trmc=ev->pTrMCCluster(itr);
				int ilay = fabs(trmc->GetTkId()/100);
				if (ilay==8) ilay=1;
				else if (ilay<8) ilay++;
				ilay--;
				
				int pNo = trmc->GetPart();
				float thisM=(pNo<1 || pNo>50)?-1.:geantMass[pNo - 1];
				if (thisM>lMass[ilay] || (thisM==lMass[ilay] && trmc->GetMomentum() > twrNT.momTrdL_MC[ilay]))
				{
					lMass[ilay]=thisM;
					twrNT.momTrL_MC[ilay]=trmc->GetMomentum();
					twrNT.pidTrL_MC[ilay]=pNo;
					twrNT.isPrimaryTrL_MC[ilay]=trmc->IsPrimary();
				}
			}
		}
*/
		
		// TOF MC quantities
		{
			
		}
		
#endif

 	}
 	else
 	{
#ifndef _IS_MC_
 		twrNT.gtod_r = rti.r;
		twrNT.gtod_theta = rti.theta;
		twrNT.gtod_phi = rti.phi;
		twrNT.cutoff_Stoermer = float(par->GetGeoCutoff(ev));
#endif
 	}
	
	return 0;
}

int twrNTupleFiller::highestRigParticle(AMSEventR* ev, int& nPar_wTrack)
{
	double maxRig = -1.;
	int ret = -1;
	
	int nP = ev->nParticle();
	nPar_wTrack = 0;
	if (nP<1) return -1;
	for (int j=0; j<nP; j++)
	{
		ParticleR* par = ev->pParticle(j);
		int trInd = par->iTrTrack();
		if (trInd == -1) continue;
		nPar_wTrack++;
		TrTrackR* trTr = par->pTrTrack();
		
		int fitCode = trTr->iTrTrackPar(0,3,0);
		if (fitCode < 0) continue;
		double thisRig = std::abs(trTr->GetRigidity(fitCode));
		if (thisRig > maxRig)
		{
			maxRig=thisRig;
			ret=j;	
		}
	}
	return ret;
}

int twrNTupleFiller::RichQC(twrRichQuality *vals)
// Rich quality cuts, as provided by J. Berdugo
// Return 1 if object passes all cuts.  Return negative value of first cut not met otherwise.
{
	float cut_prob=0.01;						//  Kolmogorov test probability
	float cut_pmt=3;							//  number of pmts
	float cut_collovertotal=0.4;				//  ring photoelctrons / total photoelectrons in the event 
	float cut_chargeconsistency=5;			  //  hit/PMT charge consistency test
		// mdb: 5 represents some index indicating level of consistency (?)
	float cut_betaconsistency[2]={0.01,0.005};  //  beta_lip vs beta_ciemat consistency ([0]=NaF, [1]=aerogel) 
		// mdb: consistency of beta values found by methods of Portuguese (lip) and Spanish (ciemat) groups
	float cut_expphe[2]={1,2};				  //  expected number of photoelectrons   ([0]=NaF, [1]=aerogel)
		// mdb: There is a long, soft tail on this function, so it is plausible that we have 3 when only 1 is expected
	float cut_aerogelexternalborder=3500.;	  //  aerogel external border (r**2)
		// mdb: Cut particles that are on the outer edge of the aerogel, esp if they fall on small tiles (?)
	float cut_aerogel_nafborder[2]={17.,19.};   //  aerogel/NaF border				  ([0]=NaF, [1]=aerogel)
		// mdb: Also cut particles that fall close to the NaF/aerogel border
	
	int nbadtiles=5;							//  tiles with bad beta recosntruction
	int kbadtile[nbadtiles];
	kbadtile[0]=3;
	kbadtile[1]=7;
	kbadtile[2]=87;
	kbadtile[3]=100;
	kbadtile[4]=108;
	
	if(!vals->IsGood || !vals->IsClean) return -1;
	if(vals->prob < cut_prob) return -2;
	if(vals->PMTs < cut_pmt) return -3;
	if(vals->photoElectrons / vals->collectedPhotoElectrons < cut_collovertotal) return -4;
	if(vals->PMTChargeConsistency > cut_chargeconsistency) return -5;
	
	//  const float* TrackEmission=ring.getTrackEmissionPoint();
	float x=vals->trackEmPt[0];
	float y=vals->trackEmPt[1];
	
	if(vals->IsNaF)
	{
		if(vals->expectedPhotoElectrons < cut_expphe[0]) return -6;
		if(vals->betaConsistency > cut_betaconsistency[0]) return -7;
		if(max(abs(x),abs(y)) > cut_aerogel_nafborder[0]) return -8;
	}
	else
	{
		if(vals->expectedPhotoElectrons < cut_expphe[1]) return -9;
		if(vals->betaConsistency > cut_betaconsistency[1]) return -10;
		if(x*x+y*y			> cut_aerogelexternalborder) return -11;
		if(max(abs(x),abs(y)) < cut_aerogel_nafborder[1]) return -12;
		for(int kbad=0;kbad<nbadtiles;kbad++)
		{
			if(vals->tileIndex == kbadtile[kbad]) return -13;
		}
	}
	
	if(vals->richHit_PMTs - vals->richHit_PMTs_false > 1) return -14;
	
	// flagRich=-15 (set later) if low-rig. NaF event and inconsistent TOF beta
	
	return 1;
}


#ifdef _IS_MC_

// Find the TrMCCluster on a given tracker ladder (by TkId) with the highest momentum.
// Return the index of this TrMCCluster in the AMSEventR, or -1 if none found.
int associateTrMCCluster_TkId(AMSEventR* ev, int tkid)
{
//	int nFound=0;
	if (!ev) return -5;
	float highMom=0.;
	int ind=-2;
	for (int icl=0; icl<ev->nTrMCCluster(); icl++)
	{
		TrMCClusterR* trmc = ev->pTrMCCluster(icl);
		if (tkid == trmc->GetTkId())
		{
//			nFound++;
			float thisMom=fabs(trmc->GetMomentum());
			if (thisMom>highMom) {highMom=thisMom; ind=icl;}
		}
	}

	return ind;
}

// Find the TrMCCluster on the same TkId as a given point.
// If there are multiple, choose the one with the highest momentum.
// Return the index of this TrMCCluster in the event, -1 if none found,
// -2 if the AMSPoint does not fall on a ladder.
int associateTrMCCluster_Pt(AMSEventR* ev, AMSPoint pt)
{
	TkSens tks(pt, true);
	int tkid=tks.GetLadTkID();
	if (tkid==0) return -2;
	else return associateTrMCCluster_TkId(ev, tkid);
}

/*
void assocaiteAllTrCrossings(AMSEventR* ev, int fitID)
{
	int iMCCl[10];
	for (int jl=1; jl<=9; jl++)
	{
		AMSPoint pt=InterpolateLayerJ(jl,fitID);
		iMCCl[jl]=associateTrMCCluster_Pt(ev, pt);
	}
}
*/

#endif // _IS_MC_


