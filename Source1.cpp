#include <stdio.h>
#include <windows.h>
#include <time.h>
#define scount 80
#define screen_x 80
#define screen_y 25

HANDLE rHnd;
HANDLE wHnd;
DWORD fdwMode;
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 }; //fill full screen
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
CHAR_INFO consoleBuffer[screen_x * screen_y];
COORD star[scount];

int setConsole(int x, int y)
{
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}

int setMode()
{
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT |
		ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

void setcursor(bool visible) {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

void init_star() //set up position star[]
{
	int i;
	for (i = 0; i < scount; i++) {
		star[i].X = rand() % screen_x;
		star[i].Y = rand() % screen_y;
	}
}

void star_fall() //change position y of star 
{
	int i;
	for (i = 0; i < scount; i++) {
		if (star[i].Y >= screen_y - 1) {
			star[i] = { SHORT(rand() % screen_x),1 };
		}
		else {
			star[i] = { star[i].X, SHORT(star[i].Y + 1) };
		}
	}
}

void clear_buffer()
{
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 0;
		}
	}
}

void fill_star_to_buffer()
{
	int i;
	for (i = 0; i < scount; i++) { //declar position x,y * in array
		consoleBuffer[star[i].X + screen_x * star[i].Y].Char.AsciiChar = '*';
		consoleBuffer[star[i].X + screen_x * star[i].Y].Attributes = 7;
	}

}

int sx, sy;
int color = 7;
void fill_ship_to_buffer()
{
	consoleBuffer[sx + screen_x * sy].Char.AsciiChar = 'A';
	consoleBuffer[sx + screen_x * sy].Attributes = color;

}

void fill_buffer_to_console()
{
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos,
		&windowSize);
}

int hit = 0;
bool ship_hit_star() {
	for (int i = 0; i < scount; i++) {
		if (sx == star[i].X && sy == star[i].Y) {
			hit++;
			star[i].X = rand() % 80;
			star[i].Y = 1;
			if (hit == 10) {
				return true; //1	
			}
		}
	}
	return false; //0
}

int main()
{
	srand(time(NULL));
	setConsole(screen_x, screen_y);
	setMode();
	setcursor(0);
	init_star();
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	while (play)
	{
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				//------ keyboard event
				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'c') {
						color = rand() % 255;
					}
				}
				//------ mouse event
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					//------ left click
					if (eventBuffer[i].Event.MouseEvent.dwButtonState &
						FROM_LEFT_1ST_BUTTON_PRESSED) {
						color = rand() % 255;
					}
					//------ mouse move
					else if (eventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) {
						sx = posx;
						sy = posy;
					}
				}
			}
			delete[] eventBuffer;
		}
		star_fall();
		clear_buffer();
		fill_star_to_buffer();
		fill_ship_to_buffer();
		if (ship_hit_star()) { //hit 10 stars = true then play = false
			play = false;
		}
		fill_buffer_to_console();
		Sleep(100);
	}
	return 0;
}