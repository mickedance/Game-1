#include "Program.h"
#include "Mode.h"
#include "ModeMenu.h"
#include "ModeEditor.h"
#include "Inputs.h"
#include "HandleLevel.h"
using std::cout;

Program::Program() {

};

bool Program::initAssets() {
	//FONTS
	if (TTF_Init() == -1) {
		errorPrompt("Could not initialize ttf");
		return false;
	}
	int h;
	SDL_GetWindowSize(window, nullptr, &h);
	fonts.emplace_back(TTF_OpenFont("assets/ShipporiMinchoB1-Medium.ttf", h/(int)10 ) );
	fonts.emplace_back(TTF_OpenFont("assets/ShipporiMinchoB1-Medium.ttf", h/(int)26 ) );
	fonts.emplace_back(TTF_OpenFont("assets/Roboto-Thin.ttf", h/(int)35 ) );
	
	for (size_t i = 0; i < fonts.size(); i++) {
		if (fonts.at(i) == NULL) {
			errorPrompt("Could not load font (index: " +std::to_string(i)+ ")" );
			return false;
		}
	}
	// CURSORS
	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	if (cursor == NULL) {
		errorPrompt("Could not create arrow cursor");
		return false;
	}
	handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	if (handCursor == NULL) {
		errorPrompt("could not create hand cursor" );
		return false;
	}
	ibeamCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	if (ibeamCursor == NULL) {
		errorPrompt("Could not create ibeam cursor");
		return false;
	}
	return true;
}
bool Program::initSDL() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << "\n could not initialize SDL";
		return false;
	}
	window = SDL_CreateWindow("Game-1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, NULL);
	if (window == NULL) {
		cout << "\n could not create SDL window";
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL) {
		errorPrompt("Could not create SDL renderer");
		return false;
	}

	return true;
}

void Program::getRGBAmask(Uint32* rmask, Uint32* gmask, Uint32* bmask, Uint32* amask) {
	// return mask for byteorder
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	*rmask = 0x000000ff;
	*gmask = 0x0000ff00;
	*bmask = 0x00ff0000;
	*amask = 0xff000000;
#endif
}
void Program::run() {

	if (!initSDL())
		return;
	if (!initAssets())
		return;

	//Handle Level objects and player
	handleLevel = new HandleLevel(this);
	//Modes
	modeMenu = new ModeMenu(this);
	modeEditor = new ModeEditor(this);
	startMode(modeMenu);
	inputs = new Inputs();

	while (keepRunning) {

		if (!inputs->handleEvents(currentMode))
			keepRunning = false;

		currentMode->render();
		SDL_Delay(20);
	}

	for (auto f : fonts)
		TTF_CloseFont(f);
	TTF_Quit();
	SDL_Quit();
	delete modeMenu;
	delete modeEditor;
	delete handleLevel;
}
void Program::errorPrompt(std::string message) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR!", message.c_str(), window);
	keepRunning = false;
}

void Program::startMode(Mode* mode) {

	if (currentMode != nullptr)
		currentMode->stop();
	currentMode = mode;
	currentMode->start();
}