#ifndef Mode_h
#define Mode_h
#include "Includes.h"
struct Program;
struct Button;
struct WindowLayer;
struct Mode {
	Program* program;
	Mode(Program*);
	std::vector<WindowLayer*> winLayers;
	virtual void start() =0;
	virtual void stop() =0;
	virtual void render() =0;
	virtual void userEvents(SDL_Event) =0;
	int checkButtonStatus(Button*, SDL_Event*, bool);
	void handleBtns(std::vector<Button>*, SDL_Event*);
};

#endif // !Mode_h

