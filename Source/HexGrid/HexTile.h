#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexCoord.h"	
#include "ProceduralMeshComponent.h"
#include "HexTile.generated.h"

UCLASS()
class HEXGRID_API AHexTile : public AActor
{
    GENERATED_BODY()

public:
    AHexTile();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProceduralMeshComponent* HexMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    FHexCoord GridCoord;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    float HexFlatToFlat = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    bool bIsSelected = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hex")
    bool bIsHovered = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    UMaterialInterface* BaseMaterial;

    UFUNCTION()
    void OnClicked(AActor* TouchedActor, FKey ButtonPressed);

    UFUNCTION()
    void OnBeginMouseOver(UPrimitiveComponent* TouchedComp);

    UFUNCTION()
    void OnEndMouseOver(UPrimitiveComponent* TouchedComp);

protected:
    virtual void BeginPlay() override;

    void GenerateHexMesh();
};