#include <curses.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	int ch;
	char string1[80], string2[80];
	ch = getch();
	initscr();
	echo();
	attron(A_REVERSE);
	mvaddstr(9, 10, "Hello,world");
	attroff(A_REVERSE);
	move(0,0);
	attron(A_BLINK);
	ch = getch();
	getstr(string1);
	scanw("%s", string2);
	mvprintw(10, 10, "String1=%s", string1);
	mvprintw(11, 10, "String2=%s", string2);
	box(stdscr, '|', '-');
	refresh();
	attroff(A_BLINK);
	sleep(1);
	endwin();
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
