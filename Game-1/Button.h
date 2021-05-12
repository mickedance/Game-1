#ifndef Button_h
#define Button_h
#include "Includes.h"
#include <functional>
#include <memory>
#include "Program.h"

using std::cout;
struct Button {
	bool initialized = false;
	int status = 0; // 0=default, 1=Hover, 2 = Clicked/down, 3
	SDL_Rect dstRect;
	SDL_Rect srcRect;
	SDL_Texture* texture = nullptr;
	std::string title;
	std::string size;
	Program* program = nullptr;
	typedef std::function<void()>lambdaFunction;

	lambdaFunction doAction;
	Button(std::string _title, Program* _program, std::string _size) {

		title = _title;
		program = _program;
		size = _size;
	}
	~Button() {
		//cout << "\ndestroy::"<<title;
	}
	Button(const Button& old) {
		initialized = old.initialized;
		status = old.status;
		dstRect = old.dstRect;
		srcRect = old.srcRect;
		title = old.title;
		size = old.size;
		program = old.program;
		doAction = old.doAction;
		texture = old.texture;
		//cout << "\ncopy:" << title;
	}
	Button(Button&& old) noexcept {
		initialized = old.initialized;
		status = old.status;
		dstRect = old.dstRect;
		srcRect = old.srcRect;
		title = old.title;
		size = old.size;
		program = old.program;
		doAction = old.doAction;
		texture = old.texture;
		old.texture = nullptr;
		//cout << "\nmove:" << title;
	}
	bool mouseDown(int x, int y) {
		//if click on button;
		if (x >= dstRect.x && x <= dstRect.x + dstRect.w && y >= dstRect.y && y <= dstRect.y + dstRect.h) {
			return true;
		}
		return false;
	}
	bool mouseMove(int x, int y) {
		if (x >= dstRect.x && x <= dstRect.x + dstRect.w && y >= dstRect.y && y <= dstRect.y + dstRect.h) {
			//inside button
			return true;		
		}
		else {//outside button
			return false;
		}
	}
	bool mouseUp(int x, int y) {
		if (x >= dstRect.x && x <= dstRect.x + dstRect.w && y >= dstRect.y && y <= dstRect.y + dstRect.h) {		
			//mouse up inside button
			return true;
		}
		else { // mouse up outside button
			return false;
		}
	}
	void init() {
		// Here we create the texture of the button. It will contain 3 status images of the button, Hover, Default and Clicked/down.

		SDL_Color color = { 255,255,255,255 };
		// Create text of button. WE need both bright and dark text so we make 2 different texture for each textcolor.
		SDL_Surface* textWhiteSurface;
		SDL_Texture* textWhiteTexture;
		SDL_Surface* textBlackSurface;
		SDL_Texture* textBlackTexture;

		SDL_Rect textRect;
		//We need som margin around the text
		int marginHeight, marginWidth;
		//Choose what font size to use
		if (size == "1/10") {
			textWhiteSurface = TTF_RenderText_Blended(program->fonts[0], title.c_str(), color);
			color = { 0,0,0,255 };
			textBlackSurface = TTF_RenderText_Blended(program->fonts[0], title.c_str(), color);
		}else if (size == "1/36") {
			textWhiteSurface = TTF_RenderText_Blended(program->fonts[2], title.c_str(), color);
			color = { 0,0,0,255 };
			textBlackSurface = TTF_RenderText_Blended(program->fonts[2], title.c_str(), color);
		}
		else {
			program->errorPrompt("could not find font: "+size );
			return;
		}
		if (textWhiteSurface == NULL || textBlackSurface == NULL) {
			program->errorPrompt("Could not create surface for button: " + title);
			return;
		}

		textWhiteTexture = SDL_CreateTextureFromSurface(program->renderer, textWhiteSurface);
		textBlackTexture = SDL_CreateTextureFromSurface(program->renderer, textBlackSurface);
		if (textWhiteTexture == NULL || textBlackTexture == NULL) {
			program->errorPrompt("Could not create texture of text for button:" + title);
			return;
		}
		SDL_FreeSurface(textWhiteSurface);
		SDL_FreeSurface(textBlackSurface);

		// Use the size of the text to make background sized accordinlgy
		SDL_QueryTexture(textWhiteTexture, NULL, NULL, &textRect.w, &textRect.h);
		marginWidth = (textRect.w / (int)8) + 7;
		marginHeight = (textRect.h / (int)8) + 4;
		textRect.x = marginWidth / (int)2;
		textRect.y = 3;

		//Create BG of button
		SDL_Surface* bgSurface = IMG_Load("pics/btn_bg.png");
		if (bgSurface == NULL) {
			program->errorPrompt("Could not create surface of bg for button:" + title);
			return;
		}
		SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(program->renderer, bgSurface);
		if (bgTexture == NULL) {
			program->errorPrompt("Could not create texture of bg for button:" + title);
			return;
		}
		SDL_FreeSurface(bgSurface);

		//Create  BTN where we have 3 different bg for Default/hover/clicked status


		SDL_Rect bgSrcRect;
		SDL_QueryTexture(bgTexture, NULL, NULL, &bgSrcRect.w, &bgSrcRect.h);
		bgSrcRect.w = bgSrcRect.w / (int)3;
		bgSrcRect.x = 0;
		bgSrcRect.y = 0;
		SDL_Rect bgDstRect;
		bgDstRect.w = textRect.w + marginWidth;
		bgDstRect.h = textRect.h + marginHeight;
		bgDstRect.y = 0;
		bgDstRect.x = 0;
		texture = SDL_CreateTexture(program->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (textRect.w + marginWidth) * 3, textRect.h + marginHeight);
		if (texture == NULL) {
			program->errorPrompt("Could not create texture target for button:" + title);
			return;
		}
		// make transparent background for texture
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		for (int i = 0; i < 3; i++) {
			// Create single Button Texture
			SDL_Texture* btnTarget = SDL_CreateTexture(program->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, textRect.w + marginWidth, textRect.h + marginHeight);
			if (btnTarget == NULL) {
				program->errorPrompt("Could not create texture for btnTarget for button:" + title);
				return;
			}

			//create one texture from each status from imagefile
			SDL_SetRenderTarget(program->renderer, btnTarget);
			bgSrcRect.x = bgSrcRect.w * i;
			SDL_RenderCopy(program->renderer, bgTexture, &bgSrcRect, &bgDstRect);

			//Default text color is white -while hover and click status has black text color
			if (i == 0)
				SDL_RenderCopy(program->renderer, textWhiteTexture, NULL, &textRect);
			else
				SDL_RenderCopy(program->renderer, textBlackTexture, NULL, &textRect);
			// Render single button to big this buttons Texture that contains all button status  images, hover, defaul and clicked
		//	SDL_Texture* t = texture.get();
			SDL_SetRenderTarget(program->renderer, texture);
			SDL_Rect btnDstRect;
			btnDstRect.y = 0;
			btnDstRect.w = bgDstRect.w;
			btnDstRect.h = bgDstRect.h;
			btnDstRect.x = btnDstRect.w * i;
			SDL_RenderCopy(program->renderer, btnTarget, NULL, &btnDstRect);
			SDL_SetRenderTarget(program->renderer, nullptr);
			SDL_DestroyTexture(btnTarget);
			btnTarget = nullptr;
		}
		SDL_DestroyTexture(textBlackTexture);
		SDL_DestroyTexture(textWhiteTexture);
		textBlackTexture = nullptr;
		textWhiteTexture = nullptr;


		dstRect.w = textRect.w + marginWidth;
		dstRect.h = textRect.h + marginHeight;
		dstRect.x = 0;
		dstRect.y = 0;
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = bgDstRect.w;
		srcRect.h = textRect.h + marginHeight;
		initialized = true;
	}

};
#endif // !Button_h
#pragma once
