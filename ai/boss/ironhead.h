
#ifndef _IRONHEAD_H
#define _IRONHEAD_H

class IronheadBoss : public StageBoss
{
public:
	void OnMapEntry();
	void OnMapExit();
	void Run();

private:
	Object *o;
	int hittimer;
};


#endif
