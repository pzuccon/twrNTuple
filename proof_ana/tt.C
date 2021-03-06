#define tt_cxx
// The class definition in tt.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("tt.C")
// Root > T->Process("tt.C","some options")
// Root > T->Process("tt.C+")
//

#include "tt.h"
#include <TH2.h>
#include <TStyle.h>




tt::tt(TTree * /*tree*/) : fChain(0),twrNT(0) { 

  BookHistos();

}



void tt::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   printf("INIT CLASS NAME %s\n",fChain->ClassName());
   twrNT=0;
   fChain->SetBranchAddress("twrNT",&twrNT);
   fChain->GetEntry(0);
  //  fChain->SetMakeClass(1);
//    twrNT=((myChain*)fChain)->GetEventPointer();


}

Bool_t tt::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

  
//    TString fn(fChain->GetCurrentFile()->GetName());
  TString fn("MMMMMMM");
    Info("Notify", "processing file: %s", fn.Data());
 return kTRUE;
}


void tt::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   if(option.Length()>0)
     hman.SetOutputName(option.Data());
   Info("Begin", "starting a simple exercise with process option: %s", option.Data());
}

void tt::SlaveBegin(TTree * /*tree*/)
{
  printf("---------------------->QUAQUA\n");
 // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   Info("SlaveBegin", "'%s' reading", "full");
  
   hman.CreateHistos();
   printf("B ------>  %d %d\n", hman.hlist.size(),hman.rlist.size());
   hashtable<TH1*>::iterator it;
   for( it=hman.hlist.begin();it!=hman.hlist.end();it++){
     TH1* hh=it->second;
     printf(" %x %s\n",hh,hh->ClassName());
     fOutput->Add(it->second);
   }
   
   
}

Bool_t tt::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either tt::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
  //   printf("QUAQUA\n");
  //  if (fEntMin == -1 || entry < fEntMin) fEntMin = entry;
//    if (fEntMax == -1 || entry > fEntMax) fEntMax = entry;

//    if (fTestAbort == 1) {
//       Double_t rr = gRandom->Rndm();
//       if (rr > 0.999) {
//          Info("Process", "%lld -> %f", entry, rr); 
//          Abort("Testing file abortion", kAbortFile);
//          return kTRUE;
//       }
//   }
 fChain->GetTree()->GetEntry(entry);
 hman.Fill("rrTrTrack",log10(twrNT->rigInnerTr));
 // ProcessEvent();

 
 return kTRUE;
}

void tt::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void tt::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
  //fPzHist->Draw();
  TIter hit(fOutput);
  TH1* hh;
  while ( hh=(TH1*) hit()){
    printf(" %s\n",hh->GetName());
    if(hman.GetRep(hh->GetName())) hman.AddH(hh);
  }
  printf(" ------>  %d %d\n", hman.hlist.size(),hman.rlist.size());
  hman.Save();
}
//===================================================================================
//===================================================================================
//===================================================================================

void tt::BookHistos(){
 hman.Add(new TH1F("rrTrTrack","Rig Tracks",50,-1,4));
  hman.Add(new TH2F("rr","rr",10,0,10.,100,0,1000));
}


void tt::ProcessEvent(){

 
}
