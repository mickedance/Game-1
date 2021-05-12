#ifndef HandleLevel_h
#define HandleLevel_h
#include "Includes.h"
struct Program;

struct HandleLevel {
	bool initialized = false;
	std::vector<SDL_Texture*> BGTextures;
	std::vector<SDL_Texture*> OBSTextures;
	Program* program;
	HandleLevel(Program*);
	bool init();
	void render();
	void updateObjects();
	void updatePlayer();
	void loadTextures();
	void loadTexture(std::string, std::string, std::vector<SDL_Texture*>* );
};

#endif // !HandleLevel_h
