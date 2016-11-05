#include <iostream>
#include <limits>
#include "othello_cut.h" // won't work correctly until .h is fixed!
#include "utils.h"

#include <unordered_map>

using namespace std;

int main(int argc, const char **argv) {
	state_t state;
	state.print(cout,0);
	// for (int i = 0; i < 36; ++i){
	
	// printf("%d\n",state.is_free(i) ); 
	// }
	state.set_color(true,11);
	state.print(cout,0);
	state = state.move(true, 28);

	// state.set_color(true,28);
	// state.print(cout,0);
	// state = state.move(true, 11);
	
	state.set_color(false,25);
	state = state.move(false,14);
	
	// state = state.move(true, 11);
	// state.print(cout,1);
	// state = state.move(false, 29);
	// state.print(cout,1);
	state.print(cout,1);

}