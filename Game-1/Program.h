#ifndef  Program_h
#define Program_h
#include "Includes.h"
struct Mode;
struct Inputs;
struct Program {
	SDL_Cursor* cursor;
	SDL_Cursor* handCursor;
	bool keepRunning = true;
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<TTF_Font*> fonts;
	Mode* modeMenu = nullptr;
	Mode* modeEditor = nullptr;
	Mode* modePlay = nullptr;
	Mode* currentMode = nullptr;
	Inputs* inputs;
	Program();
	void run();
	bool initSDL();
	bool initAssets();
	void errorPrompt(std::string);
	void startMode(Mode*);
	void getRGBAmask(Uint32*, Uint32*, Uint32*, Uint32*);
};

#endif // ! Program_h

