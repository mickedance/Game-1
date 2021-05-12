#include "PopUps.h"
#include "Mode.h"
#include "Program.h"
#include "Button.h"
#include <filesystem>

using std::cout;
LoadPopUp::LoadPopUp(std::string _title, Mode* _mode, int _index): WindowLayer(_mode, _index) {
	title = _title;
}
void LoadPopUp::stop() {
	//Destroy texture of buttons and popup's texture

	for (auto& b : buttons) {
		SDL_DestroyTexture(b.texture);
		b.texture = nullptr;
	}
	buttons.clear();
	SDL_DestroyTexture(texture);
	texture = nullptr;
	// Deleete files
	delete files;
	SDL_DestroyTexture(textureForFiles);
	//Delete scrollbar
	SDL_DestroyTexture(scrollbar->texture);
	delete scrollbar;
	scrollbar = nullptr;
	// Delete TextInputField
	if (textInputField->texture != NULL) {
		SDL_DestroyTexture(textInputField->texture);
		textInputField->texture = nullptr;
	}if (textInputField->textMarkerTexture != NULL) {
		SDL_DestroyTexture(textInputField->textMarkerTexture);
		textInputField->textMarkerTexture = nullptr;
	}
	//Delete and remove this popup from winLayers list 
	WindowLayer* ptr = mode->winLayers.at(index);
	mode->winLayers.erase(mode->winLayers.begin() + index );
	delete ptr;
	textInputField = nullptr;
	
}

void LoadPopUp::start() {
	//
	//First create a design of this load popup, without list of files
	//Then we load files
	
	Program* program = mode->program;
	// Title of popup
	SDL_Color textColor = { 255,255,255,255 };
	SDL_Surface* s = TTF_RenderText_Blended(program->fonts[1], title.c_str(), textColor );
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
	//Create textinput field-bg
	SDL_Rect rect;
	rect.w = (int) (blackBoxRect.w * .7);
	rect.h = (int) (heightOfEachFileTexture * 1.5);
	rect.x =blackBoxRect.x;
	rect.y = dstRect.y + blackBoxRect.h - ( rect.h/2 );

	tmpS = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, rmask, gmask, bmask, amask);
	if (tmpS == NULL) {
		program->errorPrompt("Could not create surface for textinput field box pop up bg");
		return;
	}
	SDL_FillRect(tmpS, NULL, SDL_MapRGB(tmpS->format, 0, 0, 0));
	SDL_Texture* textInputFieldTexture = SDL_CreateTextureFromSurface(program->renderer, tmpS);
	if (textInputFieldTexture == NULL) {
		program->errorPrompt("Could not create texture for textinput field on pop up ");
		return;
	}
	SDL_FreeSurface(tmpS);
	//Create editable text for textfield
	int marginRL = 2;

	// textinputfield
	textInputField = new TextInputField(this);
	textInputField->dstRect.x = dstRect.x + rect.x + marginRL;
	textInputField->dstRect.y = rect.y + dstRect.y + rect.h/2 - heightOfEachFileTexture/2;
	textInputField->dstRect.w = rect.w - marginRL * 2;
	textInputField->dstRect.h = heightOfEachFileTexture;
	textInputField->widthOfInputField = textInputField->dstRect.w;
	textInputField->initTextMarker();

	textInputField->srcRect.x = 0;
	textInputField->srcRect.y = 0;
	textInputField->srcRect.h = heightOfEachFileTexture;
	int widthOfTextTexture;
	SDL_QueryTexture(textInputField->texture, NULL, NULL, &widthOfTextTexture, NULL);
	//Make short text have good textfont as well
	if (widthOfTextTexture <= textInputField->dstRect.w) {
		cout << "\nyeah!";
		textInputField->dstRect.w = widthOfTextTexture;
		textInputField->srcRect.w = widthOfTextTexture;
	}
	//init marker

	//set bg color of popupwindow
	SDL_SetRenderDrawColor(program->renderer, 155, 55, 5, 255);
	
	SDL_SetRenderTarget(program->renderer, texture);
	SDL_RenderFillRect(program->renderer, NULL);
	//Render title, black box etc on popup-window/box
	SDL_RenderCopy(program->renderer, blackBoxTexture, NULL, &blackBoxRect);
	SDL_RenderCopy(program->renderer, textInputFieldTexture, NULL, &rect);
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
	cout << "\nload start";
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
	return true;
}
void LoadPopUp::createCancelBtn() {
	createButton("Cancel", "1/36", [this]() { this->onCancelBtn(); });
	
}
void LoadPopUp::createButton(std::string title, std::string size, std::function<void()> func) {
	buttons.emplace_back(title, mode->program, "1/36");
	buttons.at(buttons.size() - 1).doAction = func;
	cout << "\n  new btn"<< title;
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
	if (textInputField!=nullptr && textInputField->texture != NULL) {
		SDL_RenderCopy(mode->program->renderer, textInputField->texture, &textInputField->srcRect, &textInputField->dstRect );
		
	}
	if (textInputField!=NULL && textInputField->status == 3 ) {
		
		//SDL_Delay(120);
		auto now = std::chrono::steady_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - textInputField->markerTimerStart ).count();
		if ((int)duration > 540) {
			textInputField->markerTimerStart = now;
			textInputField->markerIsVisible = !textInputField->markerIsVisible;
		}
		if(textInputField->markerIsVisible && textInputField->status==3 && textInputField->textMarkerTexture != nullptr)
			SDL_RenderCopy(mode->program->renderer, textInputField->textMarkerTexture, NULL, &textInputField->textMarkerRect);
	}
}
void LoadPopUp::userEvents(SDL_Event e) {

	//Handle buttons
	bool stop = false;
	if(scrollbar->status==0)
		if (textInputField->status == 0 || textInputField->status==3) {
			int status = mode->handleBtns(&buttons, &e);
			if (status >= 0)
				stop = true;
			
		}
	if (stop)
		return;
	cout << "\nafter";
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
					//Set filename to textinputfield as well
					textInputField->tmpText = files->files[i];
					textInputField->update(files->files[i]);
					break;
				}
			}
		}
		//See if user clicked on inputtextfield
		else if (textInputField != nullptr && e.motion.x>= textInputField->dstRect.x  && e.motion.x<= textInputField->dstRect.x + textInputField->widthOfInputField
			&& e.motion.y>= textInputField->dstRect.y && e.motion.y<= textInputField->dstRect.y + textInputField->dstRect.h
			) {
			textInputField->status = 2;

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
			if (textInputField != nullptr && textInputField->status != 0 ) {
				cout << "\nchange";
				if (textInputField->status == 3) {
					textInputField->srcRect.x = 0;
				}
				textInputField->status = 0;
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
		}
		//  Mouse i outside list of files (textureForFiles)
		else {	
			//We unmark file that mouse was over
			if (files->mouseOverIndex != -1) {
				files->mouseOverIndex = -1;
				updateFileList();
			}
			// Textunputfield
			if (textInputField != nullptr && files->mouseOverIndex == -1 ) {
				//Check if mouse is over textinputfield
				if (e.motion.x >= textInputField->dstRect.x && e.motion.x <= textInputField->dstRect.x + textInputField->widthOfInputField
					&& e.motion.y >= textInputField->dstRect.y && e.motion.y <= textInputField->dstRect.y + textInputField->dstRect.h) {
					if (textInputField->status != 1 ) {
						if(textInputField->status!=3)
							textInputField->status = 1;
						SDL_SetCursor(mode->program->ibeamCursor);
					}
				}//Outside of textinputfield
				else {
					if (textInputField->status != 0 ) {

						if( textInputField->status != 3)
							textInputField->status = 0;
						SDL_SetCursor(mode->program->cursor);
					}
				}
			}
			
		}
		
	}
	if (e.type == SDL_MOUSEWHEEL) {
		scrollFiles(0, e.wheel.y*-1);
	}
	if (e.type == SDL_MOUSEBUTTONUP) {
		//If user clicked inside textinputfield we have to enable user to edit the text of filename
		if (textInputField != nullptr && textInputField->status == 2) {
			textInputField->status = 3;
			cout << "\nclickup";
			//Make textmarker visible by changing/restarting its startime and it's visibility 
			textInputField->markerTimerStart = std::chrono::steady_clock::now() -std::chrono::milliseconds(5000);
			textInputField->markerIsVisible = false;
			SDL_QueryTexture(textInputField->texture, NULL, NULL, &textInputField->srcRect.x, NULL);
			textInputField->srcRect.x -= textInputField->dstRect.w;
		}//outside of textinputfield
		else if(textInputField != nullptr){
			textInputField->status = 0;
		}
		// disable scroll of scrollbar
		if (scrollbar!=nullptr && scrollbar->status == 1)
			scrollbar->status = 0;
	}


	/*
							KEYBOARD EVENTS!!!!

	*/
	if (e.type == SDL_TEXTINPUT) {
		// textfield is editable/has status as active
		if (textInputField->status == 3) {
			std::string text = textInputField->tmpText;
			text.insert((int)textInputField->tmpText.size() - textInputField->posFromEnd, e.text.text);
			textInputField->update(text);
			textInputField->markerIsVisible = true;
		}
	}
	if (e.type == SDL_KEYDOWN) {
		if(textInputField->status==3 && e.key.keysym.sym == SDLK_BACKSPACE && textInputField->tmpText.size() > 0){
			textInputField->update( textInputField->tmpText.erase( textInputField->tmpText.size() - textInputField->posFromEnd-1 ,1 )  ) ;
		}
		else if (textInputField->status == 3 && e.key.keysym.sym == SDLK_DELETE && textInputField->tmpText.size()>0 && textInputField->posFromEnd!=0) {
			if (textInputField->posFromEnd >0)
				textInputField->posFromEnd--;
			textInputField->update(textInputField->tmpText.erase(textInputField->tmpText.size() - textInputField->posFromEnd-1, 1));

		}
		//change pos of textmarker in text with arrow Left and Right key
		else if (textInputField->status == 3 && e.key.keysym.sym == SDLK_LEFT) {
			if(textInputField->posFromEnd < (int)textInputField->tmpText.size())
			textInputField->posFromEnd++;
			textInputField->update(textInputField->tmpText);
		}
		else if (textInputField->status == 3 && e.key.keysym.sym == SDLK_RIGHT) {
			if(textInputField->posFromEnd>0)
				textInputField->posFromEnd--;
			textInputField->update(textInputField->tmpText);
		}
	}
}

void LoadPopUp::onCancelBtn() {
	stop();
}
void LoadPopUp::onLoadBtn() {
	//cout << "\nonload: "<< files->selectedFileIndex<<", "<<  files->files[files->selectedFileIndex];
	
}
void LoadPopUp::TextInputField::initTextMarker() {
	update("");
	Uint32 rmask, gmask, bmask, amask;
	loadpopup->mode->program->getRGBAmask(&rmask, &gmask, &bmask, &amask);
	textMarkerRect.w = 1;
	
	SDL_QueryTexture(texture, NULL, NULL,  &textMarkerRect.x, NULL);
	textMarkerRect.w  =1 ;
	textMarkerRect.h = loadpopup->textInputField->dstRect.h ;
	textMarkerRect.y = dstRect.y;
	textMarkerRect.x = dstRect.x ;
	//SDL_QueryTexture(texture, NULL, NULL, &textMarkerRect.x, NULL);
	SDL_Surface* s =SDL_CreateRGBSurface(0, textMarkerRect.w, textMarkerRect.h, 32,  rmask, gmask, bmask, amask);
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 0, 230, 0));
	if (s == NULL) {
		cout << "\nerr:  " << SDL_GetError();
		loadpopup->mode->program->errorPrompt("Could not create surface for ibeam in loadpopup");
		return;
	}
	textMarkerTexture = SDL_CreateTextureFromSurface(loadpopup->mode->program->renderer, s);
	if (textMarkerTexture == NULL) {
		loadpopup->mode->program->errorPrompt("Could not create texture for ibeam in loadpopup");
		return;
	}
	SDL_FreeSurface(s);
}
void LoadPopUp::TextInputField::update(std::string _tmpText) {
	tmpText = _tmpText;
	//Update the text 
	SDL_Color color = { 255,255,255 };
	if (texture != NULL)
		SDL_DestroyTexture(texture);
	SDL_Surface* s = TTF_RenderText_Blended(loadpopup->mode->program->fonts[2], tmpText.c_str(), color);
	texture = SDL_CreateTextureFromSurface(loadpopup->mode->program->renderer, s);
	SDL_FreeSurface(s);

	//Update width of textures Rect
	int newWith;
	SDL_QueryTexture(texture, NULL, NULL, &newWith, NULL);
	if (newWith < widthOfInputField) {
		//if newwith is smaller than textinputfields width
		srcRect.w = newWith;
		dstRect.w = newWith;
		srcRect.x = 0;
	}
	else {
		//if newwith is greater or equal to textinputfields width
		// We crop text to textinputfields width and make last char visible by scrolling and chaning x-Pos
		srcRect.w = widthOfInputField;
		dstRect.w = widthOfInputField;
		srcRect.x = newWith - widthOfInputField;
	}
	//Update markerPosition
	int fontW;
	std::string textToCheckWidth = tmpText.substr(0, tmpText.size() - posFromEnd);
	TTF_SizeText(loadpopup->mode->program->fonts[2], textToCheckWidth.c_str(), &fontW, NULL);
	textMarkerRect.x = dstRect.x +fontW+1 - srcRect.x;
	//Prevent marker from going outside of blackbox to left
	if (textMarkerRect.x <= dstRect.x) {
		 int widthToRigthOfMarker;
		 std::string t = tmpText.substr(tmpText.size() - posFromEnd, tmpText.size());
		 TTF_SizeText(loadpopup->mode->program->fonts[2], t.c_str(), &widthToRigthOfMarker, NULL);
		 int widthWholeText;
		 SDL_QueryTexture(texture, NULL, NULL, &widthWholeText, NULL);
		 srcRect.x = widthWholeText - widthToRigthOfMarker;
		 textMarkerRect.x = dstRect.x;
	}
	markerTimerStart = std::chrono::steady_clock::now() - std::chrono::milliseconds(5000);
	markerIsVisible = false;
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