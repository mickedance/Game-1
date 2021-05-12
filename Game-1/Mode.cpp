#include "Mode.h"
#include "Program.h"
#include "Button.h"
#include "WindowLayer.h"

using std::cout;
Mode::Mode(Program* _program) {
	program = _program;
}
int Mode::handleBtns(std::vector<Button>* buttons, SDL_Event* e) {
	// handle Buttons
	int  cursor = 0; // 0 = not changed, 1= changde to default cursor, 2 = changed to handcursor
	bool freeze = false;
	int returnValue = -1;
	for (auto& btn : *buttons)
		if (btn.status == 3) {
			freeze = true;
			returnValue = 0;
			break;
		}
	for (auto& b : *buttons) {
		if (b.texture == NULL)
			return returnValue;
		int status = checkButtonStatus(&b, e, freeze); 
		if (status == 0 && b.status == 1)
			returnValue = 0;
		if (status == 4) {
			returnValue = status;
			cursor = 1;
			break;
		}
		if (status == 1 && cursor != 1) {
			returnValue = status;
			cursor = 1;
		}
		else if (status == 2) {
			returnValue = status;
			cursor = 2;
			break;
		}
	}
	if (cursor == 1) {
		SDL_SetCursor(program->cursor);
	}
	else if (cursor == 2)
		SDL_SetCursor(program->handCursor);

	return returnValue;
}
int Mode::checkButtonStatus(Button* b, SDL_Event* e, bool freeze) { 
	if (b->texture == NULL)
		return 0;
	// is mouse inside or outside button
	if (e->type == SDL_MOUSEMOTION ) {
		if (b->mouseMove(e->motion.x, e->motion.y) && !freeze) {
			//on mouse
			if (b->status==2 || b->status==1 )
				return 0;
			if (b->status == 3)
				return 3;
			b->srcRect.x = b->dstRect.w;
			b->status = 1;
			return 2;
		}
		else if(!freeze){//outside mouse
			if (b->status == 0 || b->status==2)
				return 0;
			else if (b->status == 3)
				return 3;
			b->srcRect.x = 0;
			b->status = 0;
			return 1;
		}
	}
	// Mouse Button down
	else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (b->mouseDown(e->motion.x, e->motion.y)) {
			// mouse down on button
			if(b->status==2)
				return 0;
			b->srcRect.x = b->dstRect.w * 2;
			b->status = 3;
			return 3;
		}
	}
	//Mouse button UP
	else if (e->type == SDL_MOUSEBUTTONUP) {
		//If mouse is up and we consider it as a click
		if (b->mouseUp(e->motion.x, e->motion.y)) {
			b->status = 0;
			b->srcRect.x = 0;
			b->doAction();
			return 4;
		}//if mouse is up outside of button we cancel its press status
		else {
			b->status = 0;
			b->srcRect.x = 0;
			return 1;
		}
		b->srcRect.x = 0;
	}
	return 0;
}
void Mode::resetButtonsToDefault(std::vector<Button>* buttons) {
	for (auto& b : *buttons) {
		b.status = 0;
		b.srcRect.x = 0;
	}
}