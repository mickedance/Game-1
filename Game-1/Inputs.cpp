#include "Inputs.h"
#include "Mode.h"
#include "Includes.h"
Inputs::Inputs() {

}

bool Inputs::handleEvents(Mode* mode) {

	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT)
			return false;
		mode->userEvents(e);
	}
	return true;
}