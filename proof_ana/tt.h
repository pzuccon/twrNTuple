//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Apr  8 17:03:49 2015 by ROOT version 5.34/09
// from TChain tt/
//////////////////////////////////////////////////////////

#ifndef tt_h
#define tt_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include "HistoProofMan.h"
#include <TSelector.h>
#include "TH1F.h"
#include "TH2F.h"
// Header file for the classes stored in the TTree if any.
#include "twrNTuple.h"
#include <TObject.h>

// Fixed size dimensions of array or collections stored in the TTree if any.

class tt : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
//
  HistoProofManF hman;
   // Declaration of leaf types
 twrNTuple       *twrNT;


  tt(TTree * /*tree*/ =0);
  virtual ~tt() { twrNT=0;fChain=0;}
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
  virtual void BookHistos();
  virtual void ProcessEvent();
   ClassDef(tt,0);
};

#endif


