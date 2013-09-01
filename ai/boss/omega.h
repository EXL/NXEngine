
#ifndef _OMEGA_H
#define _OMEGA_H


class OmegaBoss : public StageBoss
{
public:
	void OnMapEntry();
	void OnMapExit();
	
	void Run();

private:

	Object *pieces[4];
	
	struct
	{
		int timer;
		int animtimer;
		
		int movedir, movetime;
		int nextstate;
		
		int form;
		
		int firefreq, startfiring, stopfiring, endfirestate, shotxspd;
		int firecounter;
		
		int leg_descend;
		
		int orgx, orgy;
		
		int shaketimer;
		int lasthp;
		
		bool defeated;
	} omg;

};


#endif
