#include "PopUps.h"
#include "Mode.h"
#include "Button.h"
using std::cout;

ConfirmPrompt::ConfirmPrompt(Mode* _mode, int _index, std::string _confirmText, std::function<void()> func): WindowLayer(_mode, _index) {
	mode = _mode;
	doAction = func;
	confirmText = _confirmText;
	cout << "\n confirm";
}

void ConfirmPrompt::start() {
	Program* program = mode->program;
	// Create buttons
	createNewButton("Cancel", "1/36", [this]() { this->onCancel(); });
	createNewButton("ConFirm", "1/36", [this]() { this->onConfirm(); });
	for (auto& b : buttons) {
		b.init();
	}

	//
	// Make design of this prompt
	// First create title of prompt
	SDL_Rect titleRect;
	SDL_Rect confirmTextRect;
	titleRect.x = 0;
	titleRect.y = 0;
	SDL_Color color = { 255,255,255, 255 };
	SDL_Surface* sTmp = TTF_RenderText_Blended(program->fonts[1], "Confirm", color);
	if (sTmp == NULL) {
		program->errorPrompt("Could not create surface for title for confirmPrompt");
		return;
	}
	SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(program->renderer, sTmp);
	SDL_SetTextureBlendMode(titleTexture, SDL_BLENDMODE_BLEND);
	if (titleTexture == NULL) {
		program->errorPrompt("Could not create texture for title for confirmPrompt");
		return;
	}
	SDL_FreeSurface(sTmp);

	// ConfirmText
	sTmp = TTF_RenderText_Blended(program->fonts[2], confirmText.c_str(), color);
	if (sTmp == NULL) {
		program->errorPrompt("Could not create surface for confirmText for confirmPrompt");
		return;
	}
	SDL_Texture* confirmTextTexture = SDL_CreateTextureFromSurface(program->renderer, sTmp);
	SDL_SetTextureBlendMode(confirmTextTexture, SDL_BLENDMODE_BLEND);

	if (confirmTextTexture == NULL) {
		program->errorPrompt("Could not create texture for title for confirmPrompt");
		return;
	}
	SDL_FreeSurface(sTmp);
	// Set size of prompt
	int  margin = 20;
	SDL_QueryTexture(confirmTextTexture, NULL, NULL, &confirmTextRect.w, &confirmTextRect.h);
	SDL_QueryTexture(titleTexture, NULL, NULL, &titleRect.w, &titleRect.h );
	
	if (confirmTextRect.w > titleRect.w)
		dstRect.w = confirmTextRect.w + margin*2;
	else
		dstRect.w = confirmTextRect.w + margin*2;


	dstRect.h = confirmTextRect.h + titleRect.h + margin * 3 + buttons[buttons.size()-1].dstRect.h;
	SDL_GetWindowSize(program->window, &dstRect.x, &dstRect.y);
	dstRect.x = dstRect.x / 2 - dstRect.w / 2 ;
	dstRect.y = dstRect.y /2 - dstRect.h/2  ;

	//Set size and pos of titleand confirmtext 
	titleRect.x = dstRect.w / 2 - titleRect.w / 2;
	confirmTextRect.x = margin;
	confirmTextRect.y = titleRect.h + margin;
	texture = SDL_CreateTexture(program->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, dstRect.w, dstRect.h);
	//SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(program->renderer, texture);
	SDL_RenderCopy(program->renderer, titleTexture, NULL, &titleRect );
	SDL_RenderCopy(program->renderer, confirmTextTexture, NULL,  &confirmTextRect);

	SDL_SetRenderTarget(program->renderer, NULL);

	SDL_DestroyTexture(titleTexture);
	SDL_DestroyTexture(confirmTextTexture);

	// Position buttons
	int index = 0;
	for (auto& b : buttons) {
		b.dstRect.x = dstRect.x + dstRect.w - b.dstRect.w - b.dstRect.w *index - margin*(index +1);
		b.dstRect.y = dstRect.y + dstRect.h - b.dstRect.h -margin/2;
		index++;
	}
}
void ConfirmPrompt::onCancel() {
	stop();
}
void ConfirmPrompt::onConfirm() {
	std::function<void()> f = doAction;
	stop();
	f();
}
void ConfirmPrompt::createNewButton(std::string _title,std::string _fontSize, std::function<void()> func) {
	buttons.emplace_back(_title, mode->program, _fontSize);
	buttons[buttons.size() - 1].doAction = func;
	
}
void ConfirmPrompt::stop() {
	for (auto& b : buttons) {
		SDL_DestroyTexture(b.texture);
		b.texture = nullptr;
	}
	buttons.clear();
	SDL_DestroyTexture(texture);
	//Delete and remove this popup from winLayers list 
	WindowLayer* ptr = mode->winLayers.at(index);
	mode->winLayers.erase(mode->winLayers.begin() + index);
	delete ptr;

}

void ConfirmPrompt::render() {
	Program* program = mode->program;
	
	if (texture != NULL) {
		SDL_RenderCopy(program->renderer, texture, NULL, &dstRect);
	}
	for (auto& b : buttons) {
		if (b.texture != NULL)
			SDL_RenderCopy(program->renderer, b.texture, &b.srcRect, &b.dstRect);
	}
}

void ConfirmPrompt::userEvents(SDL_Event e) {
	mode->handleBtns(&buttons, &e );
}