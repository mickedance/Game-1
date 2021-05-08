#include "PopUps.h"
#include "Mode.h"
#include "Program.h"
#include "Button.h"
#include <filesystem>

using std::cout;
LoadPopUp::LoadPopUp(Mode* _mode, int _index): WindowLayer(_mode, _index) {

}
void LoadPopUp::stop() {
	//Destroy texture of buttons and popup's texture
	cout << "\nstop0";

	for (auto& b : buttons) {
		SDL_DestroyTexture(b.texture);
		b.texture = nullptr;
	}
	cout << "\nstop1";
	SDL_DestroyTexture(texture);
	texture = nullptr;
	// Deleete files
	cout << "\nstop2";
	delete files;
	SDL_DestroyTexture(textureForFiles);
	//Delete scrollbar
	SDL_DestroyTexture(scrollbar->texture);
	cout << "\nstop3";
	delete scrollbar;
	// Delete TextInputField
	SDL_DestroyTexture(textureForTextInputField);
	textureForTextInputField = nullptr;
	//Delete and remove this popup from winLayers list 
	WindowLayer* ptr = mode->winLayers.at(index);
	cout << "\nstop4";
	mode->winLayers.erase(mode->winLayers.begin() + index );
	delete ptr;
	
	
}

void LoadPopUp::start() {
	//
	//First create a design of this load popup, without list of files
	//Then we load files
	
	Program* program = mode->program;
	// Title of popup
	SDL_Color textColor = { 255,255,255,255 };
	SDL_Surface* s = TTF_RenderText_Blended(program->fonts[1], "Load Level", textColor );
	if (s == NULL) {
		program->errorPrompt("Could not create surface for title of load pop up");
		return;
	}
	SDL_Texture* title = SDL_CreateTextureFromSurface(program->renderer, s);
	if (title == NULL) {
		program->errorPrompt("Could not create texture for title of load pop up");
		return;
	}
	SDL_FreeSurface(s);
	
	
	//Size of popup
	int ww, wh;
	SDL_GetWindowSize(program->window, &ww, &wh);
	texture = SDL_CreateTexture(program->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)(ww*.70), (int)(wh*.70) );
	if (texture == NULL) {
		program->errorPrompt("Could not create texture for load pop up bg");
		return;
	}
	//position of popup
	int tw, th;
	SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
	dstRect.x = (ww - tw) / (int)2;
	dstRect.y = (wh - th) / (int)2;

	// position of popup title
	SDL_Rect titleRect;
	SDL_QueryTexture(title, NULL, NULL, &titleRect.w, &titleRect.h);
	titleRect.x =  tw/(int)2 - titleRect.w/(int)2 ;
	titleRect.y =  10 ;
	SDL_QueryTexture(texture, NULL, NULL, &dstRect.w, &dstRect.h);

	// init blackbox where list will be displayed
	Uint32 rmask, gmask, bmask, amask;
	program->getRGBAmask(&rmask, &gmask, &bmask, &amask);
	//set postion of this black box
	SDL_Rect blackBoxRect;
	int btnHeight = 0;
	int margin = 40;
	if (buttons.size() > 0)
		btnHeight = buttons[0].dstRect.h;
	blackBoxRect.w = (int)(dstRect.w * .70);
	blackBoxRect.h = (int)dstRect.h - titleRect.y - titleRect.h*2 - btnHeight - margin;
	blackBoxRect.x = (dstRect.w - blackBoxRect.w) / (int)2;
	blackBoxRect.y = titleRect.y+ titleRect.h + margin/(int)2;
	SDL_Surface* tmpS = SDL_CreateRGBSurface(0, blackBoxRect.w, blackBoxRect.h, 32, rmask, gmask, bmask, amask );

	if (tmpS == NULL) {
		program->errorPrompt("Could not create surface for black box pop up bg");
		return;
	} 
	
	SDL_Texture* blackBoxTexture = SDL_CreateTextureFromSurface(program->renderer, tmpS);
	if (blackBoxTexture == NULL) {
		program->errorPrompt("Could not create texture for black box pop up");
		return;
	}
	// save FontHeight in var: heightOfEachFileTexture
	TTF_SizeText(mode->program->fonts[2], "", NULL, &heightOfEachFileTexture);

	SDL_FreeSurface(tmpS);
	//Create textinput field
	textFieldRect.x = blackBoxRect.x ;
	textFieldRect.w = (int)(blackBoxRect.w*.7);
	textFieldRect.h = (int) (heightOfEachFileTexture * 1.5);
	textFieldRect.y = blackBoxRect.h + blackBoxRect.y + (int)((dstRect.h-blackBoxRect.h)/4) -(int) (textFieldRect.h/2) ;
	tmpS = SDL_CreateRGBSurface(0, textFieldRect.w, textFieldRect.h, 32, rmask, gmask, bmask, amask);
	if (tmpS == NULL) {
		program->errorPrompt("Could not create surface for textinput field box pop up bg");
		return;
	}
	SDL_FillRect(tmpS, NULL, SDL_MapRGB(tmpS->format, 80, 40, 0));
	SDL_Texture* textInputFieldTexture = SDL_CreateTextureFromSurface(program->renderer, tmpS);
	if (textInputFieldTexture == NULL) {
		cout << "\n" << SDL_GetError();

		program->errorPrompt("Could not create texture for textinput field on pop up ");
		return;
	}
	SDL_FreeSurface(tmpS);

	//set bg color of popupwindow
	SDL_SetRenderDrawColor(program->renderer, 155, 55, 5, 255);
	
	SDL_SetRenderTarget(program->renderer, texture);
	SDL_RenderFillRect(program->renderer, NULL);
	//Render title, black box etc on popup-window/box
	SDL_RenderCopy(program->renderer, blackBoxTexture, NULL, &blackBoxRect);
	SDL_RenderCopy(program->renderer, textInputFieldTexture, NULL, &textFieldRect);
	SDL_RenderCopy(program->renderer, title, NULL, &titleRect);
	SDL_SetRenderTarget(program->renderer, nullptr);
	SDL_DestroyTexture(title);
	SDL_DestroyTexture(blackBoxTexture);
	SDL_DestroyTexture(textInputFieldTexture);


	//Create Cancel Button
	createCancelBtn();

	// Load files in dir
	fileListDstRect = blackBoxRect;
	fileListDstRect.x += dstRect.x;
	fileListDstRect.y += dstRect.y;
	files = new Files("levels/");
	loadFiles();
}
void LoadPopUp::loadFiles() {
	//see if directory exists, otherwise we create it
	if (!std::filesystem::exists(files->path)) {
		std::filesystem::create_directories(files->path);
	}
	// get filenames in directory
	for (const auto& entry : std::filesystem::directory_iterator(files->path)) {
		files->files.emplace_back(  entry.path().u8string() );
	}
	// init main Texture where we place file-textures on
	fileListSrcRect.x = 0;
	fileListSrcRect.y = 0;
	fileListSrcRect.h = fileListDstRect.h;
	fileListSrcRect.w = fileListDstRect.w;
	updateFileList();
	indexOfCurrentTexture++;
	scrollbar = new Scrollbar(this);
	scrollbar->init();

}
void LoadPopUp::scrollFiles(double percent, int withMouseWheel = 0) {
	//withMouseWheel  / 0 = not scrolling withmouseWheel, -1= up, 1 = down
	/*
	* Here we scroll files list of files on texture: textureForFiles
		And since we don't want that texture to have infinity height we make it max the height of sdl window height.
		And if there's more file in dir than fits on that height we render another set of files on  that texture.
		Here we check if user has scrolled to the bottom of texture: textureForFiles and there is more file in that dir we have to update
		textureForFiles with next set of list of files. 
		And we also see if user has scrolled to top of current set of files and there's more files to be scrolled up to
	*/
	if (files->files.size() < 1 )
		return;
	//
	//    Handle scroll With mousewheel
	//
	if (withMouseWheel != 0) {
		int wheelSpeed = heightOfEachFileTexture;
		fileListSrcRect.y += wheelSpeed * withMouseWheel;
	}
	else {
		int diff = files->files.size() * heightOfEachFileTexture - fileListDstRect.h;
		double scrollToHere = diff * percent;
		fileListSrcRect.y =(int) scrollToHere - topFileIndexInScroll*heightOfEachFileTexture;
	}
	int textureHeight;
	SDL_QueryTexture(textureForFiles, NULL, NULL, NULL, &textureHeight);
	int windowHeight;
	SDL_GetWindowSize(mode->program->window, NULL, &windowHeight);
	int nrOfFilesFitWinHeight = (int) windowHeight / heightOfEachFileTexture;
	int nrOfFilesFitDstRectHeight = (int)fileListDstRect.h / heightOfEachFileTexture;

	
	//If user scrolled to bottom of fileTexture and more files exists in directory,  we have to update it so we can scroll further down and view more files
	if (fileListSrcRect.y + fileListDstRect.h > nrOfFilesOnTextureNow * heightOfEachFileTexture) {
		//Stop from scrolling below last file in directory
		if ((int)files->files.size() <= topFileIndexInScroll + nrOfFilesOnTextureNow) {
			fileListSrcRect.y = nrOfFilesOnTextureNow * heightOfEachFileTexture - fileListDstRect.h;
			return;
		}
		//Get what file to start "new" textureOffiles with at top
		int nextTopIndex = topFileIndexInScroll + nrOfFilesOnTextureNow - nrOfFilesFitDstRectHeight;
		topFileIndexInScroll = nextTopIndex;
		fileListSrcRect.y = 0;
		indexOfCurrentTexture++;
		updateFileList();

	}// If user has scrolled to top of filetexture and there's more file to show scrolling upwards, we update
	else if (fileListSrcRect.y <0) {
		int nextTopIndex = topFileIndexInScroll + nrOfFilesFitDstRectHeight - nrOfFilesFitWinHeight;
		if (nextTopIndex < 0) {
			fileListSrcRect.y = 0;
			updateScrollbarPos(0.0);
			return;
		}
		
		int y = nrOfFilesFitWinHeight - nrOfFilesFitDstRectHeight;
		y = nrOfFilesFitWinHeight - y;
		y = y * heightOfEachFileTexture;
		fileListSrcRect.y = y  ;
		topFileIndexInScroll = nextTopIndex;
		updateFileList();
		indexOfCurrentTexture--;
	}

	if (withMouseWheel != 0) {
		int scrolled = topFileIndexInScroll * heightOfEachFileTexture + fileListSrcRect.y ;
		int totalHeight = files->files.size() * heightOfEachFileTexture - fileListDstRect.h;
		float percent = scrolled / (float)totalHeight;
		updateScrollbarPos(percent);
	}
	

	if (withMouseWheel == 0)
		return;
	
}
void LoadPopUp::updateScrollbarPos(float percent) {
	int diff = fileListDstRect.h - scrollbar->rect.h;
	float y = diff * percent;
	scrollbar->rect.y =    fileListDstRect.y + (int) y;
}
bool LoadPopUp::updateFileList() {
	//cout << "\n from " <<topFileIndexInScroll<<", "<< files->files[topFileIndexInScroll];
	Program* program = mode->program;
	int nrOfFilesOnLastTexture = nrOfFilesOnTextureNow;
	int wh;
	SDL_GetWindowSize(mode->program->window, NULL, &wh);
	int fontHeight;
	TTF_SizeText(mode->program->fonts[2], "", NULL, &fontHeight);
	int nrOfFilesThatFitsTextureHeight = wh/(int) fontHeight;
	int heightOfBigTexture = nrOfFilesThatFitsTextureHeight * fontHeight;
	if (textureForFiles != NULL)
		SDL_DestroyTexture(textureForFiles);
	textureForFiles = SDL_CreateTexture(program->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, fileListSrcRect.w,  heightOfBigTexture);
	nrOfFilesOnTextureNow = 0;
	for (int i = topFileIndexInScroll; i < topFileIndexInScroll + nrOfFilesThatFitsTextureHeight * (indexOfCurrentTexture +1); i++) {
		if (i > (int)files->files.size()-1 || (int) files->files.size()<1)
			break;

		// color of text, white as default, blue-ish if file is hovered or selected
		SDL_Color color = { 255,255,255 };
		if (files->mouseOverIndex == i || files->selectedFileIndex==i)
			color = { 55,255,55 };
		SDL_Surface* s = TTF_RenderText_Blended(program->fonts[2], files->files[i].c_str(), color);
		if (s == NULL) {
			program->errorPrompt("Could not create surface for file: "+ files->files[i]);
			return false;
		}
		SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(program->renderer, s);
		if (fontTexture == NULL) {
			program->errorPrompt("Could not create texture for file: "+ files->files[i]);
			return false;
		}
		SDL_FreeSurface(s);
		SDL_Rect fileRect;
		int marginLeft = 10;
		fileRect.x = marginLeft;
		fileRect.y = (i - topFileIndexInScroll) * heightOfEachFileTexture;
		SDL_QueryTexture(fontTexture, NULL, NULL, &fileRect.w, &fileRect.h);
		fileRect.w -= marginLeft;
		if (fileRect.y + heightOfEachFileTexture> heightOfBigTexture ) {
			return true;
		}
		SDL_SetRenderTarget(program->renderer, textureForFiles);
		SDL_RenderCopy(program->renderer, fontTexture, NULL, &fileRect);
		SDL_SetRenderTarget(program->renderer, nullptr);
		SDL_DestroyTexture(fontTexture);
		nrOfFilesOnTextureNow++;
	}
		updateTextInputField("Min donkey()sdlkfhlksdjhflkjsdfhlksdjhfkljsdhfklsdjhfksdjhfsdkjfh");
	return true;
}
void LoadPopUp::createCancelBtn() {
	createButton("Cancel", "1/36", [this]() { this->onCancelBtn(); });
	
}
void LoadPopUp::createButton(std::string title, std::string size, std::function<void()> func) {
	buttons.emplace_back(title, mode->program, "1/36");
	buttons.at(buttons.size() - 1).doAction = func;
}
void LoadPopUp::createLoadBtn() {
	createButton("Load", "1/42", [this]() { this->onLoadBtn(); });
}
void LoadPopUp::initBtns(){
	for (auto& b : buttons)
		b.init();

	//place buttons
	auto it = buttons.rbegin();
	int totalWidth = 0;
	int margin = 10;
	int index = 0;
	while (it != buttons.rend()) {
		it->dstRect.x = dstRect.x + dstRect.w - it->dstRect.w -  totalWidth - margin* index -16;
		it->dstRect.y = dstRect.y + dstRect.h - it->dstRect.h - 8;
		totalWidth += it->dstRect.w;
		index++;
		it++;
	}
}
void LoadPopUp::render() {

	if(texture!=NULL)
		SDL_RenderCopy(mode->program->renderer, texture, NULL, &dstRect);
	
	if (textureForFiles != NULL) {		
		SDL_RenderCopy(mode->program->renderer, textureForFiles, &fileListSrcRect, &fileListDstRect);
	}
	for (auto& b : buttons) {
		if (b.texture != NULL)
			SDL_RenderCopy(mode->program->renderer, b.texture, &b.srcRect, &b.dstRect);
	}

	if (scrollbar->texture != NULL) {
		SDL_RenderCopy(mode->program->renderer, scrollbar->texture, NULL, &scrollbar->rect);
	}
	if (textureForTextInputField != NULL) {
		int marginLR = 2;
		SDL_Rect r = textFieldRect;
		r.h = heightOfEachFileTexture;
		r.y +=dstRect.y + heightOfEachFileTexture - textFieldRect.h/2;
		r.x = dstRect.x + (dstRect.w - (int) (dstRect.w * .70) ) / 2 +marginLR ;
		r.w -= marginLR*2;
		SDL_Rect rSrc = textFieldRect;
		rSrc.x = 0;
		rSrc.y = 0;
		rSrc.h = heightOfEachFileTexture;
		rSrc.w -= marginLR*2;
		//SDL_QueryTexture(textureForTextInputField, NULL ,NULL, &r.w, NULL );
		SDL_RenderCopy(mode->program->renderer, textureForTextInputField, &rSrc, &r);

		

		
	}
}
void LoadPopUp::userEvents(SDL_Event e) {
		mode->handleBtns(&buttons, &e);
		//
		//   Handle scrollbar
		//
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			//See if user clicked scrollbar and handle that
			if (e.motion.x >= scrollbar->rect.x && e.motion.x <= scrollbar->rect.x + scrollbar->rect.w 
				&& e.motion.y >= scrollbar->rect.y && e.motion.y <= scrollbar->rect.y + scrollbar->rect.h) {
				if (scrollbar->status != 1) {
					scrollbar->yPosWhereMouseWasClicked = e.motion.y - scrollbar->rect.y;
					scrollbar->status = 1;
					return;
				}
			}
			// See if user clicked inside textureforfiles where files are listed
			else if (e.motion.x >= fileListDstRect.x && e.motion.x <= fileListDstRect.x + fileListDstRect.w && e.motion.x < scrollbar->rect.x
				&& e.motion.y >= fileListDstRect.y && e.motion.y <= fileListDstRect.y + fileListDstRect.h) {
				// Get what file user clicked on
				for (int i = 0; i < (int)files->files.size(); i++) {
					int aboveTexture = topFileIndexInScroll * heightOfEachFileTexture;
					int topOfFile = i * heightOfEachFileTexture + fileListDstRect.y - aboveTexture - fileListSrcRect.y;
					int bottomOfFile = topOfFile + heightOfEachFileTexture;
					if (e.motion.y >= topOfFile && e.motion.y <= bottomOfFile && files->selectedFileIndex != i) {
						files->selectedFileIndex = i;
						files->mouseOverIndex = -1;
						updateFileList();
						break;
					}
				}
			}
			//See if user clicked on inputtextfield
			else if (e.motion.x>= textFieldRect.x + dstRect.x && e.motion.x<=  textFieldRect.x + dstRect.x + textFieldRect.w 
				&& e.motion.y>= textFieldRect.y +  ((dstRect.h-fileListDstRect.h)/2) + ((dstRect.h - fileListDstRect.h) / 2) - textFieldRect.h 
				&& e.motion.y <= textFieldRect.y + textFieldRect.h + ((dstRect.h - fileListDstRect.h) / 2) + ((dstRect.h - fileListDstRect.h) / 2) - textFieldRect.h) {
				cout << "\n inside text"<< e.motion.y <<", "<< textFieldRect.y <<", "<< textFieldRect.h;
			}
			// otherwise user clicked outside textureoffiles and we unselect file
			else  {
				//Unselect file
				if (files->selectedFileIndex != -1) {
					cout << "\noutside";
					files->selectedFileIndex = -1;
					updateFileList();
				}
				if (scrollbar->status == 1) {
					scrollbar->status = 0;
				}
			}


		}
		if (e.type == SDL_MOUSEMOTION) {
			//SCrollbar

			if (scrollbar->status == 1) {
				scrollbar->rect.y = e.motion.y - scrollbar->yPosWhereMouseWasClicked;
				// prevent from moving scrollbar to high or low

				if (scrollbar->rect.y <= fileListDstRect.y)
					scrollbar->rect.y = fileListDstRect.y;
				else if (scrollbar->rect.y + scrollbar->rect.h >= fileListDstRect.y + fileListDstRect.h)
					scrollbar->rect.y = fileListDstRect.y + fileListDstRect.h - scrollbar->rect.h;
				int diff =  fileListDstRect.h - scrollbar->rect.h ;
				double percent = 1;
				if (diff != 0)
					percent = ((double)scrollbar->rect.y - fileListDstRect.y ) / (double)diff;
				scrollFiles(percent);
			}
			// See If user is over a file in list
			// first see if mouse is within blackBox where files are listed
			if (e.motion.x >= fileListDstRect.x && e.motion.x <= fileListDstRect.x + fileListDstRect.w && e.motion.x< scrollbar->rect.x
				&& e.motion.y>= fileListDstRect.y && e.motion.y <= fileListDstRect.y + fileListDstRect.h) {
				bool update = false;
				for (int i = 0; i < (int)files->files.size(); i++) {
					int heightAbove = topFileIndexInScroll * heightOfEachFileTexture ;
					int topOfFile = i * heightOfEachFileTexture - heightAbove - fileListSrcRect.y;
					int bottomOfFile = (i+1) * heightOfEachFileTexture - heightAbove - fileListSrcRect.y;
					// now lets se what file mouse is over
					if (e.motion.y - fileListDstRect.y >= topOfFile && e.motion.y - fileListDstRect.y <= bottomOfFile && files->mouseOverIndex!=i && scrollbar->status!=1) {
						files->mouseOverIndex = i;
						updateFileList();
						break;
					}
				}
			}//If mouse is outside textureForFiles
			else if (files->mouseOverIndex != -1) {
				files->mouseOverIndex = -1;
				updateFileList();
			}
		}
		if (e.type == SDL_MOUSEWHEEL) {
			scrollFiles(0, e.wheel.y*-1);
		}
		if (e.type == SDL_MOUSEBUTTONUP) {
			// disable scroll of scrollbar
			if (scrollbar->status == 1)
				scrollbar->status = 0;
		}
}

void LoadPopUp::onCancelBtn() {
	cout << "\ncancel";
	stop();
}
void LoadPopUp::onLoadBtn() {
	//cout << "\nonload: "<< files->selectedFileIndex<<", "<<  files->files[files->selectedFileIndex];
	
}
void LoadPopUp::updateTextInputField(std::string file) {
	SDL_Color color = { 255,255,255 };
	SDL_Surface* s = TTF_RenderText_Blended(mode->program->fonts[2], file.c_str(), color);
	textureForTextInputField = SDL_CreateTextureFromSurface(mode->program->renderer, s);
	SDL_FreeSurface(s);
}
LoadPopUp::Files::Files(std::string _path) {
	path = _path;
}
LoadPopUp::Scrollbar::Scrollbar(LoadPopUp*  _loadpopup) {
	loadpopup = _loadpopup;
}
void LoadPopUp::Scrollbar::init() {
	//set size and positionn of scrollbar
	//if (loadpopup->files->files.size() < 1)
		//return;
	int nrfitsDstHeight = loadpopup->fileListDstRect.h / (int)loadpopup->heightOfEachFileTexture;
	int totalHeightOfFileTextures = loadpopup->files->files.size()* loadpopup->heightOfEachFileTexture  ;
	double percent = (loadpopup->fileListDstRect.h) /(double) (totalHeightOfFileTextures);
	if (percent > 1)
		percent = 1;
	rect.h = (int)(percent * loadpopup->fileListDstRect.h);
	rect.w = 10;
	rect.y = loadpopup->fileListDstRect.y;
	rect.x = loadpopup->fileListDstRect.x + loadpopup->fileListDstRect.w - rect.w - 2;
	Uint32 rmask, gmask, bmask, amask;
	loadpopup->mode->program->getRGBAmask(&rmask, &gmask, &bmask, &amask);
	SDL_Surface* s = SDL_CreateRGBSurface(0,66,99, 32, rmask, gmask, bmask, amask);
	if (s == NULL) {
		loadpopup->mode->program->errorPrompt("could not create surface for scrollbar for loadpopup");
		return;
	}
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 255, 220, 0));
	texture = SDL_CreateTextureFromSurface(loadpopup->mode->program->renderer, s);
	if (texture == NULL) {
		loadpopup->mode->program->errorPrompt("could not create texture for scrollbar for loadpopup");
		cout << "\nerror:" << SDL_GetError();
		return;
	}
	SDL_FreeSurface(s);
	cout << "\n init";
}