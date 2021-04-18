#ifndef PopUps_h
#define PopUps_h
#include "WindowLayer.h"
#include "Includes.h"
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
	int topFileIndexInScroll = 0;
	int indexOfCurrentTexture = 0;
	LoadPopUp(Mode* _mode, int);
	~LoadPopUp() {
		std::cout << "\n dctr load";
	};
	struct Files {
		int yPosOfTotalHeight = 0;
		int markedFileIndex = -1;
		int selectedFileIndex = -1;
		std::vector<std::string> files;
		std::string path;
		Files(std::string);
	};
	Files* files = nullptr;
	struct Scrollbar {
		int yPosWhereMouseWasClicked = 0;
		int status = 0; // 0= default, 1 = clicked on
		LoadPopUp* loadpopup;
		SDL_Texture* texture;
		SDL_Rect rect;
		Scrollbar(LoadPopUp*);
		void init();
	};
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
	void scrollFiles(double);
	bool updateFileList();
};

#endif // !PopUps_h
