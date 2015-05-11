#include "twrRTI.h"



double twrRTI::gettime(int itm, int iev){
   
  double xtime=0;
  if     (itm==0)xtime=double(utime)+double(usec[iev])/1000000.;
  else if(itm==1)xtime=utctime[iev];
  else if(itm==2)xtime=double(utime)+double(usec[iev])/1000000.-utctime[iev];
  return xtime;
}

