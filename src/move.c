#include "hive.h"

bool hive_isonehive(Hive *hive, hive_piece_index_t pieceIndex)
{
	HivePiece *piece = &hive->pieces[pieceIndex];
	Vec3 top = piece->pos;

	(void) hive_getexposedpiece(hive, &top);

	if (top.z > 0)
		return true;

	Vec3 queue[HIVE_PIECE_COUNT];
	size_t queueLen = 0;
	Vec3 positionsVisited[HIVE_PIECE_COUNT];
	size_t numPositionsVisited = 0;

	int piecesVisited = 0;

	positionsVisited[numPositionsVisited++] = piece->pos;
	piecesVisited++;

	for (hive_direction_t dir = 0; dir < HIVE_DIRECTION_COUNT; dir++) {
		const Vec3 adj = vec_move(&piece->pos, dir);
		const hive_piece_t 
			piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
		if (piece) {
			queue[queueLen++] = adj;
			break;
		}
	}

	while (queueLen > 0) {
		Vec3 current = queue[--queueLen];
		for (int i = 0; i < numPositionsVisited; i++)
			if (memcmp(&positionsVisited[i], &current, sizeof(current)) == 0)
				goto skip;
		positionsVisited[numPositionsVisited++] = current;
		(void) hive_getexposedpiece(hive, &current);
		piecesVisited += current.z + 1;
		for (hive_direction_t dir = 0; dir < HIVE_DIRECTION_COUNT; dir++) {
			const Vec3 adj = vec_move(&current, dir);
			const hive_piece_t
				piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
			if (piece)
				queue[queueLen++] = adj;
		}
	skip:
	}

	return piecesVisited == hive->piecesPlayed;
}

bool hive_haspiecerightof(Hive *hive, const Vec3 *pos,
	hive_direction_t dir)
{
	const hive_direction_t right = (dir + 5) % HIVE_DIRECTION_COUNT;
	const Vec3 adj = vec_move(pos, right);
	const hive_piece_t 
		piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
	return piece != 0;
}

bool hive_haspieceleftof(Hive *hive, const Vec3 *pos,
	hive_direction_t dir)
{
	const hive_direction_t left = (dir + 1) % HIVE_DIRECTION_COUNT;
	const Vec3 adj = vec_move(pos, left);
	const hive_piece_t 
		piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
	return piece != 0;
}

void hive_addmove(Hive *hive, hive_piece_index_t index, const Vec3 *pos)
{
	hive->moves = realloc(hive->moves,
		(hive->numMoves + 1) * sizeof(*hive->moves));

	hive->moves[hive->numMoves++] = (HiveMove){
		.pos = *pos,
		.index = index
	};
}

static void hive_getslidesunlimited(Hive *hive, const Vec3 *pos, const Vec3 *lastPos)
{
	for (hive_direction_t dir = 0; dir < HIVE_DIRECTION_COUNT; dir++) {
		const Vec3 adj = vec_move(pos, dir);
		const hive_piece_t
			piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
		if (piece == 0 && memcmp(&adj, lastPos, sizeof(adj)) != 0
				&& memcmp(&adj, &hive->startPos, sizeof(adj)) != 0) {
			if (hive_haspiecerightof(hive, pos, dir) != 
					hive_haspieceleftof(hive, pos, dir)) {
				hive_addmove(hive, hive->startPiece, &adj);
				hive_getslidesunlimited(hive, &adj, pos);
			}
		}
	}
}

static void hive_getslideslimited(Hive *hive, const Vec3 *pos, const Vec3 *lastPos,
	int depth)
{
	if (depth == 0) {
		hive_addmove(hive, hive->startPiece, pos);
	} else {
		for (hive_direction_t dir = 0; dir < HIVE_DIRECTION_COUNT; dir++) {
			const Vec3 adj = vec_move(pos, dir);
			const hive_piece_t
				piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
			if (piece == 0 && memcmp(&adj, lastPos, sizeof(adj)) != 0
					&& memcmp(&adj, &hive->startPos, sizeof(adj)) != 0) {
				if (hive_haspiecerightof(hive, pos, dir) != 
						hive_haspieceleftof(hive, pos, dir)) {
					hive_getslideslimited(hive, &adj, pos, depth - 1);
				}
			}
		}
	}
}

void hive_getslides(Hive *hive, hive_piece_index_t index, int depth)
{
	const Vec3 pos = hive->pieces[index].pos;
	const hive_piece_t piece = hive->pieces[index].piece;
	hive->startPos = pos;
	hive->startPiece = index;
	hive_piece_t *space = &hive->grid[pos.x + pos.y * HIVE_GRID_COLUMNS];
	*space = 0;
	if (depth == 0) {
		hive_getslidesunlimited(hive, &hive->startPos, &hive->startPos);
	} else {
		hive_getslideslimited(hive, &hive->startPos, &hive->startPos, depth);
	}
	*space = piece;
}

void hive_movesforant(Hive *hive, hive_piece_index_t index)
{
	hive_getslides(hive, index, 0);
}

void hive_movesforspider(Hive *hive, hive_piece_index_t index)
{
	hive_getslides(hive, index, 3);
}

void hive_movesforqueen(Hive *hive, hive_piece_index_t index)
{
	hive_getslides(hive, index, 1);
}

void hive_movesforbeetle(Hive *hive, hive_piece_index_t index)
{

	const Vec3 pos = hive->pieces[index].pos;
	const hive_piece_t piece = hive->pieces[index].piece;
	
	for (hive_direction_t dir = 0; dir < HIVE_DIRECTION_COUNT; dir++) {
		const Vec3 adj = vec_move(&pos, dir);
		const hive_piece_t
			piece = hive->grid[adj.x + adj.y * HIVE_GRID_COLUMNS];
		if (piece) {
			hive_addmove(hive, index, &adj);
		} else {
			if (hive_haspiecerightof(hive, &pos, dir) != 
					hive_haspieceleftof(hive, &pos, dir)) {
				hive_addmove(hive, index, &adj);
			}
		}
	}
}

void hive_movesfor(Hive *hive, hive_piece_index_t index)
{
	const hive_type_t 
		type = HIVE_GETTYPE(hive->pieces[index].piece);
	switch (type) {
		case HIVE_QUEEN:
			hive_movesforqueen(hive, index);
			break;
		case HIVE_BEETLE:
			hive_movesforbeetle(hive, index);
			break;
		case HIVE_SPIDER:
			hive_movesforspider(hive, index);
			break;
		case HIVE_GRASSHOPPER:
			// hive_movesforgrasshopper(hive, index);
			break;
		case HIVE_ANT:
			hive_movesforant(hive, index);
			break;
		case HIVE_LADYBUG:
			// hive_movesforladybug(hive, index);
			break;
		case HIVE_MOSQUITO:
			// hive_movesformosquito(hive, index);
			break;
		case HIVE_PILLBUG:
			// hive_movesforpillbug(hive, index);
			break;
	}
}

void hive_generatemoves(Hive *hive)
{
	memset(hive->moves, 0, hive->numMoves * sizeof(*hive->moves));
	hive->numMoves = 0;
	for (int i = 0; i < ARRLEN(hive->pieces); i++)
		if (hive->pieces[i].piece & HIVE_PLACED) {
			hive_movesfor(hive, i);
		}
}
