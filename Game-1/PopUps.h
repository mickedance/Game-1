#ifndef PopUps_h
#define PopUps_h
#include "WindowLayer.h"
#include "Includes.h"
#include <chrono>
struct Mode;
struct Button;
struct LoadPopUp: public WindowLayer  {
	SDL_Rect dstRect;
	SDL_Rect srcRect;
	std::vector<Button> buttons;
	SDL_Texture* texture = nullptr;
	SDL_Texture* textureForFiles = nullptr;
	SDL_Rect fileListDstRect;
	SDL_Rect fileListSrcRect;
	
	int heightOfEachFileTexture = 0;
	int nrOfFilesOnTextureNow = 0;
	int topFileIndexInScroll = 0;
	int indexOfCurrentTexture = 0;
	LoadPopUp(Mode* _mode, int);
	~LoadPopUp() {
		std::cout << "\n dctr load";
	};
	struct Files {
		int yPosOfTotalHeight = 0;
		int selectedFileIndex = -1;
		int mouseOverIndex = -1;
		std::vector<std::string> files;
		std::string path;
		Files(std::string);
	};
	Files* files = nullptr;
	struct Scrollbar {
		int yPosWhereMouseWasClicked = 0;
		int status = 0; //  0= default, 1 = mouse over, 2= clicked on
		LoadPopUp* loadpopup;
		SDL_Texture* texture;
		SDL_Rect rect;
		Scrollbar(LoadPopUp*);
		void init();
	};
	struct TextInputField {
		int status = 0; // 0 = default, 1 = mouse over, 2= mouse down, 3 = active
		LoadPopUp* loadpopup = nullptr;
		int widthOfInputField = 0;
		SDL_Rect dstRect;
		SDL_Rect srcRect;
		SDL_Rect textMarkerRect;
		SDL_Texture* textMarkerTexture = nullptr;
		bool markerIsVisible = false;
		std::chrono::time_point<std::chrono::steady_clock> markerTimerStart;
		SDL_Texture* texture = nullptr;
		std::string text;
		std::string tmpText;
		int posFromEnd = 0;
		void update(std::string);
		void initTextMarker();
		TextInputField(LoadPopUp* _loadpopup) {
			loadpopup = _loadpopup;
		}
	};
	TextInputField* textInputField = nullptr;
	Scrollbar* scrollbar;
	virtual void start();
	virtual void stop();
	virtual void render();
	virtual void userEvents(SDL_Event);
	void createCancelBtn();
	void createLoadBtn();
	void initBtns();
	void createButton(std::string , std::string , std::function<void()>);
	void onCancelBtn();
	void onLoadBtn();
	void loadFiles();
	void scrollFiles(double, int);
	bool updateFileList();
	void updateScrollbarPos(float);
};

#endif // !PopUps_h
