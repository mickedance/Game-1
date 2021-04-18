#ifndef  WindowLayer_h
#define WindowLayer_h
#include "Includes.h"

struct Mode;
struct WindowLayer {
	Mode* mode;
	int index = 0;
	virtual void start() =0;
	virtual void stop()=0;
	virtual void render() = 0;
	virtual void userEvents(SDL_Event)=0;
	WindowLayer(Mode*, int);
	~WindowLayer() {
		std::cout << "\n -destrotun winlayer";
	}
};

#endif // ! WindowLayer_h
