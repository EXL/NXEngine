
#ifndef _ALMOND_H
#define _ALMOND_H

// states to control the water-level object
#define WL_CALM				10			// calm and slow at set point

#define WL_CYCLE			20			// cycles between set point and top of screen
#define WL_DOWN				21			// in cycle--currently down
#define WL_UP				22			// in cycle--currently up

#define WL_STAY_UP			30			// goes to top of screen and doesn't come back down

#endif
