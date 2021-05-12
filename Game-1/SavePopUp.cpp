#include "PopUps.h"
#include "Button.h"
#include "Mode.h"
#include <filesystem>
#include "WindowLayer.h"

SavePopUp::SavePopUp(std::string _title, Mode* _mode, int _index) : LoadPopUp(_title, _mode, _index) {

	cout << "\nsave popup";

}

void SavePopUp::createSaveAsBtn() {
	//new Button("", mode->program );
	buttons.emplace_back("Save As", mode->program, "1/36" );
	buttons[buttons.size() - 1].doAction = [this]() { this->onSaveAs(); };
}

void SavePopUp::onSaveAs() {
	cout << "\nsave as clicked" << this->textInputField->tmpText;
	std::string filepath =  this->textInputField->tmpText;
	bool fileExists = std::filesystem::exists(filepath.c_str());

	if (fileExists) {
		std::string text = "Overwrite " + this->textInputField->tmpText + "?";
		ConfirmPrompt* confirm = new ConfirmPrompt(mode, mode->winLayers.size(), text, [this]() { this->saveAs(); });
		confirm->start();
		WindowLayer* w = confirm;
		mode->winLayers.emplace_back(w);
	}
	else {
		saveAs();
	}
	//Check if that file already exists, if it does user has to confirm that we can overwrite that file

}
void SavePopUp::saveAs() {
	stop();
	cout << "\n saved to dir";
}