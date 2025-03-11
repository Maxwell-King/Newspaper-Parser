#include "../include/filesys_interaction.h"

// 1) get file from directory
// 2) call child process
// 3) user interaction
// 4) train from user data

int main ( int argc, char *argv[] ) {
	if ( argc < 2 ) {
		fprintf(stderr, "usage:\theadlinereader <path/to/directory_containing_pictures>");
		return -1;
	}
	set_dir(argv[1]);
	while (1) {
		watch_dir(); 
	}
}
