#include "ModeEditor.h"
#include "Button.h"
#include "PopUps.h"

using std::cout;

ModeEditor::ModeEditor(Program* _program): Mode(_program) {
	cout << "\n new mode editor";
}

void ModeEditor::start() {
	cout << "\nstarting Editor mode";
}
void ModeEditor::stop() {
	cout << "\nsotp";

	for (auto& win : winLayers) {
		win->stop();
		delete win;
	}
	winLayers.clear();
}
void ModeEditor::render() {

	SDL_RenderClear(program->renderer);
	SDL_SetRenderDrawColor(program->renderer, 255, 55,55,255);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	SDL_GetWindowSize(program->window, &rect.w, &rect.h);
	SDL_RenderFillRect(program->renderer, &rect);

	for (auto &win : winLayers) {
		win->render();
	}
	SDL_RenderPresent(program->renderer);
}

void ModeEditor::userEvents(SDL_Event e) {
	if (e.type == SDL_KEYDOWN) {
		if (e.key.keysym.sym == SDLK_ESCAPE) {
			program->startMode(program->modeMenu);
		}
	}
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (winLayers.size() == 0) {
			LoadPopUp* popup = new LoadPopUp(this, winLayers.size());
			popup->start();
			popup->createLoadBtn();
			popup->initBtns();
			WindowLayer* w = popup;

			winLayers.emplace_back(w);

		}
	}
	if(winLayers.size()>0)
		for (int i =winLayers.size()-1; i >= 0; i--) {
			winLayers[0]->userEvents(e);
		}

}