#include "hive.h"

Vec3 dir_offsets[2][6] = {
	{
		{  1,  0, 0 }, {  1, -1, 0 }, { 0, -1, 0 },
		{ -1, -1, 0 }, { -1,  0, 0 }, { 0,  1, 0 }
	},
	{
		{  1,  1, 0 }, {  1,  0, 0 }, { 0, -1, 0 },
		{ -1,  0, 0 }, { -1,  1, 0 }, { 0,  1, 0 }
	}
};

Vec3 vec_move(const Vec3 *vec, int dir)
{
	Vec3 offset = dir_offsets[vec->x % 2][dir];
	return (Vec3) {
		vec->x + offset.x,
		vec->y + offset.y,
		0
	};
}
