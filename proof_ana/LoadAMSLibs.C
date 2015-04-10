{
  gROOT->Reset();
  
  // Set include path
  //	  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSSRC/include"));
  TProof* p=TProof::Open("");
  p->Load("../lib/libResTemp.so");
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("../src/"));
  // for loading AMSRoot libraries
  TChain aa("tt");
  aa.Add("/data1/DATA/twr_ntuples/v3/Data/mdb138*root");
  aa.SetProof();
  //aa.Process("ProofEventProc.C+","",10000);
  //gROOT->ProcessLine(".L BHist.C+"); 
  
  //gROOT->ProcessLine(".L DHF.C+"); 
}
