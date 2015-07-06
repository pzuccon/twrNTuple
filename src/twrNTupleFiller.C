#include "twrNTupleFiller.h"
#include "twrTrdK.h"
#include "TrdKCluster.h"


void FilltwrTrdKHit( twrTrdKHit* mhit,TrdKHit* hit){

  AMSPoint pnt;
  AMSDir dir;

  mhit->tube_length=hit->Tube_Track_3DLength(&pnt,&dir);
 

  
  mhit->Time=hit->Time;
  mhit->TRDHit_Amp=hit->TRDHit_Amp;
  mhit->TRDHit_GainCorrection=hit->TRDHit_GainCorrection;
  mhit->TRDHit_Layer=hit->TRDHit_Layer;
  mhit->TRDHit_Ladder=hit->TRDHit_Ladder;
  mhit->tubeid=hit->tubeid;
  mhit->TRDHit_x=hit->TRDHit_x;
  mhit->TRDHit_y=hit->TRDHit_y;
  mhit->TRDHit_z=hit->TRDHit_z;
  mhit->TRDHit_TRDTrack_x=hit->TRDHit_TRDTrack_x;
  mhit->TRDHit_TRDTrack_y=hit->TRDHit_TRDTrack_y;
  mhit->TRDHit_TRDTrack_z=hit->TRDHit_TRDTrack_z;

  for (int ii=0;ii<3;ii++){
    mhit->TRDTube_Center[ii]=hit->TRDTube_Center[ii];
    mhit->TRDTube_Dir[ii]=hit->TRDTube_Dir[ii];
  }

}


Bool_t FillTrdK(AMSEventR* pev,ParticleR* part, twrTrdK* tdk){

  
  TrdTrackR *trdtrack=pev->pTrdTrack(part->iTrdTrack());
  TrTrackR *trk=pev->pTrTrack(part->iTrTrack());
  if(trdtrack){
    tdk->ntrdseg=trdtrack->NTrdSegment();
    for (int ss=0;ss<tdk->ntrdseg;ss++)
      tdk->nlay+=trdtrack->pTrdSegment(ss)->NTrdCluster();
    //---TrdTrack Match
    AMSDir dir;AMSPoint pos;
    
    if(trk){
      trk->Interpolate(trdtrack->Coo[2],pos,dir);
      tdk->distrd[0]=trdtrack->Coo[0]-pos.x();
      tdk->distrd[1]=trdtrack->Coo[1]-pos.y();
      AMSDir trddir(trdtrack->Theta,trdtrack->Phi);
      tdk->distrd[2]=acos(fabs(trddir.prod(dir)))*180/3.1415926;
    }
  }

  //---TrdTrack-Default-Charge
  ChargeR *charge=pev->pCharge(part->iCharge());
  if(charge){
    ChargeSubDR *zTrd=charge->getSubD("AMSChargeTRD");
    if(zTrd){
      tdk->trd_rz = TMath::Max(Int_t(zTrd->ChargeI[0]),1);
      tdk->trd_rq = zTrd->Q;
      tdk->trd_rpz= zTrd->getProb();
    }
  }

 
  //-----TrdK
  //  for(int i=0;i<20;i++){trd_amplk[i]=trd_pathlk[i]=0;}


  if(!trk)return false;
  //---


  float ecalen=0;
  EcalShowerR *show= pev->pEcalShower(part->iEcalShower());
  if(show){ecalen=show->EnergyE;}
  bool ecalpar=(ecalen>1);


  //---
  //---Track
  tdk->fid=trk->iTrTrackPar(1,5,2); // Get any prefered fit code

  double trdlikr[3]={-1,-1,-1},trdlikre[3]={-1,-1,-1},trdlik[3]={-1,-1,-1}; //LikelihoodRatio :  e/P, e/H, P/H

  //int NHits=0;  //number of hits in Likelihood Calculation

  float threshold=15;  //ADC above will be taken into account in Likelihood Calculation

  TrdKCluster trdcluster=TrdKCluster(pev,trk,tdk->fid);

  tdk->IsReadAlignmentOK=trdcluster.IsReadAlignmentOK; //0: No Alignment, 1: Static Alignment Layer level,  2: Dynamic
  tdk->IsReadCalibOK=trdcluster.IsReadCalibOK;  // 0: No Gain Calibration, 1: Gain Calibration Succeeded

  if(tdk->IsReadAlignmentOK==0||tdk->IsReadCalibOK==0){tdk->trd_statk=-200;} 
  else                                      {tdk->trd_statk=-100;}
  if(tdk->trd_statk!=-100)return false;

  tdk->trd_statk=trdcluster.GetLikelihoodRatio_TrTrack(threshold,tdk->trd_likr,tdk->NHits);//Track=1
 
  trdcluster.GetOffTrackHit_TrTrack(tdk->trd_onhitk,tdk->trd_oampk);

  if(tdk->trd_statk!=1){
    tdk->trd_statk=trdcluster.GetLikelihoodRatio_TRDRefit(threshold,tdk->trd_likr,tdk->NHits);
    if(tdk->trd_statk==1)tdk->trd_statk+=10;//TRD=11
    trdcluster.GetOffTrackHit_TRDRefit(tdk->trd_onhitk,tdk->trd_oampk);
  }
  // trd_nhitk=NHits;//Number of surrounding fired tubes
  if(tdk->trd_statk%10==1){//trd_statk=1 || trd_statk=11
    if(ecalpar){
      if(tdk->trd_statk==1)trdcluster.GetLikelihoodRatio_TrTrack(threshold,trdlikre,tdk->NHits,ecalen,trdlik);
      else            trdcluster.GetLikelihoodRatio_TRDRefit(threshold,trdlikre,tdk->NHits,ecalen,trdlik);
    }
    for(int i=0;i<3;i++){tdk->trd_lik[i]=trdlikr[i];tdk->trd_likre[i]=trdlikre[i];}
    AMSPoint pnt; AMSDir dir;
    int x= trdcluster.GetTrTrackExtrapolation(pnt,dir);
    if(x>=0){
      for(int i=0;i<tdk->NHits;i++){
	TrdKHit *hit=trdcluster.GetHit(i);
	if(!hit)continue;
	twrTrdKHit mtdhit;
	FilltwrTrdKHit( &mtdhit,hit);
	tdk->hits.push_back(mtdhit);
      }
    }
  }
  ///TRD-Charge
    trdcluster.CalculateTRDCharge(0,part->pBetaH()->GetBeta());
    tdk->trd_qk[0]=   trdcluster.GetTRDCharge();
    tdk->trd_qk[1]=   trdcluster.GetTRDChargeUpper();
    tdk->trd_qk[2]=   trdcluster.GetTRDChargeLower();
    tdk->trd_qrmsk[0]=trdcluster.GetTRDChargeError();
    tdk->trd_qnhk[0]= trdcluster.GetQNHit();
    tdk->trd_qnhk[1]= trdcluster.GetQNHitUpper();
    tdk->trd_qnhk[2]= trdcluster.GetQNHitLower();
    tdk->trd_ipch=    trdcluster.GetIPChi2();//impact Chis

    ///--End Fragmentation delta
      trdcluster.CalculateTRDCharge(1,part->pBetaH()->GetBeta());
      tdk->trd_qk[3]=   trdcluster.GetTRDCharge();
      tdk->trd_qrmsk[1]=trdcluster.GetTRDChargeError();
      trdcluster.CalculateTRDCharge(2,part->pBetaH()->GetBeta());
      tdk->trd_qk[4]=   trdcluster.GetTRDCharge();
      tdk->trd_qrmsk[2]=trdcluster.GetTRDChargeError();



      return true;
}



Bool_t FillTrdK(AMSEventR* pev,ParticleR* part, twrTrdK* tdk);
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

// 	AMSSetupR::RTI::UseLatest(); // before pass6
	AMSSetupR::RTI::UseLatest(6); // pass6 onward
	
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
	  if (i%1000==0) printf("Processed %9d out of %9d(max %9d)  preselected %9d \n",i,nToUse,nEv,presel);
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



/*
int twrNTupleFiller::doPreselect(char* rootListFile, char* outNTupleFile)
{
	AMSChain ch;
	
	// Compose and display full output filename
	char outF[256];
	if (outNTupleFile == 0) sprintf(outF,"%stwr%s",outNTuplePath,rootListFile);
	else sprintf(outF,"%s%s",outNTuplePath,outNTupleFile);
	
	// Decide whether we're looking at a single data file or a text file containing a list of data files
	int addOK;
	bool addBool;
	if (isFileType(rootListFile,".root"))
	{
		char tmpF[256];
 		sprintf(tmpF,"%s%s",inDataPath,rootListFile);
 		addOK = ch.Add(tmpF);
		printf("Loading single AMS data file: %s\n  > Status=%d\n",rootListFile,addOK);
		addBool=(addOK>0)?true:false;
		printf("Status: %d | Number of entries in AMSChain: %d\n",addOK,ch.GetEntries());
	}
	else
	{
		addOK = ch.AddFromFile(rootListFile,0,nMaxToProcess,true);
		printf("Loading list of AMS ROOT files: %s\n  > Status=%d\n",rootListFile,addOK);
		addBool=(addOK==0)?true:false;
	}
//	int addOK = ch.Add("root://eosams.cern.ch//eos/ams/Data/AMS02/2011B/ISS.B620/pass4/1378000368.00000001.root");
	// TODO: Fix this
	if (!ch.GetEntries()) {printf("ERROR processing input data file(s)--ABORT doPreselect()\n"); return addOK;}
	printf("Number of entries loaded: %d\n",ch.GetEntries());
	
	// Open output file and tree for n-tuples
	TFile* tf = new TFile(outF, "RECREATE");
	if (!tf->IsOpen())
	{
		printf("ERROR opening output TFile:%s--ABORT doPreselect()\n",outF);
		if (tf) {delete tf; tf=0;}
		return -10;
	}
	
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
	int nToUse = (nMaxToProcess<nEv?nMaxToProcess:nEv);
	for (int i = 0; i < nToUse;i++)
	{
		if (i%5000==0) printf("Processed %9d out of %9d (preselect)\n",i,nToUse);
		AMSEventR* ev = ch.GetEvent(i);
		int procRet = fillNTuple_preselect(nt, ev);
//		printf("ev%d: %d\n",i,procRet);
		if (procRet==0) tt->Fill();
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
	printf("Output n-tuple file will be found at %s\n",outF);
//	hman.Save();

	return 0;
}

int twrNTupleFiller::doPreselect_separate_files(char* listFile, int nMaxFiles)
{
	ifstream ifs(listFile);
	if (!ifs.is_open()) {printf("ERROR opening list file\n"); return -20;}
	char line[30];
	//char fullRootFile[256];
	int ret1, retAll = 0, nf = 0;
	//	while ( (ifs.getline(line,30)) && (nf<nMaxFiles) )
	while ( (ifs.getline(line,30)) && ((nf<nMaxFiles) || (nMaxFiles<0)) )
	{
		nf++;
		//sprintf(fullRootFile,"%s%s",inDataPath,line);
		//ret1 = doPreselect(fullRootFile,outNTuplePath);
		ret1 = doPreselect(line);
		if (ret1 != 0) ++retAll;	
	}
	// Return the number of .root files that have failed (0=all successful)
	return retAll;
}
*/

int twrNTupleFiller::fillNTuple_preselect(twrNTuple &twrNT, AMSEventR* ev)
{  
  

  int hrPart = highestRigParticle(ev, twrNT.nParticle_withTrack);
  // Problem finding the particle with the highest (absolute) rigidity.
  //    (This includes the case that there are no particles with an associated track.)
  if (hrPart<0) return 6;
  
  twrNT.isMC = ((ev->nMCEventg()>0)?true:false);

	if (!twrNT.isMC)
	{
//		RTI cuts
		int rtiRet = ev->GetRTI(rti);
		if (rtiRet != 0) return 1;
		
		//printf("RTI Status: %d , livetime: %f\n",ev->GetRTIStat(),rti.lf);
		if (rti.lf < 0.7) return 2;
		if (rti.IsInSAA()) return 3;
		if ((rti.nhwerr/rti.ntrig)>0.01) return 4;
		if ((rti.ntrig/rti.nev)<0.99) return 5;
	}

	twrNT.fStatus=ev->fStatus;
	twrNT.Event=ev->Event();
	twrNT.Run=ev->Run();

	ParticleR* par = ev->pParticle(hrPart);

	TrTrackR* trTr = par->pTrTrack();
	
	// No XY hit on tracker layer 1
	//pz	if (!trTr->TestHitLayerJHasXY(1)) return 7;
	if (!trTr->TestHitLayerJ(1)) return 7;
	
	// Choutko algo. fit (1); inner tracker only (3); force refit (2);
	//   assume He4 mass; assume charge 2
	twrNT.trFitCode = trTr->iTrTrackPar(1,3,2,mHe4,2);
	// chiKanian algo. fit (3)
	twrNT.trFitCode_K = trTr->iTrTrackPar(3,3,2,mHe4,2);
	// Fit did not succeed.  (SHOULD NOT OCCUR)
	if ((twrNT.trFitCode < 0)&&(twrNT.trFitCode_K < 0)) return 8;
	
	twrNT.iBH=par->iBetaH();
	bool flag_betaH = (twrNT.iBH > -1);
	bool flag_richRing = (twrNT.iRichRing());
	// No source of beta
	if (!(flag_betaH || flag_richRing)) return 9;
//	if ((twrNT.iBH == -1)||(!twrNT.iBeta())) return 3;
//	if (!twrNT.iRichRing()) return 4;
	
//	TofRecH::ReBuild();
	BetaHR* betaH = par->pBetaH();
	if (!betaH || (betaH->GetBeta()<=0)) return 10;
	twrNT.betaHGood = betaH->IsGoodBeta();
	
	twrNT.chi2TBetaH = betaH->GetNormChi2T();
	twrNT.chi2CBetaH = betaH->GetNormChi2C();

	twrNT.qUTof = 0.5*(betaH->GetQL(0) + betaH->GetQL(1));
	// Upper TOF charge far from 2.0
	if ((twrNT.qUTof < 1.5)||(twrNT.qUTof > 3.0)) return 11;
	twrNT.qLTof = 0.5*(betaH->GetQL(2) + betaH->GetQL(3));
	// Lower TOF charge far from 2.0
	if ((twrNT.qLTof < 1.5)||(twrNT.qLTof > 3.0)) return 12;
	
	twrNT.qTrLayer1 = trTr->GetLayerJQ(1,betaH->GetBeta());
	// Tracker layer 1 charge far from 2.0
	if ((twrNT.qTrLayer1 < 1.5)||(twrNT.qTrLayer1 > 3.0)) return 13;
	
	if (ev->nLevel1() != 1) return 14;
	cloneLevel1R(twrNT.lvl1,ev->Level1(0));
	cloneRTI(twrNT.RTI,rti);

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
	FillTrdK(ev,ev->pParticle(0), &(twrNT.trdk));
 	if (twrNT.isMC)
 	{
	  //if (ev->nMCEventg() < 1) return 14;
 		MCEventgR* mcev = ev->pMCEventg(0);
 		twrNT.qMC = mcev->Charge;
 		if (twrNT.qMC != 0.) twrNT.rigMC = (mcev->Momentum / twrNT.qMC);
 		else twrNT.rigMC = mcev->Momentum;
 		twrNT.mMC = mcev->Mass;
 	}
 	else
 	{
 		twrNT.gtod_r = rti.r;
		twrNT.gtod_theta = rti.theta;
		twrNT.gtod_phi = rti.phi;
		twrNT.cutoff_Stoermer = float(par->GetGeoCutoff(ev));
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
