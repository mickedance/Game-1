#ifndef  Inputs_h
#define Inputs_h
struct Mode;

struct Inputs {
	Inputs();
	bool handleEvents(Mode*);
};

#endif // ! Inputs_h
