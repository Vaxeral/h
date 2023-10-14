#ifndef INCLUDED_HIVE_H
#define INCLUDED_HIVE_H

#include <assert.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <stddef.h>

#define ARRLEN(a) (sizeof(a)/sizeof(*(a)))

#define HIVE_STACK_SIZE 32
#define HIVE_PIECE_COUNT 28

#define HIVE_TYPE_MASK 0x0f
#define HIVE_SIDE_MASK 0x30

#define HIVE_GETSIDE(p) ((p) & HIVE_SIDE_MASK)
#define HIVE_GETTYPE(p) ((p) & HIVE_TYPE_MASK)

#define HIVE_GETNSIDE(p) (((p) & HIVE_SIDE_MASK) >> 4)
#define HIVE_GETNTYPE(p) ((p) & HIVE_TYPE_MASK)

#define HIVE_WHITE 0x20
#define HIVE_BLACK 0x10
#define HIVE_SPACE 0x30
#define HIVE_ABOVE 0x40
#define HIVE_PLACED 0x80

#define HIVE_EMPTY 0x0
#define HIVE_QUEEN 0x1
#define HIVE_BEETLE 0x2
#define HIVE_GRASSHOPPER 0x3
#define HIVE_SPIDER 0x4
#define HIVE_ANT 0x5
#define HIVE_LADYBUG 0x6
#define HIVE_MOSQUITO 0x7
#define HIVE_PILLBUG 0x8
#define HIVE_TYPE_COUNT 0x9

typedef unsigned char hive_side_t;
typedef unsigned char hive_type_t;

typedef enum hive_color_pair {
	HIVE_PAIR_BLACK = 1,
	HIVE_PAIR_WHITE = 2,
	HIVE_PAIR_SPACE = 3,
	HIVE_PAIR_BLACK_WHITE = 4,
	HIVE_PAIR_WHITE_BLACK = 5,
	HIVE_PAIR_BLACK_BLACK = 6,
	HIVE_PAIR_BLACK_SPACE = 7,
	HIVE_PAIR_WHITE_WHITE = 8,
	HIVE_PAIR_WHITE_SPACE = 9,
	HIVE_PAIR_SPACE_SPACE = 10,
	HIVE_PAIR_SPACE_WHITE = 11,
	HIVE_PAIR_SPACE_BLACK = 12
} hive_color_pair_t;

typedef enum hive_direction {
	HIVE_SOUTH_EAST,
	HIVE_NORTH_EAST,
	HIVE_NORTH,
	HIVE_NORTH_WEST,
	HIVE_SOUTH_WEST,
	HIVE_SOUTH,
	HIVE_DIRECTION_COUNT
} hive_direction_t;

typedef unsigned char hive_piece_t;
typedef char hive_piece_index_t;

typedef struct vec3 {
	int x, y, z;
} Vec3;

typedef struct hive_move {
	Vec3 pos;
	hive_piece_index_t index;
} HiveMove;

typedef struct hive_piece {
	hive_piece_t piece;
	Vec3 pos;
} HivePiece;

#define HIVE_GRID_COLUMNS 127
#define HIVE_GRID_ROWS 127
#define HIVE_GRID_SPACES (HIVE_GRID_COLUMNS * HIVE_GRID_ROWS)

#define HIVE_PIECE_LINES 2
#define HIVE_PIECE_COLUMNS 4
#define HIVE_PAD_LINES (HIVE_GRID_ROWS * HIVE_PIECE_LINES)
#define HIVE_PAD_COLUMNS (HIVE_GRID_COLUMNS * HIVE_PIECE_COLUMNS)
#define HIVE_PAD_MINROW (32 * HIVE_PIECE_LINES)
#define HIVE_PAD_MINCOL (32 * HIVE_PIECE_COLUMNS)

typedef struct hive {
	WINDOW *win;
	int pminrow;
	int pmincol;
	hive_piece_t grid[HIVE_GRID_SPACES];
	hive_piece_index_t stacks[HIVE_STACK_SIZE];
	size_t numStacks;
	size_t movesPlayed;
	int piecesPlayed;
	HivePiece pieces[HIVE_PIECE_COUNT];
	HiveMove *moves;
	size_t numMoves;
	Vec3 startPos;
	hive_piece_index_t startPiece;
	HiveMove move;
} Hive;

Vec3 vec_move(const Vec3 *vec, int dir);

int hive_init(Hive *hive);
bool hive_haspiece(Hive *hive, hive_type_t type);
hive_piece_index_t hive_getpieceatpos(Hive *hive, const Vec3 *pos);
hive_piece_t hive_getabove(const Hive *hive, const Vec3 *pos);
hive_piece_t hive_getexposedpiece(const Hive *hive, Vec3 *pos);
void hive_initgrid(Hive *hive, int rows, int cols);
int hive_handle(Hive *hive, int c);
void hive_print(Hive *hive);
void hive_printpiece(Hive *hive, Vec3 *pos);
int hive_getabovestack(Hive *hive, Vec3 *pos);
void hive_putpiece(Hive *hive, hive_piece_index_t index, const Vec3 *pos);
void hive_delpiece(Hive *hive, hive_piece_index_t index);
hive_side_t hive_whosturn(Hive *hive);
size_t hive_whichturn(Hive *hive);
bool hive_canplace(Hive *hive, hive_side_t side, const Vec3 *pos);
void hive_generatemoves(Hive *hive);

#endif
