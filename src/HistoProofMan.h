//Author P. Zuccon -- MIT
#ifndef HistoProofMan_h
#define HistoProofMan_h
#include "TObject.h"
#include "hashtable.h"
#include "TH1.h"
#include <string>
#include <vector>

class Fthrep{
public:
  enum hkinds{knull,kth1f,kth2f,ktprofile,kth1d,kth2d,ktprofile2d,kth3f,kth3d};
  hkinds htype;
  std::string name;
  std::string title;
  std::string opt;
  int nbin[3];
  std::vector<double> xlim;
  std::vector<double> ylim;
  std::vector<double> zlim;
public:
  int getdimension();
  hkinds getkind(const TH1* hin);
  
public:
  Fthrep();
  Fthrep(const TH1* hin);
  Fthrep(const Fthrep& orig);
  TH1* gethisto() const;
  
  virtual ~Fthrep(){}
  Fthrep& operator=(const Fthrep& orig);

  ClassDef(Fthrep,0);

};

class HistoProofManF: public TObject{
public:
  hashtable<Fthrep> rlist;
  hashtable<TH1*> hlist;
  char fname[200];  

public:
  HistoProofManF(const char * nam="HistoOut.root"){sprintf(fname,"%s",nam);}
  ~HistoProofManF();
  /// Add a new histogram
  void Add(TH1* hist );
  void Fill(const char * name, double a,double  b=1.,double w=1.);
  void SetOutputName(const char* ff){sprintf(fname,"%s",ff);}
  const char* GetOutputName(){return fname;}
  void Save();
  
  TH1* Get(const char * name);
  const Fthrep* GetRep(const char* nn);  
  void AddH(TH1* hist ){hlist.Add(hist->GetName(),hist);}
  TH1* GetNew(const char * name);
  

  void CreateHistos();
  

  
  ClassDef(HistoProofManF,0);
};

#endif


