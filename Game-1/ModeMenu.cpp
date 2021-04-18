#include "ModeMenu.h"
#include "Mode.h"
#include "Button.h"

using std::cout;

ModeMenu::ModeMenu(Program* _program): Mode(_program) {

}
ModeMenu::~ModeMenu() {

}
void ModeMenu::start() {

	//Create buttons
	createNewButton("Editor",  "1/10", [this]() { this->goToEditor(); });
	createNewButton("Play",  "1/10", [this]() { this->goToPlay(); });
	createNewButton("Quit",  "1/10", [this]() { this->goQuit(); });

	//Positon buttons
	int totalBtnWidth = 0;
	for (auto& b : buttons) {
		totalBtnWidth += b.dstRect.w;
	}
	int ww, wh;
	SDL_GetWindowSize(program->window, &ww, &wh);
	int diff = ww - totalBtnWidth;
	diff = diff / (int)4;
	for (size_t i = 0; i < buttons.size(); i++) {
		if (i == 0)
			buttons[i].dstRect.x = diff;
		else
			buttons[i].dstRect.x = buttons[i - 1].dstRect.x + buttons[i - 1].dstRect.w + diff ;

		buttons[i].dstRect.y = wh / (int)2 - buttons[i].dstRect.h / (int)2;
	} 
}
void ModeMenu::stop() {
	for (auto b : buttons)
		SDL_DestroyTexture(b.texture);
	buttons.clear();
}
void ModeMenu::createNewButton(std::string title,  std::string fontSize, std::function<void()>fun )  {
	//Create and init new button
	buttons.emplace_back(title,  program, fontSize);
	buttons.at(buttons.size()-1).doAction = fun;
	buttons.at(buttons.size() - 1).init();

}
void ModeMenu::goQuit() {
	cout << "\nQUIT selected";
	program->keepRunning = false;
}
void ModeMenu::goToPlay() {
	cout << "\nPLAY selected";
}
void ModeMenu::goToEditor() {
	program->startMode(program->modeEditor);
}
void ModeMenu::userEvents(SDL_Event e) {
	// handle Buttons
	int  cursor = 0 ; // 0 = not changed, 1= changde to default cursor, 2 = changed to handcursor
	bool freeze = false;
	for (auto& btn : buttons)
		if (btn.status == 3) {
			freeze = true;
			break;
		}
 	for (auto &b : buttons) {
		int status = checkButtonStatus(&b, &e, freeze); // 0 = break, 1= changde to cursor, 2 = change to handcursor, 3 = unfreeze curspr
		if (status == 1 && cursor != 1)
			cursor = 1;
		else if (status == 2 ) {
			cursor = 2;
			break;
		}
	}
	if (cursor == 1)
		SDL_SetCursor(program->cursor);
	else if(cursor == 2)
		SDL_SetCursor(program->handCursor);

}

void ModeMenu::render() {
	SDL_RenderClear(program->renderer);
	for (auto &b : buttons) {
		if(b.initialized)
			SDL_RenderCopy(program->renderer, b.texture, &b.srcRect, &b.dstRect);
	}

	SDL_RenderPresent(program->renderer);
}