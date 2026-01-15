// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include <Kismet/GameplayStaticsTypes.h>
#include "Grid.h"
#include "HexTile.h"

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    check(InputComponent);

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    InputComponent->BindAction("SelectTileLeft", IE_Pressed, this, &AMyPlayerController::OnLeftClick);
    InputComponent->BindAction("SelectTileRight", IE_Pressed, this, &AMyPlayerController::OnRightClick);
}

void AMyPlayerController::OnLeftClick()
{
    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        if (AHexTile* Tile = Cast<AHexTile>(Hit.GetActor()))
        {
            if (AGrid* Grid = Tile->GetOwner<AGrid>())
            {
                Grid->SelectTile(Tile, true);  // left click
            }
        }
    }
}

void AMyPlayerController::OnRightClick()
{
    FHitResult Hit;
    if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
    {
        if (AHexTile* Tile = Cast<AHexTile>(Hit.GetActor()))
        {
            if (AGrid* Grid = Tile->GetOwner<AGrid>())
            {
                Grid->SelectTile(Tile, false);  // right click
            }
        }
    }
}