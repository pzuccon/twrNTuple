#ifndef twrRTI_h
#define twrRTI_h
#include "Rtypes.h"

class twrRTI {

public:
  
  //--------------------------------------------------------------------------------------------------
  ///
  //!   Says if AMS was inside SAA boundaries
  /*!
    returns: true if inside SAA, false if outside.
  */
  bool IsInSAA;				///< True if ISS in in SAA, false otherwize
  ///  BadRuns accessor
         /*!
           returns: 0 GoodRun, 1 BadRun, 2 UnableToLoadBadRunList         
         */
  //int IsBadRun(std::string &reason);
  
  unsigned int run;  ///< run
  unsigned int evno;///< first event no in one second
  unsigned int evnol;///< last event no in one second
  float lf;  ///< life time
  float cf[4][2];  ///< max cutoff for 25,30,35,40 degrees (gv) for neg+pos
  float cfi[4][2]; ///< max IGRF cutoff for 25,30,35,40 degrees (gv) for neg+pos
  float mphe;   ///< most probable He rigidity;
  float theta;  ///< theta gtod (rad)
  float phi;    ///< phi gtod (rad)
  float r;      ///< rad gtod (cm)
  float zenith; ///< ams zenith angle (degrees)
  float glat;   ///< ams pointing galatic latitude (degrees) -1 faild
  float glong;  ///< ams pointing galatic longitude (degrees) -1 faild
  float nev;    ///< exist events  nev+nerr=sumev
  float nerr;   ///< absent events
  float ntrig;  ///< events with trigger;
  float nhwerr; ///< events with has HW error(JINJStatus)
  float npart;  ///< events with tof+trd+tracker+ecal
  float nl1l9[2][2]; ///<events with track L1 L9 XY hit 
  float dl1l9[2][3]; ///< mean difference(um) bewteen PG ad CIEMAT alignment of L1 and L9(XYZ)
  float mtrdh; ///< average trdrawhit number for one event  
  int good;    ///< 0 if good
  /*!<
    bit0: duplicate events                          \n
    bit1: event number flip                         \n
    bit2: event missing at the beginging of second  \n
    bit3: event missing at the end of second        \n
    bit4: second at the begining of run             \n
    bit5: second at the end of run                  \n
  */
  unsigned int utime;  ///< JMDC unix time(second)
  unsigned int usec[2];///< JMDC unix time microsecond(us) for first and last event
  double utctime[2];   ///< UTC time for first and last event
  int Version; ///< RTI Version id
public:
  /// get unix time
  /*!
   * @param[in] itm  0: JMDC  time,  1: UTC  time,  2: JMDC time-UTC time
   * @param[in] iev  0: first event, 1: last event
   */
  double gettime(int itm=0, int iev=0); 
  //  float  getthetam(); ///< PhiM (rad)
  //  float  getphim();   ///< ThetaM(rad)
  // int    getissatt(float &roll,float &pitch,float &yaw);///<roll, pitch, yaw(rad)
  // int    getisstle(float &velocitys,float &veltheta,float &velphi);///< velocitys(rad/sec),velphi(rad),veltheta(rad)
  // double getbetasun();///< solar beta angle(degree)
  /*!< 
    0:  2013-01 B620 default \n
    1:  2013-08 B620         \n
    2:  2013-12 B700         \n
    3:  2014-03 B620 latest  \n 
    503:2014-03 B800(p5)  \n
  513:2014-12 B800(p5) (TTCS study) \n
  604:2015-03 B950(p6) latest \n
  */
 
public:
 
  //---
  twrRTI():run(0),evno(0),evnol(0),lf(0),mphe(0),theta(0),phi(0),r(0),zenith(0),glat(-2),glong(-2),nev(0),nerr(0),ntrig(0),nhwerr(0),npart(0),mtrdh(0),good(-1),utime(0){
    for(int ifv=0;ifv<4;ifv++){
      for(int ipn=0;ipn<2;ipn++){cf[ifv][ipn]=0;cfi[ifv][ipn]=0;}
    }
    for(int iexl=0;iexl<2;iexl++){
      for(int ico=0;ico<3;ico++){if(ico<2)nl1l9[iexl][ico]=0;dl1l9[iexl][ico]=0;}
    }
    usec[0]=usec[1]=0;
    utctime[0]=utctime[1]=0;
  }
  virtual ~twrRTI() { }
  ClassDef(twrRTI,1);
};
#endif
