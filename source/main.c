// bfm-switch brainfuck interpreter //
// credits to: team switchbrew for libnx, and thekgg for his brainfuck //
// homebrew that i was able to use as a reference //
// copyright (c) 2018 isy //
// under the GNU General Public License v3 (see LICENSE.md for more info) //

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define ARRSIZE 30000

#include <switch.h>

char* input;
char* array;
int size = 0;
int counter = 0;
int pluscount = 0;
int minuscount = 0;
int loopmode = 0;
int beginloop = 0;
int inputbuff = 0;

int interpret(char x, int i) {
	switch (x) {
		case '>': // move cell pointer (the counter int) left
			++counter;
			break;
		case '<': // move cell pointer (the counter int) left
			--counter;
			break;
		case '+': // add 1 to current cell
			++(array[counter]);
			break;
		case '-': // subtract 1 from current cell
			--(array[counter]);
			break;
		case '.': // print contents of current cell in ASCII
			printf("%c", array[counter]);
			break;
		case ',': // ask for one byte of input
			printf("use the A and B buttons to choose the number you want to input, and - to submit it.\n%d", inputbuff);
			while(1) {
				hidScanInput();
				u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				if (kDown & KEY_A) {
					++inputbuff;
					printf("\b%d", inputbuff);
				}
				else if (kDown & KEY_B) {
					--inputbuff;
					printf("\b%d", inputbuff);
				}
				else if (kDown & KEY_MINUS)
					break;
			}
			array[counter] = inputbuff;
			inputbuff = 0;
			break;
		case '[': // begin while (array[counter] == 0) loop
			loopmode = 1;
			beginloop = i;
			break;
		case '*': // sleeps for 1 second
			svcSleepThread(1000000000);
			break;
		case '@': // frees array
		  	memset(array, 0, ARRSIZE);
		  	printf("tape cleared.\n");
      		  	break;
		case '^': // print current cells numerical value
			printf("%d\n", array[counter]);
			break;
		case '_': // subtract 10 from current cell
			while (minuscount < 10) {
				--array[counter];
				++minuscount;
			}
			minuscount = 0;
			break;
		case '=': // add 10 to current cell
			while (pluscount < 10) {
				++array[counter];
				++pluscount;
			}
			pluscount = 0;
			break;
		case '/': // clear screen
			printf("\x1b[2J");
			break;
		default: // else
			break;
	}
	return 0;
}

// the code responsible for passing the file on to interpret() and doing weird loop shit
void runbrain(char* code, int size) {
	for (int i = 0; i < size; ++i) {;
		interpret(code[i], i);
		if (loopmode) {
			if (code[i] == ']') {
				if (array[counter]) {
					i = beginloop;
				} 
        else
					loopmode = 0;
			}
		}
	}
	puts("");
}

void runfile() {
	// let's read the file
	FILE* myfile = fopen("main.bf", "rb");

	if (myfile == NULL) {
		printf("Could not load file main.bf, using testfile instead.\n");
		myfile = "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.****@++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.**/@,.";
	}

	// obtain file size
	fseek(myfile, 0, SEEK_END);
	int filesize = ftell(myfile);
	rewind(myfile);

	// load the file into "input"
	input = (char*)malloc(filesize+1);
	fread(input, 1, filesize, myfile);
	input[filesize] = '\0';
	fclose(myfile);
	//printf("Now input contains: %s\n", input);

	array = (char*)calloc(1, ARRSIZE);
	//int size = sizeof(input)-1;
	int size = filesize;

	runbrain(input, filesize);
}

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(NULL);

    runfile();

    // Main loop
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        gfxFlushBuffers();
        gfxSwapBuffers();
        gfxWaitForVsync();
    }
    free(array);
    gfxExit();
    return 0;
}
