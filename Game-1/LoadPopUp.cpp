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
	for (auto& b : buttons) {
		SDL_DestroyTexture(b.texture);
		b.texture = nullptr;
	}
	SDL_DestroyTexture(texture);
	texture = nullptr;
	// Deleete files
	delete files;
	SDL_DestroyTexture(textureForFiles);
	//Delete scrollbar
	SDL_DestroyTexture(scrollbar->texture);
	delete scrollbar;
	//Delete and remove this popup from winLayers list 
	WindowLayer* ptr = mode->winLayers.at(index);
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

	// set bg color for list of files (Black box)
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
	SDL_FillRect(tmpS, NULL, SDL_MapRGB(tmpS->format, 0, 0, 0));
	SDL_Texture* blackBoxTexture = SDL_CreateTextureFromSurface(program->renderer, tmpS);
	if (blackBoxTexture == NULL) {
		program->errorPrompt("Could not create texture for black box pop up bg");
		return;
	}
	SDL_FreeSurface(tmpS);
	//set bg color of popupwindow
	SDL_SetRenderDrawColor(program->renderer, 155, 55, 5, 255);
	
	SDL_SetRenderTarget(program->renderer, texture);
	SDL_RenderFillRect(program->renderer, NULL);
	//Render title, black box etc on popup window/box
	SDL_RenderCopy(program->renderer, blackBoxTexture, NULL, &blackBoxRect);
	SDL_RenderCopy(program->renderer, title, NULL, &titleRect);
	SDL_SetRenderTarget(program->renderer, nullptr);
	SDL_DestroyTexture(title);
	SDL_DestroyTexture(blackBoxTexture);

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
	scrollbar = new Scrollbar(this);
	scrollbar->init();
}
void LoadPopUp::scrollFiles(double percent) {
	/*
		   Here we scroll the list of files.
		   We take a percentage of how much to scroll
		   And since we don't want the textureForFiles to have infinity height we just render textureForFiles with a height with filetextures as height as
		   windows height.
		   So we therefore needs to update textureForFiles when there's files/textures that needs to be displayed
	*/
	if (files->files.size() < 1 )
		return;

	
	int heightOfBigTextureOfFiles;
	SDL_QueryTexture(textureForFiles,NULL, NULL, nullptr, &heightOfBigTextureOfFiles);
	files->yPosOfTotalHeight = (int)((double)percent * ( (double)files->files.size() * heightOfEachFileTexture - heightOfBigTextureOfFiles+fileListSrcRect.h)   ) ;
	int nrOfFilesThatFitsHeghtOfBigTexture(heightOfBigTextureOfFiles / heightOfEachFileTexture);
//  13 = 294 - index(589-288 =301 ) + (589-288 - 288)*index 
	int smallPosY = files->yPosOfTotalHeight - indexOfCurrentTexture*(heightOfBigTextureOfFiles - fileListSrcRect.h);
	cout << "\npercent" << percent;
	fileListSrcRect.y = smallPosY;
	//cout << "\n "<<"; ypos:"<< yPosOfTotalHeight<<", " << heightOfBigTextureOfFiles <<", srch"<< fileListSrcRect.h;

	//Scroll down
	if (files->yPosOfTotalHeight - indexOfCurrentTexture*( heightOfBigTextureOfFiles - fileListSrcRect.h )  + fileListSrcRect.h >= heightOfBigTextureOfFiles ) {
		cout << "\nnow";
		for (int i = 0; i < (int)files->files.size(); i++) {
			if ((i + 1) * heightOfEachFileTexture + fileListDstRect.h - indexOfCurrentTexture* (heightOfBigTextureOfFiles - fileListSrcRect.h) > heightOfBigTextureOfFiles) {
				cout << "\n" << files->files[i];
				topFileIndexInScroll = i;
				updateFileList();
				indexOfCurrentTexture++;
				fileListSrcRect.y = 0;
				break;
			}
		}
	}// Scroll down
	else if (smallPosY <= 0 && percent!=0) {
		for (int i = 0; i < (int)files->files.size(); i++) {
			if ((i) * heightOfEachFileTexture + heightOfBigTextureOfFiles  >= files->yPosOfTotalHeight + fileListDstRect.h) {
				cout << "\nstart here" << files->files[i];
				topFileIndexInScroll = i;
				fileListSrcRect.y =  287;
				updateFileList();
				indexOfCurrentTexture--;
				break;
			}
		}
	}
	
}
bool LoadPopUp::updateFileList() {
	if (topFileIndexInScroll >(int) files->files.size() || files->files.size()<1)
		return false;
	cout << "\nfrom: - " << topFileIndexInScroll;
	int wh;
	SDL_GetWindowSize(mode->program->window, NULL, &wh);
	int fontHeight;
	TTF_SizeText(mode->program->fonts[2], "", nullptr, &fontHeight);
	int nrOfFilesThatFitsWholeWindowHeight ( wh / (int)fontHeight);
	int heightOfFilesThatFitsWindowHeight = nrOfFilesThatFitsWholeWindowHeight * fontHeight;
	textureForFiles = SDL_CreateTexture(mode->program->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, fileListDstRect.w, heightOfFilesThatFitsWindowHeight);
	Program* program = mode->program;
	SDL_Color defaultColor = { 255,255,255,255 };
	SDL_Color selectedColor = { 55,55,255,255 };
	SDL_Surface* fileSurface;
	SDL_Rect dstRect;
	int marginLeft = 6;
	dstRect.x = marginLeft;
	for (size_t i = topFileIndexInScroll; i < files->files.size(); i++) {
		std::string filename = files->files[i].c_str();
		filename = filename.erase(0, files->path.size());
		if (files->markedFileIndex == i || files->selectedFileIndex == i)
			fileSurface = TTF_RenderText_Blended(program->fonts[2], filename.c_str(), selectedColor);
		else
			fileSurface = TTF_RenderText_Blended(program->fonts[2], filename.c_str(), defaultColor);
		SDL_Texture* fileTexture = SDL_CreateTextureFromSurface(program->renderer, fileSurface);
		if (fileSurface == NULL || fileTexture == NULL) {
			program->errorPrompt("Could not create surface or texture for file: " + files->files[i]);
			return false;
		}
		SDL_FreeSurface(fileSurface);
		SDL_QueryTexture(fileTexture, NULL, NULL, &dstRect.w, &dstRect.h);
		if ((int)i > topFileIndexInScroll)
			dstRect.y = dstRect.h * (i - topFileIndexInScroll);
		else {
			heightOfEachFileTexture = dstRect.h;
			dstRect.y = 0;
		}

		if ( dstRect.y + dstRect.h > heightOfFilesThatFitsWindowHeight) {
			return true;
		}
			SDL_SetRenderTarget(program->renderer, textureForFiles);
			SDL_RenderCopy(program->renderer, fileTexture, NULL, &dstRect);
			SDL_SetRenderTarget(program->renderer, nullptr);

	}
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
}
void LoadPopUp::userEvents(SDL_Event e) {
		mode->handleBtns(&buttons, &e);
		
		//
		//   Handle scrollbar
		//
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			//Scrollbar
			if (e.motion.x >= scrollbar->rect.x && e.motion.x <= scrollbar->rect.x + scrollbar->rect.w
				&& e.motion.y >= scrollbar->rect.y && e.motion.y <= scrollbar->rect.y + scrollbar->rect.h) {
				if (scrollbar->status != 1) {
					scrollbar->yPosWhereMouseWasClicked = e.motion.y - scrollbar->rect.y;
					scrollbar->status = 1;
					return;
				}
			}
			// See if user selects a file
			bool userSelectedFile = false;
			for (int i = 0; i < (int)files->files.size(); i++) {
				if (e.motion.x >= fileListDstRect.x && e.motion.x <= fileListDstRect.x + fileListDstRect.w - scrollbar->rect.w
					&& e.motion.y >= fileListDstRect.y + heightOfEachFileTexture * (i)-files->yPosOfTotalHeight
					&& e.motion.y <= fileListDstRect.y + heightOfEachFileTexture * (i + 1) - files->yPosOfTotalHeight) {
					if (files->selectedFileIndex != i && scrollbar->status == 0) {
						userSelectedFile = true;
						files->selectedFileIndex = i;
						updateFileList();
						return;
					}
					break;
				}
				else {
					userSelectedFile = false;
				}
			}
			if (userSelectedFile == false) {
				cout<<"\n clicked outside",
				files->selectedFileIndex =- 1;
				updateFileList();
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
				int diff =  fileListDstRect.h - scrollbar->rect.h;
				double percent = 1;
				if (diff != 0)
					percent = ((double)scrollbar->rect.y - fileListDstRect.y ) / (double)diff;
				scrollFiles(percent);
			}
			// See If user is over a file in list
			for (int i = 0; i < (int)files->files.size(); i++) {
				if (e.motion.x >= fileListDstRect.x && e.motion.x <= fileListDstRect.x + fileListDstRect.w - scrollbar->rect.w
					&& e.motion.y>= fileListDstRect.y  + heightOfEachFileTexture*(i) - files->yPosOfTotalHeight 
					&& e. motion.y <= fileListDstRect.y + heightOfEachFileTexture*(i+1) - files->yPosOfTotalHeight ) {
					if (files->markedFileIndex != i && scrollbar->status==0 && files->selectedFileIndex==-1) {
						files->markedFileIndex = i;
						updateFileList();
					}
					break;
				}
			}
		}
		if (e.type == SDL_MOUSEBUTTONUP) {
			if (scrollbar->status == 1)
				scrollbar->status = 0;
		}
}

void LoadPopUp::onCancelBtn() {
	cout << "\ncancel";
	stop();
}
void LoadPopUp::onLoadBtn() {
	cout << "\ncancetel";
	
}

LoadPopUp::Files::Files(std::string _path) {
	path = _path;
}
LoadPopUp::Scrollbar::Scrollbar(LoadPopUp*  _loadpopup) {
	loadpopup = _loadpopup;
}
void LoadPopUp::Scrollbar::init() {
	//set size and positionn of scrollbar
	if (loadpopup->files->files.size() < 1)
		return;

	int totalHheightOfFileTextures = loadpopup->heightOfEachFileTexture * (loadpopup->files->files.size()-1) ;
	double percent = loadpopup->fileListDstRect.h /(double) (totalHheightOfFileTextures);
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
}