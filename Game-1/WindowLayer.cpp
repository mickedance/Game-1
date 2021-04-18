#include "WindowLayer.h"
#include "Mode.h"
#include "Includes.h"

using std::cout;
WindowLayer::WindowLayer(Mode* _mode, int _index) {
	index = _index;
	mode = _mode;
}
