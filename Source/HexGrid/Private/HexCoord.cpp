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

TArray<FHexCoord> FHexCoord::GetHexLine(const FHexCoord& Start, const FHexCoord& End)
{
    TArray<FHexCoord> Results;

    int32 N = AxialDistance(Start, End);
    if (N == 0)
    {
        Results.Add(Start);
        return Results;
    }

    FCubeCoord aCube(Start);
    FCubeCoord bCube(End);

    for (int i = 0; i <= N; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(N);
        FCubeCoord lerped = CubeLerp(aCube, bCube, t);
        FCubeCoord rounded = CubeRound(
            static_cast<float>(lerped.X),
            static_cast<float>(lerped.Y),
            static_cast<float>(lerped.Z)
        );
        Results.Add(rounded.ToAxial());
    }

    return Results;
}
