
#ifndef _HEAVYPRESS_H
#define _HEAVYPRESS_H


class HeavyPress : public StageBoss
{
public:
	void OnMapEntry();
	void Run();

private:
	void run_defeated();
	void run_passageway();

	Object *o;
	Object *shield_left, *shield_right;
	
	int uncover_left, uncover_right;
	int uncover_y;
	
	SIFRect fullwidth_bbox;
	SIFRect center_bbox;
};


#endif
