#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void initial();
#define StartX 1
#define StartY 1

int main(void)
{
	int x = StartX;
	int y = StartY;
	int ch;
	initial();
	box(stdscr, '|', '-');
	attron(A_REVERSE);
	mvaddstr(0, 20, "curses program");
	attroff(A_REVERSE);
	move(y, x);
	do
	{
		ch = getch();
		switch(ch)
		{
			case KEY_UP:
				--y;
				break;
			case KEY_DOWN:
				++y;
				break;
			case KEY_RIGHT:
				++x;
				break;
			case KEY_LEFT:
				--x;
				break;
			case '\t':
				x += 7;
				break;
			case KEY_BACKSPACE:
				mvaddch(y, --x, ' ');
				break;
			case '\r':
				++y;
				x=0;
				break;
			case 27:
				endwin();
				exit(1);
			default:
				addch(ch);
				x++;
				break;
		}
		move(y, x);
	}while(1);
	return 0;
}

void initial()
{
	initscr();
	cbreak();
	nonl();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	refresh();
}
