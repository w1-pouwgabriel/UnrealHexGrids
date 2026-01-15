// Fill out your copyright notice in the Description page of Project Settings.
#include "Grid.h"
#include "HexTile.h"

// Sets default values
AGrid::AGrid()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AGrid::BeginPlay()
{
    Super::BeginPlay();

    if (!HexTileClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ERROR: HexTileClass not set!"));
        return;
    }

    Tiles.Empty();

    // Generate symmetric hex shape centered at (0,0)
    TArray<FHexCoord> AllCoords = GetHexRange(HexRadius);

    int32 SpawnedCount = 0;
    for (const FHexCoord& Coord : AllCoords)
    {
        FVector LocalLocation = HexToWorldLocation(Coord);
        FVector SpawnLoc = GetActorLocation() + LocalLocation;  // (0,0) at actor center!

        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        AHexTile* NewTile = GetWorld()->SpawnActor<AHexTile>(
            HexTileClass, SpawnLoc, GetActorRotation(), SpawnParams);

        if (NewTile)
        {
            NewTile->GridCoord = Coord;
            NewTile->HexFlatToFlat = HexFlatToFlat;
            NewTile->UpdateDebugTextFromCoord();
            NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            Tiles.Add(Coord, NewTile);
            ++SpawnedCount;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Hex grid spawning complete: %d tiles (radius %d)"), SpawnedCount, HexRadius);
}

void AGrid::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

AHexTile* AGrid::GetTileAtCoord(const FHexCoord& Coord) const
{
    AHexTile* const* FoundTile = Tiles.Find(Coord);
    return FoundTile ? *FoundTile : nullptr;
}

FVector AGrid::HexToWorldLocation(const FHexCoord& Coord) const
{
    const float size = HexFlatToFlat;

    float x = size * (FMath::Sqrt(3.0f) * Coord.Q + FMath::Sqrt(3.0f) / 2.0f * Coord.R);
    float y = size * (3.0f / 2.0f * Coord.R);

    return FVector(x, y, 0.0f);
}

TArray<FHexCoord> AGrid::GetNeighbors(const FHexCoord& Coord) const
{
    TArray<FHexCoord> Neighbors;
    for (const FHexCoord& Dir : FHexCoord::Directions())
    {
        FHexCoord Neighbor = Coord + Dir;
        if (Tiles.Contains(Neighbor))  // Only valid neighbors in grid
            Neighbors.Add(Neighbor);
    }
    return Neighbors;
}

void AGrid::SelectTile(AHexTile* Tile, bool IsLeftClick)
{
    if (!Tile) return;

    // Clear previous highlights on path
    ClearPathHighlight();

    if (IsLeftClick)
    {
        if (SelectedTileA && SelectedTileA != Tile)
        {
            SelectedTileA->ResetHighlight();
        }
        SelectedTileA = Tile;
        Tile->SetTileHighlight(Tile->SelectedAColor);
    }
    else
    {
        if (SelectedTileB && SelectedTileB != Tile)
        {
            SelectedTileB->ResetHighlight();
        }
        SelectedTileB = Tile;
        Tile->SetTileHighlight(Tile->SelectedBColor);
    }

    if (SelectedTileA && SelectedTileB && SelectedTileA != SelectedTileB)
    {
        int32 Distance = AxialDistance(SelectedTileA->GridCoord, SelectedTileB->GridCoord);
        UE_LOG(LogTemp, Warning, TEXT("Distance between %s and %s = %d steps"),
            *SelectedTileA->GridCoord.ToString(), *SelectedTileB->GridCoord.ToString(), Distance);

        // Show path (simple A* or line for now)
        ShowPathBetween(SelectedTileA->GridCoord, SelectedTileB->GridCoord);
    }
}

void AGrid::ClearSelection()
{
    if (SelectedTileA) SelectedTileA->ResetHighlight();
    if (SelectedTileB) SelectedTileB->ResetHighlight();
    SelectedTileA = nullptr;
    SelectedTileB = nullptr;
    ClearPathHighlight();
}

void AGrid::ClearPathHighlight()
{
    for (const auto& Pair : Tiles)
    {
        if (AHexTile* Tile = Pair.Value)
        {
            Tile->ResetHighlight();
        }
    }
}

void AGrid::ShowPathBetween(const FHexCoord& Start, const FHexCoord& End)
{
    TArray<FHexCoord> Line = FHexCoord::GetHexLine(Start, End);

    for (const FHexCoord& Coord : Line)
    {
        if (AHexTile* Tile = GetTileAtCoord(Coord))
        {
            Tile->SetTileHighlight(Tile->PathColor);
        }
    }
}

void AGrid::MakeCubicMap(int32 Width, int32 Height)
{
    Tiles.Empty();

    FVector CenterOffset = FVector::ZeroVector;
    if (bCenterGrid) {
        float midQ = (Width - 1) * 0.5f;
        float midR = (Height - 1) * 0.5f;
        FHexCoord midCoord(FMath::RoundToInt(midQ), FMath::RoundToInt(midR));
        CenterOffset = -HexToWorldLocation(midCoord);
    }

    int32 SpawnedCount = 0;

    for (int32 col = 0; col < Width; col++)
    {
        for (int32 row = 0; row < Height; row++)
        {
            int32 q = col;
            int32 r = row;

            FHexCoord Coord(q, r);

            FVector LocalLocation = HexToWorldLocation(Coord);
            FVector SpawnLoc = GetActorLocation() + LocalLocation + CenterOffset;

            // Spawn as child attached to THIS Grid actor
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            AHexTile* NewTile = GetWorld()->SpawnActor<AHexTile>(
                HexTileClass,
                SpawnLoc,
                GetActorRotation(),
                SpawnParams
            );

            if (NewTile)
            {
                NewTile->GridCoord = Coord;
                NewTile->HexFlatToFlat = HexFlatToFlat;

                // Set coords as text on the tile
                NewTile->UpdateDebugTextFromCoord();
                NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

                Tiles.Add(Coord, NewTile);
                ++SpawnedCount;

                UE_LOG(LogTemp, Log, TEXT("Spawned child tile %s at local %s"),
                    *Coord.ToString(), *LocalLocation.ToCompactString());
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Grid spawning complete: %d child tiles attached."), SpawnedCount);
}

TArray<FHexCoord> AGrid::GetHexRange(int32 N) const
{
    TArray<FHexCoord> Results;
    for (int32 q = -N; q <= N; ++q)
    {
        int32 rStart = FMath::Max(-N, -q - N);
        int32 rEnd = FMath::Min(N, -q + N);
        for (int32 r = rStart; r <= rEnd; ++r)
        {
            Results.Add(FHexCoord(q, r));
        }
    }
    return Results;
}

TArray<FHexCoord> AGrid::GetSpiralCoords(int32 Radius) const
{
    TArray<FHexCoord> Coords;

    // Center first
    Coords.Add(FHexCoord(0, 0));

    // Then each ring/layer outward
    for (int32 layer = 1; layer <= Radius; ++layer)
    {
        // Start at the hex to the top-right of the previous ring
        FHexCoord hex = FHexCoord(layer, -layer);  // one of the standard ring starts

        // Walk around the 6 sides of the ring
        for (int32 side = 0; side < 6; ++side)
        {
            for (int32 step = 0; step < layer; ++step)
            {
                Coords.Add(hex);

                // Move one step in current direction
                switch (side)
                {
                case 0: hex = hex + FHexCoord(1, 0); break;  // east
                case 1: hex = hex + FHexCoord(1, -1); break;  // northeast
                case 2: hex = hex + FHexCoord(0, -1); break;  // northwest
                case 3: hex = hex + FHexCoord(-1, 0); break;  // west
                case 4: hex = hex + FHexCoord(-1, 1); break;  // southwest
                case 5: hex = hex + FHexCoord(0, 1); break;  // southeast
                }
            }
        }
    }

    return Coords;
}
