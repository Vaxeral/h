#include "hive.h"

const Vec3 vec3_null = { -1, -1, -1 };

int hive_init(Hive *hive)
{
	static const HivePiece defaultPieces[HIVE_PIECE_COUNT] = {
		{ .pos = (Vec3){ 1, 2 }, .piece = HIVE_WHITE | HIVE_QUEEN },
		{ .pos = (Vec3){ 2, 2 }, .piece = HIVE_WHITE | HIVE_BEETLE },
		{ .pos = (Vec3){ 2, 2 }, .piece = HIVE_WHITE | HIVE_BEETLE },
		{ .pos = (Vec3){ 3, 2 }, .piece = HIVE_WHITE | HIVE_SPIDER },
		{ .pos = (Vec3){ 3, 2 }, .piece = HIVE_WHITE | HIVE_SPIDER },
		{ .pos = (Vec3){ 4, 2 }, .piece = HIVE_WHITE | HIVE_GRASSHOPPER },
		{ .pos = (Vec3){ 4, 2 }, .piece = HIVE_WHITE | HIVE_GRASSHOPPER },
		{ .pos = (Vec3){ 4, 2 }, .piece = HIVE_WHITE | HIVE_GRASSHOPPER },
		{ .pos = (Vec3){ 5, 2 }, .piece = HIVE_WHITE | HIVE_ANT },
		{ .pos = (Vec3){ 5, 2 }, .piece = HIVE_WHITE | HIVE_ANT },
		{ .pos = (Vec3){ 5, 2 }, .piece = HIVE_WHITE | HIVE_ANT },
		{ .pos = (Vec3){ 6, 2 }, .piece = HIVE_WHITE | HIVE_LADYBUG },
		{ .pos = (Vec3){ 7, 2 }, .piece = HIVE_WHITE | HIVE_MOSQUITO },
		{ .pos = (Vec3){ 8, 2 }, .piece = HIVE_WHITE | HIVE_PILLBUG },

		{ .pos = (Vec3){ 1, 1 }, .piece = HIVE_BLACK | HIVE_QUEEN },
		{ .pos = (Vec3){ 2, 1 }, .piece = HIVE_BLACK | HIVE_BEETLE },
		{ .pos = (Vec3){ 2, 1 }, .piece = HIVE_BLACK | HIVE_BEETLE },
		{ .pos = (Vec3){ 3, 1 }, .piece = HIVE_BLACK | HIVE_SPIDER },
		{ .pos = (Vec3){ 3, 1 }, .piece = HIVE_BLACK | HIVE_SPIDER },
		{ .pos = (Vec3){ 4, 1 }, .piece = HIVE_BLACK | HIVE_GRASSHOPPER },
		{ .pos = (Vec3){ 4, 1 }, .piece = HIVE_BLACK | HIVE_GRASSHOPPER },
		{ .pos = (Vec3){ 4, 1 }, .piece = HIVE_BLACK | HIVE_GRASSHOPPER },
		{ .pos = (Vec3){ 5, 1 }, .piece = HIVE_BLACK | HIVE_ANT },
		{ .pos = (Vec3){ 5, 1 }, .piece = HIVE_BLACK | HIVE_ANT },
		{ .pos = (Vec3){ 5, 1 }, .piece = HIVE_BLACK | HIVE_ANT },
		{ .pos = (Vec3){ 6, 1 }, .piece = HIVE_BLACK | HIVE_LADYBUG },
		{ .pos = (Vec3){ 7, 1 }, .piece = HIVE_BLACK | HIVE_MOSQUITO },
		{ .pos = (Vec3){ 8, 1 }, .piece = HIVE_BLACK | HIVE_PILLBUG },
	};

	memset(hive, 0, sizeof(*hive));
	memcpy(hive->pieces, defaultPieces, sizeof(defaultPieces));


	for (int index = 0; index < ARRLEN(hive->pieces); index++) {
		hive_putpiece(hive, index, &defaultPieces[index].pos);
	}

	if ((hive->win = newpad(HIVE_PAD_LINES, HIVE_PAD_COLUMNS)) == NULL)
		return 1;
	hive->pmincol = (HIVE_GRID_COLUMNS / 2) * HIVE_PIECE_COLUMNS;
	hive->pminrow = (HIVE_GRID_ROWS / 2) * HIVE_PIECE_LINES;

	hive->move.index = -1;
	hive->move.pos = (Vec3){ -1, -1, -1 };
}

void hive_playmove(Hive *hive, const HiveMove *move)
{
	HivePiece *piece = &hive->pieces[move->index];

	hive_delpiece(hive, move->index);
	hive_putpiece(hive, move->index, &move->pos);

	if ((piece->piece & HIVE_PLACED) == 0) {
		piece->piece |= HIVE_PLACED;
		hive->piecesPlayed++;
	}
	hive->movesPlayed++;
	hive_generatemoves(hive);
}

bool hive_canplace(Hive *hive, hive_side_t side, const Vec3 *pos)
{
	hive_piece_t piece = 
		hive->grid[pos->x + pos->y * HIVE_GRID_COLUMNS];
	if (piece)
		return false;
	int count = 0;
	for (hive_direction_t dir = 0; dir < HIVE_DIRECTION_COUNT; dir++) {
		const Vec3 adj = vec_move(pos, dir);
		const hive_piece_t
			piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
		if (piece) {
			if ((piece & side) == 0)
				return false;
			count++;
		}
	}
	return count != 0;
}

bool hive_validatemove(Hive *hive, const HiveMove *move)
{
	const hive_piece_index_t i = move->index;
	const hive_piece_t piece = hive->pieces[i].piece;
	const Vec3 pos = hive->pieces[i].pos;

	if (hive_whosturn(hive) != HIVE_GETSIDE(piece))
		return false;

	/* White should move first. */
	if(hive->movesPlayed == 0) 
		assert(hive_whosturn(hive) == HIVE_WHITE);

	switch(hive_whichturn(hive)) {
	case 1:
		/* blacks first turn */
		if (hive_whosturn(hive) == HIVE_BLACK) {
			if (hive_canplace(hive, HIVE_WHITE | HIVE_BLACK,
					&move->pos)) {
				return true;
			} else {
				return false;
			}
		/* white's first turn */
		} else {
			return true;
		}
		break;
	case 2:
	case 3:
		if (piece & HIVE_PLACED)
			if (hive_haspiece(hive, HIVE_QUEEN))
				return false;
		break;
	case 4:
		if (hive_haspiece(hive, HIVE_QUEEN)) {
			if (HIVE_GETTYPE(piece) != HIVE_QUEEN) {
				return false;
			}
		}
	}

	if ((piece & HIVE_PLACED) == 0) {
		if (hive_canplace(hive, HIVE_GETSIDE(piece), &move->pos)) {
			return true;
		} else {
			return false;
		}
	}

	for (int i = 0; i < hive->numMoves; i++) {
		const HiveMove *query = &hive->moves[i];
		if (memcmp(move, query, sizeof(*query)) == 0)
			return true;
	}
	return false;
}

int hive_getabovestack(Hive *hive, Vec3 *pos)
{
	for (int i = 0; i < hive->numStacks; i++) {
		const hive_piece_index_t p = hive->stacks[i];
		if ((hive->pieces[p].piece & HIVE_ABOVE) == 0)
			if (memcmp(&hive->pieces[p].pos, pos, offsetof(Vec3, z)) == 0) {
				*pos = hive->pieces[p].pos;
				return i;
			}
	}
	return -1;
}

void hive_delpiece(Hive *hive, hive_piece_index_t index)
{
	Vec3 top = hive->pieces[index].pos;
	hive_piece_t piece = hive->pieces[index].piece;

	int s;
	s = hive_getabovestack(hive, &top);

	if (s < 0)
		hive->grid[top.x + top.y * HIVE_GRID_COLUMNS] = HIVE_EMPTY;
	else
		memmove(&hive->stacks[s],
			&hive->stacks[--hive->numStacks], sizeof(*hive->stacks));

	if (s < 0)
		return;

	hive_piece_t *space = &hive->grid[top.x + top.y * HIVE_GRID_COLUMNS];

	for (int i = 0; i < hive->numStacks; i++) {
		const hive_piece_index_t p = hive->stacks[i];
		if (memcmp(&hive->pieces[p].pos, &top, offsetof(Vec3, z)) == 0)
			if (hive->pieces[p].pos.z == top.z - 1)
				space = &hive->pieces[p].piece;
	}
	*space &= ~HIVE_ABOVE;
}

void hive_putpiece(Hive *hive, hive_piece_index_t index, const Vec3 *pos)
{
	assert(hive->numStacks < HIVE_STACK_SIZE);
	assert(pos->z == 0);

	hive->pieces[index].pos = *pos;
	const Vec3 top = *pos;
	const int s = hive_getabovestack(hive, &top);
	hive_piece_t *space = &hive->grid[top.x + top.y * HIVE_GRID_COLUMNS];
	if (*space == HIVE_EMPTY) {
		*space = hive->pieces[index].piece;
	} else if (s < 0) {
		*space |= HIVE_ABOVE;
		hive->pieces[index].pos = (Vec3){top.x, top.y, top.z + 1};
		hive->stacks[hive->numStacks++] = index;
	} else {
		const hive_piece_index_t above = hive->stacks[s];
		hive->pieces[above].piece |= HIVE_ABOVE;
		hive->pieces[index].pos = (Vec3){top.x, top.y, top.z + 1};
		hive->stacks[hive->numStacks++] = index;
	}
}

bool hive_haspiece(Hive *hive, hive_type_t type)
{
	const hive_side_t side = hive_whosturn(hive);
	const hive_piece_t piece = side | type;
	for (int i = 0; i < ARRLEN(hive->pieces); i++) {
		if ((hive->pieces[i].piece & piece) == hive->pieces[i].piece) {
			return true;
		}
	}
	/* else */
	return false;
}

hive_piece_index_t hive_getpieceatpos(Hive *hive, const Vec3 *pos)
{
	for (int index = 0; index < ARRLEN(hive->pieces); index++)
		if (memcmp(&hive->pieces[index].pos, pos, sizeof(*pos)) == 0) {
			printf("piece index %d found at pos\n", index);
			return index;
		}
	/* else */
	return -1;
}

hive_side_t hive_whosturn(Hive *hive)
{
	return (hive->movesPlayed & 1) ? HIVE_BLACK : HIVE_WHITE;
}

size_t hive_whichturn(Hive *hive)
{
	return hive->movesPlayed / 2 + 1;
}

void hive_print(Hive *hive)
{
	werase(hive->win);

	for (int i = 0; i < ARRLEN(hive->pieces); i++) {
		Vec3 pos = hive->pieces[i].pos;
		hive_piece_t piece = hive->pieces[i].piece;
		if (piece & HIVE_ABOVE)
			continue;
		hive_printpiece(hive, &pos);
	}

	wattr_set(hive->win, 0, 0, NULL);
	
	prefresh(hive->win, hive->pminrow, hive->pmincol, 
		0, 0, LINES - 1, COLS / 2);
	prefresh(hive->win, 0, 0, 0, COLS / 2, LINES - 1, COLS - 1);	
	refresh();
}

void hive_printpiece(Hive *hive, Vec3 *pos)
{
	static const char pieceNames[] = {
		' ',
		'Q',
		'B',
		'G',
		'S',
		'A',
		'L',
		'M',
		'P',
	};
	static const char *triangles[] = {
		"\u25e2",
		"\u25e3",
		"\u25e4",
		"\u25e5"
	};
	static const int toDirection[] = {
		HIVE_NORTH_WEST,
		HIVE_NORTH_EAST,
		HIVE_SOUTH_EAST,
		HIVE_SOUTH_WEST
	};
	static const Vec3 cellOffsets[] = {
		{ 0, 0, 0 },
		{ 4, 0, 0 },
		{ 4, 1, 0 },
		{ 0, 1, 0 }
	};
	static const short pairs[4][4] = {
		/* -1 because these values are unused */
		{ -1, -1, -1, -1 },
		{ HIVE_PAIR_BLACK,
			HIVE_PAIR_BLACK_BLACK,
			HIVE_PAIR_BLACK_WHITE,
			HIVE_PAIR_BLACK_SPACE, },
		{ HIVE_PAIR_WHITE,
			HIVE_PAIR_WHITE_BLACK,
			HIVE_PAIR_WHITE_WHITE,
			HIVE_PAIR_WHITE_SPACE, },
		{ HIVE_PAIR_SPACE,
		  	HIVE_PAIR_SPACE_BLACK,
		  	HIVE_PAIR_SPACE_WHITE,
		  	HIVE_PAIR_SPACE_SPACE, },
	};
	hive_piece_t piece;
	hive_piece_t type;

	Vec3 world;

	world.x = pos->x * 4;
	world.y = pos->y * 2 + pos->x % 2;

	pos->z = 0;
	piece = hive_getexposedpiece(hive, pos);
	type = HIVE_GETTYPE(piece);
	if (!piece)
		return;

	const hive_piece_t side = HIVE_GETSIDE(piece);
	short pair;
	switch (side) {
	case HIVE_WHITE:
		pair = HIVE_PAIR_WHITE;
		break;
	case HIVE_BLACK:
		pair = HIVE_PAIR_BLACK;
		break;
	case HIVE_SPACE:
		pair = HIVE_PAIR_SPACE;
		break;
	}
	wattr_set(hive->win, A_REVERSE, pair, NULL);

	mvwprintw(hive->win, world.y, world.x + 1,
		"%c%c%c", ' ', pieceNames[(int) type], ' ');
	mvwprintw(hive->win, world.y + 1, world.x + 1,
		"%c%c%c", ' ', ' ', ' ');

	for (int n = 0; n < 4; n++) {
		Vec3 adj;

		adj = vec_move(pos, toDirection[n]);
		const hive_piece_t adjPiece =
			hive_getexposedpiece(hive, &adj);
		const short pair =
			pairs[HIVE_GETNSIDE(piece)]
				[HIVE_GETNSIDE(adjPiece)];
		wattr_set(hive->win, 0, pair, NULL);

		const Vec3 off = cellOffsets[n];
		mvwprintw(hive->win, world.y + off.y,
			world.x + off.x,
			"%s", triangles[n]);
	}
}

hive_piece_t hive_getabove(const Hive *hive, const Vec3 *pos)
{
	for (int i = 0; i < hive->numStacks; i++) {
		const hive_piece_index_t p = hive->stacks[i];
		if (memcmp(&hive->pieces[p].pos, pos, sizeof(*pos)) == 0)
			return hive->pieces[p].piece;
	}
	return 0;
}

hive_piece_t hive_getexposedpiece(const Hive *hive, Vec3 *pos)
{
	hive_piece_t piece;
	
	piece = hive->grid[pos->x + pos->y * HIVE_GRID_COLUMNS];

	while (piece & HIVE_ABOVE) {
		pos->z++;
		piece = hive_getabove(hive, pos);
	}
	return piece;
}

Vec3 hive_celltopos(const Vec3 *cell)
{
	Vec3 pos = *cell;
	pos.x = pos.x / HIVE_PIECE_COLUMNS;
	pos.y = (pos.y - (pos.x % 2)) / HIVE_PIECE_LINES;
	pos.z = 0;
	return pos;
}

hive_direction_t hive_getadjacent(Hive *hive, const Vec3 *pos, Vec3 *adj)
{
	hive_direction_t dir;
	for (dir = 0; dir < HIVE_DIRECTION_COUNT; dir++) {
		*adj = vec_move(pos, dir);
		const hive_piece_t
			piece = hive->grid[adj->x + adj->y * HIVE_GRID_COLUMNS];
		if (piece)
			goto next;
	}
	return -1;
next:
	return dir;
}

int hive_handle(Hive *hive, int c)
{
	switch (c) {
	case KEY_MOUSE:
		MEVENT event;
		Vec3 cell;

		getmouse(&event);
		
		cell.x = event.x;
		cell.y = event.y;
		cell.z = 0;

		if (event.bstate & BUTTON1_CLICKED
			|| event.bstate & BUTTON1_PRESSED) {

			/* Hive Window */
			if (cell.x > 0 && cell.x < COLS / 2
					&& cell.y > 0 && cell.y < LINES - 1) {
				cell.x = cell.x + hive->pmincol;
				cell.y = cell.y + hive->pminrow;
				Vec3 pos = hive_celltopos(&cell);
				Vec3 top = pos;
				(void) hive_getexposedpiece(hive, &top);
				const hive_piece_index_t
					index = hive_getpieceatpos(hive, &top);
				if (index < 0) {
					/* Space was clicked */
					if (hive->move.index >= 0)
						hive->move.pos = pos;
				} else {
					/* Piece was clicked */
					if (hive->move.index < 0)
						hive->move.index = index;
					else
						hive->move.pos = pos;
				}
				if (hive->move.index >= 0 && memcmp(
						&hive->move.pos, &(Vec3){ -1, -1, -1 }, sizeof(Vec3)) != 0) {
					if (hive_validatemove(hive, &hive->move)) {
						hive_playmove(hive, &hive->move);
					}
					hive->move.index = -1;
					hive->move.pos = (Vec3){ -1, -1, -1 };
				}
			}
			/* Inventory */
			if (cell.x > COLS / 2 && cell.x < COLS - 1
					&& cell.y > 0 && cell.y < LINES - 1) {
				cell.x = cell.x - COLS / 2;
				Vec3 pos = hive_celltopos(&cell);
				const hive_piece_index_t
					index = hive_getpieceatpos(hive, &pos);
				if (index < 0) {
					/* Space was clicked. */
				} else {
					hive->move.index = index;
				}
			}
		}
		goto success;
	case 'w':
		hive->pminrow += (hive->pminrow > HIVE_PAD_MINROW) ? HIVE_PIECE_LINES : 0;
		goto success;
	case 'a':
		hive->pmincol += (hive->pmincol > HIVE_PAD_MINCOL) ? HIVE_PIECE_COLUMNS : 0;
		goto success;
	case 's':
		hive->pminrow -= HIVE_PIECE_LINES;
		goto success;
	case 'd':
		hive->pmincol -= HIVE_PIECE_COLUMNS;
		goto success;
	case 'r':
		hive_init(hive);
		goto success;
	case ERR:
		goto failure;
	default:
		goto success;
	}
failure:
	return 1;
success:
	hive_print(hive);
	return 0;
}
