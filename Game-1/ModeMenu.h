#ifndef  ModeMenu_h
#define ModeMenu_h
#include "Includes.h"
#include "Mode.h"

struct Program;
struct Button;

struct ModeMenu : public Mode {
	std::vector<Button> buttons;
	ModeMenu(Program*);
	~ModeMenu();
	virtual void start();
	virtual void render();
	virtual void userEvents(SDL_Event);
	virtual void stop();
	void goToEditor();
	void goToPlay();
	void goQuit();
	void createNewButton(std::string title, std::string fontSize, std::function<void()>);
};
#endif // ! ModeMenu_h
