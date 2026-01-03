#include "HexCoord.h"

const TArray<FHexCoord>& FHexCoord::Directions()
{
	static const TArray<FHexCoord> Dirs =
	{
		{ 1,  0 }, { 1, -1 }, { 0, -1 },
		{ -1, 0 }, { -1, 1 }, { 0,  1 }
	};

	return Dirs;
}
