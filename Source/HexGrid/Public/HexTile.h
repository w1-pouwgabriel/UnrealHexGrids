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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug Text")
    class UTextRenderComponent* DebugText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Text")
    FString DebugTextString = TEXT("Q:0 R:0");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Text")
    FColor DebugTextColor = FColor::Yellow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Text", meta = (ClampMin = "0.01", ClampMax = "5.0"))
    float DebugTextWorldSize = 40.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug Text")
    float DebugTextZOffset = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
    UMaterialInstanceDynamic* HighlightMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
    FLinearColor NormalColor = FLinearColor(1, 0, 0);   // default red

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
    FLinearColor SelectedAColor = FLinearColor(0, 0.5f, 1);   // blue-ish

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
    FLinearColor SelectedBColor = FLinearColor(0, 1, 0.3f);   // green-ish

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Highlight")
    FLinearColor PathColor = FLinearColor(1, 1, 0);           // yellow for path

    UFUNCTION(BlueprintCallable, Category = "Highlight")
    void SetTileHighlight(FLinearColor NewColor);

    UFUNCTION(BlueprintCallable, Category = "Highlight")
    void ResetHighlight();
    
    UFUNCTION(BlueprintCallable, Category = "Debug Text")
    void UpdateDebugTextFromCoord();

    UFUNCTION()
    void OnClicked(AActor* TouchedActor, FKey ButtonPressed);

    UFUNCTION()
    void OnBeginMouseOver(UPrimitiveComponent* TouchedComp);

    UFUNCTION()
    void OnEndMouseOver(UPrimitiveComponent* TouchedComp);

    void NotifyActorOnClicked(FKey ButtonPressed) override;

protected:
    virtual void BeginPlay() override;

    void GenerateHexMesh();
};