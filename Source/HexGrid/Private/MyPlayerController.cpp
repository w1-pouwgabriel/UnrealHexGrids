// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include <Kismet/GameplayStaticsTypes.h>

void AMyPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    check(InputComponent);

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

}