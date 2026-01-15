// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexCoord.h"
#include "HexTile.h"
#include "Grid.generated.h"

UCLASS()
class HEXGRID_API AGrid : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AGrid();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    TMap<FHexCoord, AHexTile*> Tiles;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    bool bCenterGrid = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridWidth = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridHeight = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    TSubclassOf<AHexTile> HexTileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float HexFlatToFlat = 100.0f;

    UPROPERTY()
    AHexTile* SelectedTileA;

    UPROPERTY()
    AHexTile* SelectedTileB;

    // Core functions (for later: neighbors, pathfinding)
    UFUNCTION(BlueprintCallable, Category = "Hex Grid")
    AHexTile* GetTileAtCoord(const FHexCoord& Coord) const;

    UFUNCTION(BlueprintCallable, Category = "Hex Grid")
    FVector HexToWorldLocation(const FHexCoord& Coord) const;

    UFUNCTION(BlueprintCallable, Category = "Hex Grid")
    TArray<FHexCoord> GetNeighbors(const FHexCoord& Coord) const;

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void SelectTile(AHexTile* Tile, bool IsLeftClick);

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void ClearSelection();

    UFUNCTION(BlueprintCallable, Category = "Selection")
    void ClearPathHighlight();

    void ShowPathBetween(const FHexCoord& Start, const FHexCoord& End);

    UFUNCTION(BlueprintCallable, Category = "Hex Grid")
    void MakeCubicMap(int32 Width, int32 Height);

    UFUNCTION(BlueprintCallable, Category = "Hex Grid")
    TArray<FHexCoord> GetSpiralCoords(int32 Radius) const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 HexRadius = 3;

    UFUNCTION(BlueprintCallable, Category = "Hex Grid")
    TArray<FHexCoord> GetHexRange(int32 N) const;
};