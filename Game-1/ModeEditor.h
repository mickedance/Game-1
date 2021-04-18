#ifndef  ModeEditor_h
#define ModeEditor_h

#include "Includes.h"
#include "Mode.h"

struct Program;
struct Button;

struct ModeEditor : public Mode {
	std::vector<Button> buttons;
	ModeEditor(Program*);
	virtual void start();
	virtual void render();
	virtual void userEvents(SDL_Event);
	virtual void stop();

	void createNewButton(std::string title, int x, int y, std::string fontSize, std::function<void()>);
};


#endif // ! ModeEditor_h
