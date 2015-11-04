#ifndef twrMC_h
#define twrMC_h

class twrMC
{
public:

	// Default values indicate that this instance of the class has not been
	// filled and should not be used.
	twrMC()
	{
		Clear();
	}
	~twrMC() {}
	twrMC(const twrMC& orig);

	void Clear();

// === MC quantities ===
	float qMC, rigMC, mMC; // True values of Q,R,m for incoming particle
	int pid;

// === Derived quantities ===	
	float betaMC(); // True beta of incoming particle, from other MC quantities

  ClassDef(twrMC,1);
};

#endif
