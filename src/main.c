#include "hive.h"

Hive hive;

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	setlocale(LC_ALL, "");
	initscr();
	noecho();
	cbreak();
	mousemask(ALL_MOUSE_EVENTS, NULL);
	mouseinterval(0);
	keypad(stdscr, TRUE);
	start_color();
	init_pair(HIVE_PAIR_BLACK, COLOR_RED, COLOR_BLACK);
	init_pair(HIVE_PAIR_WHITE, COLOR_BLUE, COLOR_BLACK);
	init_pair(HIVE_PAIR_SPACE, COLOR_WHITE, COLOR_BLACK);
	init_pair(HIVE_PAIR_BLACK_BLACK, COLOR_RED, COLOR_RED);
	init_pair(HIVE_PAIR_BLACK_WHITE, COLOR_RED, COLOR_BLUE);
	init_pair(HIVE_PAIR_BLACK_SPACE, COLOR_RED, COLOR_WHITE);
	init_pair(HIVE_PAIR_WHITE_BLACK, COLOR_BLUE, COLOR_RED);
	init_pair(HIVE_PAIR_WHITE_WHITE, COLOR_BLUE, COLOR_BLUE);
	init_pair(HIVE_PAIR_WHITE_SPACE, COLOR_BLUE, COLOR_WHITE);
	init_pair(HIVE_PAIR_SPACE_BLACK, COLOR_WHITE, COLOR_RED);
	init_pair(HIVE_PAIR_SPACE_WHITE, COLOR_WHITE, COLOR_BLUE);
	init_pair(HIVE_PAIR_SPACE_SPACE, COLOR_WHITE, COLOR_WHITE);
	refresh();

	hive_init(&hive);
	hive_print(&hive);

	while (1) {
		MEVENT event;
		int c;
		c = getch();
		if (c == 'q')
			break;
		if (hive_handle(&hive, c))
			if (c == KEY_MOUSE)
				getmouse(&event);
	}
	endwin();

	return 0;
}