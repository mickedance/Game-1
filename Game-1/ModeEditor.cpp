#include "ModeEditor.h"
#include "Button.h"
#include "PopUps.h"
#include "HandleLevel.h"
using std::cout;

ModeEditor::ModeEditor(Program* _program): Mode(_program) {
	cout << "\n new mode editor";
}

void ModeEditor::start() {
	//Create button
	createNewButton("Load Level", "1/36", [this]() {this->onLoadPopUp(); });
	createNewButton("Save Level", "1/36", [this]() {this->onSavePopUp(); });
	createNewButton("Menu", "1/36", [this]() {this->onMenu(); });
	createNewButton("Save", "1/36", [this]() {this->onSave(); });

	//Init Buttons
	for (auto& b : buttons) {
		b.init();
	}
	//Place buttons
	int winW, winH;
	SDL_GetWindowSize(program->window, &winW, &winH);
	for (int i = 0; i < (int)buttons.size(); i++) {
		if (buttons[i].title == "Save")
			buttons[i].dstRect.x = winW - buttons[i].dstRect.w - 10;
		else if (i > 0)
			buttons[i].dstRect.x = buttons[i - 1].dstRect.w + buttons[i - 1].dstRect.x + 10;
		else
			buttons[i].dstRect.x = 0;

		buttons[i].dstRect.y = winH - buttons[i].dstRect.h;
	}

	// if handleLevel is not initialized we initilize it
	if (program->handleLevel->initialized == false)
		program->handleLevel->init();

	/*
						DROPDOWN
	*/

	//create menuclickitems 
	SDL_Rect rect;
	rect.h = (int)(winH * .04);
	if (rect.h < 25)
		rect.h = 25;
	rect.w = (int)(rect.h * 3);
	rect.x = (int)winW / 2 - rect.w / 2;
	rect.y = 0;

	createMenuClickItem("arrow_down.png", rect, program, true, [this]() { this->toggleDropdown(); });

	// Design and position dropdown
	dropdown = new Dropdown(this);
	dropdown->dstRect.h = (int)(winH * .165);
	if (dropdown->dstRect.h < 72)
		dropdown->dstRect.h = 75;
	dropdown->dstRect.x = 0;
	dropdown->dstRect.w = winW;
	dropdown->dstRect.y = 0;
	Uint32 rmask, gmask, bmask, amask;
	program->getRGBAmask(&rmask, &gmask, &bmask, &amask);
	SDL_Surface* s = SDL_CreateRGBSurface(0, dropdown->dstRect.w, dropdown->dstRect.h, 32, rmask, gmask, bmask, amask);
	if (s == NULL) {
		program->errorPrompt("Could not create surface for dropdown BG");
		return;
	}
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 25, 255, 55));
	dropdown->texture = SDL_CreateTextureFromSurface(program->renderer, s);
	if (dropdown->texture == NULL) {
		program->errorPrompt("Could not create texurue for dropdown BG");
		return;
	}
	SDL_FreeSurface(s);

	//Load and position Levelitems in list on dropdown	
	int x = 0;
	int margin = 10;
	//First place an image that tells what kind of item user can select. Here BG
	if (program->handleLevel->BGTextures.size() > 0) {
		SDL_Surface* s = IMG_Load("pics/bg_here.png");
		SDL_Texture* t = SDL_CreateTextureFromSurface(program->renderer, s);
		if (s == NULL || t == NULL) {
			program->errorPrompt("Could not create surface or texture for: pics/bg_here.png");
			return;
		}
		SDL_FreeSurface(s);
		dropdown->loadAndPostionItem(t, margin, false);
	}
	//BG Textures
	for (int i = 0; i < (int)program->handleLevel->BGTextures.size(); i++) {
		if (dropdown->levelItems.size() == 0)
			x = margin;
		else
			x = dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.x + dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.w + margin;
		dropdown->loadAndPostionItem(program->handleLevel->BGTextures[i], x, true);
	}
	//Now place an image that tells what kind of item user can select. Here OBstacle
	if (program->handleLevel->OBSTextures.size() > 0) {
		SDL_Surface* s = IMG_Load("pics/obs_here.png");
		SDL_Texture* t = SDL_CreateTextureFromSurface(program->renderer, s);
		if (s == NULL || t == NULL) {
			program->errorPrompt("Could not create surface or texture for: pics/bg_here.png");
			return;
		}
		SDL_FreeSurface(s);
		int x = 10;
		if (dropdown->levelItems.size() > 0) {
			x += dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.x + dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.w;
		}
		dropdown->loadAndPostionItem(t, x, false);
	}
	//OBS textures
	for (int i = 0; i < (int)program->handleLevel->OBSTextures.size(); i++) {
		if (dropdown->levelItems.size() == 0)
			x = margin;
		else
			x = dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.x + dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.w + margin;
		dropdown->loadAndPostionItem(program->handleLevel->OBSTextures[i], x, true);
	}

	/*
			Dropdown Scrollbar
	*/

	//Design and position dropdowns scrollbar
	scrollbars.emplace_back();
	scrollbars[scrollbars.size() - 1].dstRect.x = 0;
	scrollbars[scrollbars.size() - 1].dstRect.h = (int)(dropdown->dstRect.h * .22);
	if (dropdown->levelItems.size() > 0)
		scrollbars[scrollbars.size() - 1].dstRect.h = (dropdown->dstRect.h - dropdown->levelItems[0].dstRect.h) / 3;

	scrollbars[scrollbars.size() - 1].dstRect.y = dropdown->dstRect.h - scrollbars[scrollbars.size() - 1].dstRect.h;
	double percent = winW / (double)(dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.x + dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.w);
	scrollbars[scrollbars.size() - 1].dstRect.w = (int)(winW * percent);
	scrollbars[scrollbars.size() - 1].init(program);
}
void ModeEditor::Scrollbar::init(Program* _program){
	Uint32 rmask, gmask, bmask, amask;
	_program->getRGBAmask(&rmask, &gmask, &bmask, &amask);
	SDL_Surface* s = SDL_CreateRGBSurface(0, dstRect.w, dstRect.h, 32, rmask, gmask, bmask, amask );
	if (s == NULL) {
		_program->errorPrompt("Could not create surface for: scrollbar");
		return;
	}
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 55, 55, 155));
	texture = SDL_CreateTextureFromSurface(_program->renderer, s);
	if (texture == NULL) {
		_program->errorPrompt("Could not create texture for: scrollbar");
		return;
	}
	SDL_FreeSurface(s);
}
void ModeEditor::createMenuClickItem(std::string _filename, SDL_Rect _rect, Program* _program, bool _clickable, std::function<void()> func) {
	_filename = "pics/" + _filename;
	SDL_Surface* s = IMG_Load(_filename.c_str());
	if (s == NULL) {
		_program->errorPrompt("Could not create surface for: "+ _filename);
		return;
	}
	SDL_Texture* t = SDL_CreateTextureFromSurface(_program->renderer, s);
	if (t == NULL) {
		_program->errorPrompt("Could not create texurue for: "+ _filename);
		return;
	}
	SDL_FreeSurface(s);

	menuClickItems.emplace_back(t, _rect, func, true);
}
void ModeEditor::Dropdown::loadAndPostionItem(SDL_Texture* _texture, int x, bool _clickable) {
	SDL_Rect rect;
	rect.h = (int)(dstRect.h * .75);
	rect.y = (dstRect.h - rect.h) / 2;
	levelItems.emplace_back(_texture, _clickable);
	int tW, tH;
	SDL_QueryTexture(_texture, NULL, NULL, &tW, &tH);
	double ratio = tW / (double)tH;
	cout << "\nratio:" << ratio << ", " << tW << ", " << tH;
	rect.w = (int)(ratio * rect.h);

	rect.x = x;
	levelItems[levelItems.size()-1].dstRect = rect;
}
ModeEditor::Dropdown::Dropdown(ModeEditor* _modeEditor) {
	modeEditor = _modeEditor;
}
ModeEditor::MenuClickItem::MenuClickItem(SDL_Texture *_texture, SDL_Rect rect, std::function<void()> func, bool _clickable) {
	clickable = _clickable;
	texture = _texture;
	dstRect = rect;
	doAction = func;
}
void ModeEditor::toggleDropdown() {
	if (dropdown != nullptr) {
		dropdown->active = !dropdown->active;
	}
}
void ModeEditor::onSave() {
	cout << "\nsaved";
}
void ModeEditor::onLoadPopUp() {
	std::string confirmText = "Your current level will be replaced. Are You sure you wanna proceed?";
	ConfirmPrompt* confirm = new ConfirmPrompt(this, winLayers.size(), confirmText, [this]() {this->startLoadPopUp(); });
	confirm->start();
	WindowLayer* w = confirm;
	winLayers.emplace_back(w);
}
void ModeEditor::onMenu() {
	program->startMode(program->modeMenu);
}
void ModeEditor::startLoadPopUp() {
	LoadPopUp* popup = new LoadPopUp("Load Level", this, winLayers.size());
	popup->start();
	popup->createLoadBtn();
	popup->initBtns();
	WindowLayer* w = popup;
	winLayers.emplace_back(w);
}
void ModeEditor::onSavePopUp(){
	SavePopUp* popup = new SavePopUp("Save Level", this, winLayers.size());
	popup->start();
	popup->createSaveAsBtn();
	popup->initBtns();
	WindowLayer* w = popup;
	winLayers.emplace_back(w);
	cout << "\nonSavepopup";
}
void ModeEditor::createNewButton(std::string _title, std::string _fontSize, std::function<void()> func ) {
	buttons.emplace_back(_title, program, _fontSize);
	buttons[buttons.size() - 1].doAction = func;
}
void ModeEditor::stop() {
	for (auto& win : winLayers) {
		win->stop();
		delete win;
	}
	winLayers.clear();
	for (auto& b : buttons) {
		SDL_DestroyTexture(b.texture);
		b.texture = nullptr;
	}
	buttons.clear();
	SDL_SetRenderDrawColor(program->renderer, 0, 0, 0, 0);

	for (auto& m : menuClickItems) {
		SDL_DestroyTexture(m.texture);
		m.texture = nullptr;
	}
	menuClickItems.clear();

	if (dropdown != nullptr) {
		SDL_DestroyTexture(dropdown->texture);
	}
	delete dropdown;
}
void ModeEditor::render() {

	SDL_RenderClear(program->renderer);
	//SET BG COLOR
	SDL_SetRenderDrawColor(program->renderer, 255, 55,55,255);

	//Buttons
	for (auto b : buttons) {
		if (b.texture != NULL) {
			SDL_RenderCopy(program->renderer, b.texture, &b.srcRect, &b.dstRect);
		}
	}
	//dropDown
	if (dropdown != nullptr) {
		//If dropdown is not active we show arrow user can click to activate and see dropdown
		if (!dropdown->active && menuClickItems.size()>0) {
		{
				if (menuClickItems[0].texture != nullptr && !dropdown->active )
					SDL_RenderCopy(program->renderer, menuClickItems[0].texture, NULL,  &menuClickItems[0].dstRect);
		}
		}//else if dropdown is active, we show it
		else {
			//BG
			SDL_RenderCopy(program->renderer, dropdown->texture, NULL, &dropdown->dstRect);

			// Level ITems
			for (auto item : dropdown->levelItems) {
				SDL_Rect rect = item.dstRect;
				rect.x -= dropdown->xScrollPos;
				cout << "\nrect: "<< rect.x;
				SDL_RenderCopy(program->renderer, item.texture_ptr, NULL, &rect);
			}
			//Scrollbar
			for (auto &bar : scrollbars) {
				if (bar.texture != NULL) {
					SDL_RenderCopy(program->renderer, bar.texture, NULL, &bar.dstRect);
				}
			}
		}
	}
	//winlayers
	for (auto &win : winLayers) {
		win->render();
	}
	SDL_RenderPresent(program->renderer);
}

void ModeEditor::userEvents(SDL_Event e) {

	if (winLayers.size() > 0) {
		winLayers[winLayers.size() - 1]->userEvents(e);
		//reset all buttons to default
		resetButtonsToDefault(&buttons);
		return;
	}

	handleBtns(&buttons, &e);
	if (e.type == SDL_KEYDOWN) {
		if (e.key.keysym.sym == SDLK_ESCAPE) {
			program->startMode(program->modeMenu);
		}
	}
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		//See if user clicked on some menuClickItem when dropdown is no visible/active
		if(dropdown!=nullptr && !dropdown->active)
			for (auto &m : menuClickItems) {
				if ( m.clickable && e.motion.x >= m.dstRect.x && e.motion.x <= m.dstRect.x + m.dstRect.w && e.motion.y >= m.dstRect.y && e.motion.y <= m.dstRect.y + m.dstRect.h) {
					cout << "\n clicked";
					m.doAction();
					SDL_SetCursor(program->cursor);
				}
			}
		//Dropdown is visible/active
		else {
			// See if mouse is over a scrollbar
			for (auto &bar : scrollbars) {
				if (e.motion.x >= bar.dstRect.x && e.motion.x <= bar.dstRect.x + bar.dstRect.w && e.motion.y >= bar.dstRect.y && e.motion.y <= bar.dstRect.y + bar.dstRect.h) {
					if (bar.status != 1) {
						bar.status = 1;
						bar.mouseClickedXPos = e.motion.x - bar.dstRect.x;
						cout << "\nposx: " << bar.mouseClickedXPos;
					}
					
				}
			}
		}
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
		//changde status and disable scroll of scrollbar that is currently active with status 1

		for (auto& bar : scrollbars) {
			if (bar.status == 1)
				bar.status = 0;
		}
	}
	else if (e.type == SDL_MOUSEMOTION) {
		int changeMouse = 0;// 0= not changed, 1 = change to cursor, 2 = change to hand

		//If dropdown is hidden/not active
		if (dropdown != nullptr && !dropdown->active) {
			//See if user has mouse on some menuClickItem, than we change it's status and cursor	
			for (auto& m : menuClickItems) {
				//Inside
				if (e.motion.x >= m.dstRect.x && e.motion.x <= m.dstRect.x + m.dstRect.w
					&& e.motion.y >= m.dstRect.y && e.motion.y <= m.dstRect.y + m.dstRect.h) {
					if (m.status != 1 && m.clickable) {
						m.status = 1;
						changeMouse = 2;
					}
				}//outside
				else {
					if (m.status != 0 && m.clickable) {
						m.status = 0;
						changeMouse = 1;
					}
				}
			}
		}//Dropdown is visible/active
		else {
			//see if mouse is over one of the levelitems
			for (auto& item : this->dropdown->levelItems) {
				if (e.motion.x >= item.dstRect.x && e.motion.x <= item.dstRect.x + item.dstRect.w
					&& e.motion.y>= item.dstRect.y && e.motion.y<=item.dstRect.y + item.dstRect.h) {
					if (item.status != 1 && item.clickable) {
						item.status = 1;
						changeMouse = 2;
					}					
				}//outside
				else {
					if (item.status != 0 && item.clickable) {
						item.status = 0;
						changeMouse = 1;
					}
				}
			}
			// update scrollbar if they have status=1
			int i = 0;
			for (auto &bar : scrollbars) {
				if (bar.status == 1) {
					bar.dstRect.x = e.motion.x - bar.mouseClickedXPos;
					// prevent from scrolling outside window
					if (bar.dstRect.x <= 0)
						bar.dstRect.x = 0;
					int winW;
					SDL_GetWindowSize(program->window, &winW, NULL);
					if (bar.dstRect.x + bar.dstRect.w >= winW)
						bar.dstRect.x = winW - bar.dstRect.w;
					//scroll level items x-axis
					if (i == 0) {
						int smallWidth = winW - bar.dstRect.w;
						double percent = bar.dstRect.x / (double)smallWidth;
						int widthLevelItems = winW;
						if(dropdown!=nullptr &&  dropdown->levelItems.size()>0)
							widthLevelItems = 10+ dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.x + dropdown->levelItems[dropdown->levelItems.size() - 1].dstRect.w - winW;
	
						dropdown->xScrollPos = (int)(percent * widthLevelItems);
					}
					// update 
				}
				i++;
			}
		}
		if (changeMouse == 1) {
			SDL_SetCursor(program->cursor);
		}
		else if (changeMouse == 2) {
			SDL_SetCursor(program->handCursor);
		}
	}
}