#ifndef twrLevel1R_h
#define twrLevel1R_h

#include "Rtypes.h"

class twrLevel1R 
{
//private:
//	static char _Info[255];
public:
	bool IsEcalFtrigOK()const {return (EcalFlag/10)>=2;}
	bool IsEcalLev1OK()const {return (EcalFlag%10)>1;}
public:
	int   PhysBPatt;   ///< 8 lsbits-> pattern of LVL1 sub-triggers ("predefined physics" branches)
	/*!<        LVL1 is (masked) OR of above branches.
	The list of predefined branches(preliminary):                                         \n                    \n
	bit1: unbiased TOF-trig(i.e. FTC= z>=1)  \n
	bit2: Z>=1(FTC+anti) "protons"           \n
	bit3: Z>=2(FTC & BZ) ions                \n
	bit4: SlowZ>=2 (FTZ) slow ions           \n
	bit5: electrons(FTC & FTE)               \n
	bit6: gammas(FTE & ECLVL1("shower angle")\n
	bit7: unbECAL(FTE)                       \n
	bit8: External                           \n
	*/
	int   JMembPatt; ///< 16 lsbits -> pattern of trig.system members checked by particular physics branch decision logic.
	/*!<        The bits assignment :                              \n
	bit0:  FTC (TOF FT z>=1)                                   \n
	bit1:  LUT-1 decision (used by FTC as OR with LUT-2)       \n
	bit2:  LUT-2 .......  (used by FTC as OR with LUT-1)       \n
	bit3:  same as  LUT-1, but for subset of central counters  \n
	bit4:  same as  LUT-2, but for subset of central counters  \n
	bit5:  FTZ (TOF SlowFT z>=2)                               \n
	bit6:  FTE (ECAL FT, OR or AND of desisions in 2 proj.)    \n
	bit7:  ACC0 (bit set if Nacc < Nthr_1)                     \n
	bit8:  ACC1 (bit set if Nacc < Nthr_2)                     \n
	bit9:  BZ-flag (LUT-3 decision on z>=2 4-layers config.)   \n
	bit10: ECAL FT using AND of 2 projections decisions        \n
	bit11: ............. OR ...........................        \n
	bit12: ECAL LVL1("shower angle") using AND ........        \n
	bit13: ECAL LVL1...................... OR .........        \n
	bit14: EXT-trigger_1                                       \n
	bit15: EXT-trigger_2                                       \n
	*/
	int   AuxTrigPatt;///< 5 lsbits-> pattern of Aux.trig.members(LA-0/LA-1/Reserv/DSP/InternTrigger)
	int   TofFlag1;   ///< FTC(z>=1) LayersPatternCode, <0:noFTC,=0:4L,(1-4):1missLnumb,5:1+3,6:1+4,7:2+3,8:2+4,9:1+2,10:3+4,(11-14):1,..4
	int   TofFlag2;   ///< BZ(z>=2) LayersPatternCode, <0:noBZ,=0:4L,(1-4):1missLnumb,..... as above
	int   TofPatt1[4]; ///< 4-layers TOF paddles pattern for FTC(z>=1)(separately for each side)
	int   TofPatt2[4]; ///< the same for BZ(z>=2)(separately for each side):

	/*!<
									   1-10 bits  Side-1  \n
									   17-26      Side-2  \n
	*/
	int   AntiPatt;   ///< Antipatt:(1-8)bits->sectors in coincidence with FastTrigger
	int   EcalFlag;   ///< =MN, where
	/*!<
	M=0/1/2/3->FTE(energ=multipl_High):no/no_in1prj@2prj-requir/FTE&in1proj/FTE&in2proj) \n
		  N=0/1/2/3->LVL1(ShowerAngleSmall):Unknown/in0proj_whenFTEok/in1proj/in2proj       \n
	*/
	unsigned short int EcalPatt[6][3];///< EC DynodesPattern for 6 "trigger"-SupLayers(36dynodes use 36of(3x16)lsbits)
	float EcalTrSum; ///< EC-energy trig.sum(Gev, MC only)
	float LiveTime;  ///< Fraction of "nonBusy" time;  May be 0 at the bef of run using  AMSEventR::LiveTime() isrecommended instead
	float TrigRates[19]; ///< TrigCompRates(Hz):FT,FTC,FTZ,FTE,NonPH,LVL1,L1M1-M8,CPmx,BZmx,ACmx,EFTmx,EANmx
	unsigned int TrigTime[5];///< [0]-Tcalib.counter,[1]-Treset.counter,[2]-[3]-0.64mks Tcounter(32lsb+8msb), [4]-time_diff in mksec

	int GetGPSTime(unsigned int & gps_sec, unsigned int & gps_nsec) ; // return 0 if success

	twrLevel1R(){};
  //	void cloneLevel1R(const Level1R &copyMe);
	/// \param level -1 any tof level
	///  \return true if any tof related level1 was set
	bool isTOFTrigger(int level=-1);
	/// \param mg !=0 restore pattern in anycase; physbpat==0  otherwise 
	void RestorePhysBPat( int mg=0 );

	int RebuildTrigPatt(int &L1TrMemPatt,int &PhysTrPatt);///< Rebuid JMembPatt/PhysBPatt according to Flight tr.setup(mainly for old MC root-files)
	/// \return 1/0 (was physical/unbiased trigger)
	int RebuildTrigPatt(int &L1TrMemPatt,int &PhysTrPatt,int &AccPatt);///< Rebuid JMembPatt/PhysBPatt, also return ACC-sectors pattern (AccPat 8 lsb)
	/// \return 1/0 (was physical/unbiased trigger)

//	twrLevel1R(Trigger2LVL1 *ptr);
	/// \param number index in container
	/// \return human readable info about twrLevel1R
	char * Info(int number=-1);



	~twrLevel1R(){};
  ClassDef(twrLevel1R,9);       //twrLevel1R
//#pragma omp threadprivate(fgIsA)
};

#endif /* twrLevel1R_h */
