
#ifndef _COREBOSS_H
#define _COREBOSS_H

class CoreBoss : public StageBoss
{
public:
	void OnMapEntry();
	void OnMapExit();
	void Run();

private:
	void RunOpenMouth();
	
	void StartWaterStream(void);
	void StopWaterStream(void);
	
	Object *o;
	Object *pieces[8];
	int hittimer;
};


#endif
