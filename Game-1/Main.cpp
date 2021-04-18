#include "includes.h"
#include "Program.h"

int main(int argc, char* args[]) {

	Program* program = new Program();

	program->run();

	delete program;

	return 0;
}