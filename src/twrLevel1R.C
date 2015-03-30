#include "twrLevel1R.h"

ClassImp(twrLevel1R)

/*
twrLevel1R::twrLevel1R(Trigger2LVL1 *ptr)
{
#ifndef __ROOTSHAREDLIBRARY__
	PhysBPatt = ptr->_PhysBPatt;
	JMembPatt = ptr->_JMembPatt;
	AuxTrigPatt = ptr->_AuxTrigPatt;
	TofFlag1 = ptr->_tofflag1;
	TofFlag2 = ptr->_tofflag2;
	for (int i=0; i<4; i++)
	{
		TofPatt1[i] = ptr->_tofpatt1[i];
		TofPatt2[i] = ptr->_tofpatt2[i];
	}
	AntiPatt = ptr->_antipatt;
	EcalFlag = ptr->_ecalflag;
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<3;j++)EcalPatt[i][j] = ptr->_ectrpatt[i][j];
	}
	EcalTrSum= ptr->_ectrsum;
	LiveTime   = ptr->_LiveTime;
	for(int i=0; i<19; i++)TrigRates[i]  = ptr->_TrigRates[i];
	for(unsigned int i=0; i<sizeof(TrigTime)/sizeof(TrigTime[0]); i++)
	{
		TrigTime[i]  = ptr->_TrigTime[i];
	}
#endif
}
*/

int twrLevel1R::GetGPSTime(unsigned int &sec, unsigned int &nsec)
{
	const unsigned long long ns=1000000000LL; 
	if(TrigTime[1]==0)return 6;
	unsigned long long gpstime=((unsigned long long)TrigTime[3]<<32);
	gpstime+=TrigTime[2];
	gpstime*=640;
	gpstime+=((unsigned long long)(sec+(TrigTime[1]-1)))*ns;
	sec=gpstime/ns;
	nsec=gpstime%ns ;
	return 0;
}

void  twrLevel1R::RestorePhysBPat(int mg)
{
	if( mg || PhysBPatt==0)
	{
		unsigned short int ft_pattern=JMembPatt;

		if ( (ft_pattern & 0x0001) == 0x0001 ) PhysBPatt |= 0x0001; // unbiased 3/4 TOF-CP
		if ( (ft_pattern & 0x0090) == 0x0090 ) PhysBPatt |= 0x0002; // 4/4 TOF-CT & ACC0
		if ( (ft_pattern & 0x0300) == 0x0300 ) PhysBPatt |= 0x0004; // 4/4 TOF-BZ & ACC1
		if ( (ft_pattern & 0x0020) == 0x0020 ) PhysBPatt |= 0x0008; // 4/4 FTZ
		if ( (ft_pattern & 0x0410) == 0x0410 ) PhysBPatt |= 0x0010; // 4/4 TOF-CT & ECALF&
		if ( (ft_pattern & 0x1000) == 0x1000 ) PhysBPatt |= 0x0020; // ECALA&
		if ( (ft_pattern & 0x0800) == 0x0800 ) PhysBPatt |= 0x0040; // unbiased ECALA||
	}
}

int twrLevel1R::RebuildTrigPatt(int &L1TrMemPatt,int &PhysTrPatt)
{
	//returns: two new trig-patterns and flag: 0/1 --> "Not"/"Was" PhysTrig
	L1TrMemPatt=0;
	PhysTrPatt=0;
	int tofpattft[4]={0,0,0,0};
	int AccPatt(0),NAccs(0);
	int TOFTrigFl1(-1),TOFTrigFl2(-1),ECTrigFl(-1);
	bool FTZ(0);

	int PhysTrSet[8]={0,0,0,0,0,0,0,0};
	PhysTrSet[0]=(1<<1);//ch_unb:FTCP0
	PhysTrSet[1]=(1<<4)+(1<<7);//singl_charged(not el):FTCT1&ACC0
	PhysTrSet[2]=(1<<8)+(1<<9);//ions:ACC1&BZ
	PhysTrSet[3]=(1<<5);//sl_ions:FTZ
	PhysTrSet[4]=(1<<4)+(1<<10);//el:FTCT1&EC-F_and
	PhysTrSet[5]=(1<<12);//ph:EC-A_and
	PhysTrSet[6]=(1<<11);//unbEC:EC-A_or

	//------> get current trig. parameters:
	for(int ii=0;ii<4;ii++)
	{
		tofpattft[ii]=TofPatt1[ii];
	}
	//  FTZ=((TrigPatt&(1<<5))!=0);//SlowIon setting is disabled to match with Data 
	AccPatt=AntiPatt;
	TOFTrigFl1=TofFlag1;
	TOFTrigFl2=TofFlag2;//bz
	ECTrigFl=EcalFlag;//MN, M=0/1/2/3->noFTE/noFTE(1prj@2requested)/FTEor/FTEand, 
	//                                N=0/1/2/3-> /undef/noLev1/Lev1or/Lev1and
	bool CentrOK=((tofpattft[2]&0x1FE01FEL)>0);//have any central counter in Lay-3 trig.patt
	//-----------------------------------------
	//------>FTC,FTCP0,FTCP1 rebuild:
	//
	if(TOFTrigFl1<=4 && TOFTrigFl1>=0)
	{
		L1TrMemPatt|=(1<<1);// set FTCP0(>=3of4)
		L1TrMemPatt|=1;// set FTC
		if(CentrOK)L1TrMemPatt|=(1<<3);// set FTCT0(not used in TrigDecision but let it be seen)
	}
	if(TOFTrigFl1==0)L1TrMemPatt|=(1<<2);// set FTCP1(4of4)
	//-----------------------------------------
	//------>FTCT1 rebuild:    
	//
	if(TOFTrigFl1==0 && CentrOK)
	{//was tof 4of4
		L1TrMemPatt|=(1<<4);// set FTCT1 
	}
	//-----------------------------------------
	//------> ACC0/1 rebuild:
	//
	for(int i=0;i<8;i++)if((AccPatt & (1<<i))>0)NAccs+=1;//count nsectors       
	if(NAccs<1)L1TrMemPatt|=(1<<7);// set ACC0
	if(NAccs<5)L1TrMemPatt|=(1<<8);// set ACC1
	//-----------------------------------------
	//------> FTZ,BZ-bits rebuild(no need to exclude out.c in L3):
	//
	if(FTZ)L1TrMemPatt|=(1<<5);// set FTZ as it was
	if(TOFTrigFl2==0)L1TrMemPatt|=(1<<9);// set BZ "4of4"
	//-----------------------------------------
	//------> EC-bits rebuild:
	//
	if((ECTrigFl/10)==3)
	{
		L1TrMemPatt|=(1<<10);// set EC-F_and
		L1TrMemPatt|=(1<<11);// set EC-F_or
	}  
	else if((ECTrigFl/10==1) || (ECTrigFl/10==2))L1TrMemPatt|=(1<<11);// set EC-F_or
	if(ECTrigFl%10>=2)
	{
		L1TrMemPatt|=(1<<13);// set EC-A_or (not used in TrigDecision but let it be seen)
		if(ECTrigFl%10==3)L1TrMemPatt|=(1<<12);// set EC-A_and
	}  
	if((L1TrMemPatt&(1<<11))>0)L1TrMemPatt|=(1<<6);// set FTE (EC-F_or is required)
	//-----------------------------------------
	//------>rebuild PhysTrigPatt:
	//
	for(int i=0;i<7;i++)
	{
		if((L1TrMemPatt&PhysTrSet[i])==PhysTrSet[i])PhysTrPatt|=(1<<i);
	}
	//-----------------------------------------
	if((PhysTrPatt&0x3EL)!=0)return 1;
	else return 0;
}

int twrLevel1R::RebuildTrigPatt(int &L1TrMemPatt,int &PhysTrPatt, int &AccSectPatt)
{
	//returns: two new trig-patterns and flag: 0/1 --> "Not"/"Was" PhysTrig
	L1TrMemPatt=0;
	PhysTrPatt=0;
	int tofpattft[4]={0,0,0,0};
	int AccPatt(0),NAccs(0);
	int TOFTrigFl1(-1),TOFTrigFl2(-1),ECTrigFl(-1);
	bool FTZ(0);

	int PhysTrSet[8]={0,0,0,0,0,0,0,0};
	PhysTrSet[0]=(1<<1);//ch_unb:FTCP0
	PhysTrSet[1]=(1<<4)+(1<<7);//singl_charged(not el):FTCT1&ACC0
	PhysTrSet[2]=(1<<8)+(1<<9);//ions:ACC1&BZ
	PhysTrSet[3]=(1<<5);//sl_ions:FTZ
	PhysTrSet[4]=(1<<4)+(1<<10);//el:FTCT1&EC-F_and
	PhysTrSet[5]=(1<<12);//ph:EC-A_and
	PhysTrSet[6]=(1<<11);//unbEC:EC-A_or

	//------> get current trig. parameters:
	for(int ii=0;ii<4;ii++)
	{
		tofpattft[ii]=TofPatt1[ii];
	}
	//  FTZ=((TrigPatt&(1<<5))!=0);//SlowIon setting is disabled to match with Data 
	AccSectPatt=AntiPatt;
	AccPatt=AntiPatt;
	TOFTrigFl1=TofFlag1;
	TOFTrigFl2=TofFlag2;//bz
	ECTrigFl=EcalFlag;//MN, M=0/1/2/3->noFTE/noFTE(1prj@2requested)/FTEor/FTEand, 
	//                                N=0/1/2/3-> /undef/noLev1/Lev1or/Lev1and
	bool CentrOK=((tofpattft[2]&0x1FE01FEL)>0);//have any central counter in Lay-3 trig.patt
	//-----------------------------------------
	//------>FTC,FTCP0,FTCP1 rebuild:
	//
	if(TOFTrigFl1<=4 && TOFTrigFl1>=0)
	{
		L1TrMemPatt|=(1<<1);// set FTCP0(>=3of4)
		L1TrMemPatt|=1;// set FTC
		if(CentrOK)L1TrMemPatt|=(1<<3);// set FTCT0(not used in TrigDecision but let it be seen)
	}
	if(TOFTrigFl1==0)L1TrMemPatt|=(1<<2);// set FTCP1(4of4)
	//-----------------------------------------
	//------>FTCT1 rebuild:    
	//
	if(TOFTrigFl1==0 && CentrOK)
	{//was tof 4of4
		L1TrMemPatt|=(1<<4);// set FTCT1 
	}
	//-----------------------------------------
	//------> ACC0/1 rebuild:
	//
	for(int i=0;i<8;i++)if((AccPatt & (1<<i))>0)NAccs+=1;//count nsectors       
	if(NAccs<1)L1TrMemPatt|=(1<<7);// set ACC0
	if(NAccs<5)L1TrMemPatt|=(1<<8);// set ACC1
	//-----------------------------------------
	//------> FTZ,BZ-bits rebuild(no need to exclude out.c in L3):
	//
	if(FTZ)L1TrMemPatt|=(1<<5);// set FTZ as it was
	if(TOFTrigFl2==0)L1TrMemPatt|=(1<<9);// set BZ "4of4"
	//-----------------------------------------
	//------> EC-bits rebuild:
	//
	if((ECTrigFl/10)==3)
	{
		L1TrMemPatt|=(1<<10);// set EC-F_and
		L1TrMemPatt|=(1<<11);// set EC-F_or
	}  
	else if((ECTrigFl/10==1) || (ECTrigFl/10==2))L1TrMemPatt|=(1<<11);// set EC-F_or
	if(ECTrigFl%10>=2)
	{
		L1TrMemPatt|=(1<<13);// set EC-A_or (not used in TrigDecision but let it be seen)
		if(ECTrigFl%10==3)L1TrMemPatt|=(1<<12);// set EC-A_and
	}  
	if((L1TrMemPatt&(1<<11))>0)L1TrMemPatt|=(1<<6);// set FTE (EC-F_or is required)
	//-----------------------------------------
	//------>rebuild PhysTrigPatt:
	//
	for(int i=0;i<7;i++)
	{
		if((L1TrMemPatt&PhysTrSet[i])==PhysTrSet[i])PhysTrPatt|=(1<<i);
	}
	//-----------------------------------------
	if((PhysTrPatt&0x3EL)!=0)return 1;
	else return 0;
}


char * twrLevel1R::Info(int number)
{
	RestorePhysBPat();
	int antif=0;
	for(int k=0;k<8;k++)
	{
		if(AntiPatt & (1<<(k)))
		{
			antif++;
		}
	}
	//
	int pat[8];
	for(int k=0;k<7;k++)
	{
		pat[k]=0;
		if(PhysBPatt & (1<<k))pat[k]=1;
	}
	//
	char toftyp[5],toftypz[5];
	if(TofFlag1==0)strcpy(toftyp,"4of4");
	else if(TofFlag1>0 && TofFlag1<5)strcpy(toftyp,"3of4");
	else if(TofFlag1>=5 && TofFlag1<8)strcpy(toftyp,"1t1b");
	else if(TofFlag1>=5 && TofFlag1<8)strcpy(toftyp,"1t1b");
	else if(TofFlag1==9)strcpy(toftyp,"2top");
	else if(TofFlag1==10)strcpy(toftyp,"2bot");
	else strcpy(toftyp,"unkn");
	//
	if(TofFlag2==0)strcpy(toftypz,"4of4");
	else if(TofFlag2>0 && TofFlag2<5)strcpy(toftypz,"3of4");
	else if(TofFlag2>=5 && TofFlag2<8)strcpy(toftypz,"1t1b");
	else if(TofFlag2>=5 && TofFlag2<8)strcpy(toftypz,"1t1b");
	else if(TofFlag2==9)strcpy(toftypz,"2top");
	else if(TofFlag2==10)strcpy(toftypz,"2bot");
	else strcpy(toftypz,"unkn");
	//
	double xtime=TrigTime[4]/1000.;

	char* _Info = new char[255];

	sprintf(_Info,"TrigLev1: TofZ>=1 %s, TofZ>1 %s, EcalFT  %s, EcalLev1 %d,  TimeD[ms]%6.2f LiveTime%6.2f, PhysTr=|uTf:%d|Z>=1:%d|Ion:%d|SIon:%d|e:%d|ph:%d|uEc:%d|",toftyp,toftypz,IsEcalFtrigOK()?"Yes":"No",EcalFlag,xtime,LiveTime,pat[0],pat[1],pat[2],pat[3],pat[4],pat[5],pat[6]);
	return _Info;
}

bool twrLevel1R::isTOFTrigger(int level)
{
	for(int i=1;i<5;i++)
	{
		if ((PhysBPatt & (1<<i))) return true;
	}
	return false;
}


