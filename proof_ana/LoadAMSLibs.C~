{
  gROOT->Reset();
  
  // Set include path
  //	  gInterpreter->AddIncludePath(gSystem->ExpandPathName("$AMSSRC/include"));
  TProof* p=TProof::Open("");
  p->Load("../lib/libResTemp.so");
  gInterpreter->AddIncludePath(gSystem->ExpandPathName("../src/"));
  // for loading AMSRoot libraries
  TChain aa("tt");
  aa.Add("/data2/DATA/twr_ntuples/v4/Data/mdb13*root");
  aa.SetProof();
  aa.Process("myselector.C+","HistoOutBS2.root");
  //gROOT->ProcessLine(".L BHist.C+"); 
  
  //gROOT->ProcessLine(".L DHF.C+"); 
}
