#ifndef  ModeEditor_h
#define ModeEditor_h

#include "Includes.h"
#include "Mode.h"

struct Program;
struct Button;

struct ModeEditor : public Mode {
	std::vector<Button> buttons;
	ModeEditor(Program*);
	virtual void start();
	virtual void render();
	virtual void userEvents(SDL_Event);
	virtual void stop();
	void createNewButton(std::string , std::string , std::function<void()>);
	void onLoadPopUp();
	void onSavePopUp();
	void onMenu();
	void onSave();
	void startLoadPopUp();
	void startSaveAsPopUp();
	void toggleDropdown();
	void createMenuClickItem(std::string, SDL_Rect, Program*, bool, std::function<void()>);
	struct Scrollbar {
		Scrollbar() {};
		void init(Program*);
		int status = 0; // 0 = defaul, 1 = mouse over
		int mouseClickedXPos = 0;
		SDL_Rect dstRect;
		SDL_Texture* texture;
	};
	std::vector<Scrollbar> scrollbars;
	struct Dropdown {
		void loadAndPostionItem(SDL_Texture*, int, bool);
		bool active = false;
		ModeEditor* modeEditor;
		Dropdown(ModeEditor*);
		SDL_Rect dstRect;
		SDL_Texture* texture = nullptr;
		int xScrollPos = 0;
		struct LevelItem {
			bool clickable = false;
			int status = 0;
			LevelItem(SDL_Texture* _t, bool _clickable) {
				clickable = _clickable;
				texture_ptr = _t;
			}
			SDL_Texture* texture_ptr;
			SDL_Rect dstRect;
		};
		std::vector<LevelItem> levelItems;
	};
	struct MenuClickItem {
		bool clickable = false;
		int status = 0; //0 = default, 1= mouse over
		SDL_Rect dstRect;
		SDL_Texture* texture = nullptr;
		MenuClickItem(SDL_Texture*, SDL_Rect, std::function<void()>, bool);
		std::function<void()> doAction;  
	};
	std::vector<MenuClickItem>  menuClickItems;
	Dropdown* dropdown = nullptr;
};


#endif // ! ModeEditor_h
