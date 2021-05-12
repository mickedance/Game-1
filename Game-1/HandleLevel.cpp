#include "HandleLevel.h"
#include "Program.h"

using std::cout;
HandleLevel::HandleLevel(Program* _program) {
	program = _program;
}
void HandleLevel::loadTextures() {
	std::vector<std::string> filenames;

	//BG
	std::string path = "pics/bg/";
	filenames.emplace_back("bg0.png");
	filenames.emplace_back("bg1.png");
	filenames.emplace_back("bg2.png");
	filenames.emplace_back("bg3.png");
	filenames.emplace_back("bg4.png");
	filenames.emplace_back("bg4.png");
	filenames.emplace_back("bg2.png");
	filenames.emplace_back("bg4.png");
	for (auto f : filenames) {
		loadTexture(f, path, &BGTextures);
	}
	filenames.clear();

	//OBSTACLES
	path = "pics/obs/";
	filenames.emplace_back("obs0.png");
	filenames.emplace_back("obs1.png");
	filenames.emplace_back("obs2.png");
	filenames.emplace_back("obs2.png");
	for (auto f : filenames) {
		loadTexture(f, path, &OBSTextures);
	}
}
void HandleLevel::loadTexture(std::string file, std::string path, std::vector<SDL_Texture*> *textures) {
	std::string filepath = path + file;
	SDL_Surface* s = IMG_Load(filepath.c_str()) ;
	if (s == NULL) {
		program->errorPrompt("Could not load surface of: "+ file);
		return;
	}
	textures->emplace_back(SDL_CreateTextureFromSurface(program->renderer, s));
	if (&textures[textures->size() - 1] == NULL) {
		program->errorPrompt("Could not create texture for: " + file);
		return;
	}

}
bool HandleLevel::init() {

	loadTextures();

	initialized = true;
	cout << "\ninit";
	return true;
}

void HandleLevel::updateObjects() {

}

void HandleLevel::updatePlayer() {

}

void HandleLevel::render() {

}