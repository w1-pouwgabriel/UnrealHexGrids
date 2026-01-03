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

    int32 SpawnedCount = 0;

    for (int32 q = 0; q < GridWidth; ++q)
    {
        for (int32 r = 0; r < GridHeight; ++r)
        {
            FHexCoord Coord(q, r);

            // Calculate LOCAL position relative to the Grid actor
            FVector LocalLocation = HexToWorldLocation(Coord);

            // Spawn as child attached to THIS Grid actor
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            SpawnParams.Owner = this;                     // Important: Set owner
            SpawnParams.Instigator = GetInstigator();

            AHexTile* NewTile = GetWorld()->SpawnActor<AHexTile>(
                HexTileClass,
                GetActorLocation() + LocalLocation,       // World = Grid Location + Local Offset
                GetActorRotation(),
                SpawnParams
            );

            if (NewTile)
            {
                NewTile->GridCoord = Coord;
                NewTile->HexFlatToFlat = HexFlatToFlat;

                // Attach to the Grid actor (makes it move with the grid)
                NewTile->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

                // Optional: set relative location explicitly (usually not needed after Attach)
                // NewTile->SetActorRelativeLocation(LocalLocation);

                Tiles.Add(Coord, NewTile);
                ++SpawnedCount;

                UE_LOG(LogTemp, Log, TEXT("Spawned child tile %s at local %s"),
                    *Coord.ToString(), *LocalLocation.ToCompactString());
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Grid spawning complete: %d child tiles attached."), SpawnedCount);
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
    float x = HexFlatToFlat * (FMath::Sqrt(3.0f) * Coord.Q + FMath::Sqrt(3.0f) / 2.0f * Coord.R);
    float y = HexFlatToFlat * (3.0f / 2.0f * Coord.R);
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