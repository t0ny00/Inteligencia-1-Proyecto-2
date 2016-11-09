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
	state.set_color(false,0);
	state.set_color(false,3);
	state.set_color(false,4);
	state.set_color(false,10);
	state.set_color(false,11);
	state.set_color(false,15);
	state.set_color(false,19);
	state.set_color(false,23);
	state.set_color(false,25);
	state.set_color(false,35);
	state.set_color(false,24);
	state.set_color(false,25);
	state.set_color(false,28);
	state.set_color(false,29);

	state.set_color(true,1);
	state.set_color(true,2);
	state.set_color(true,5);
	state.set_color(true,6);
	state.set_color(true,7);
	state.set_color(true,12);
	state.set_color(true,13);
	state.set_color(true,14);
	state.set_color(true,16);
	state.set_color(true,17);
	state.set_color(true,18);
	state.set_color(true,20);
	state.set_color(true,21);
	state.set_color(true,22);
	state.set_color(true,26);
	state.set_color(true,27);
	state.set_color(true,30);
	state.set_color(true,31);
	state.set_color(true,32);
	state.set_color(true,33);
	state.set_color(true,34);

	state.print(cout,0);
	printf("%d\n",state.terminal() );
	for (int i = 0; i < DIM; ++i)
    {
        if (state.is_white_move(i)){
            printf("HOLA\n");;
        }
    }
	// state = state.move(false,12);
	// printf("%d\n",state.value() );

}