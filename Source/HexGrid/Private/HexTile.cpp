#include "HexTile.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"
#include <GameFramework/Character.h>
#include <NavigationTestingActor.h>
#include <Engine/NavigationObjectBase.h>
#include "Components/TextRenderComponent.h"

AHexTile::AHexTile()
{
    PrimaryActorTick.bCanEverTick = false;

    HexMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("HexMesh"));
    RootComponent = HexMesh;

    HexMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    HexMesh->SetCollisionObjectType(ECC_WorldStatic);

    HexMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
    HexMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    HexMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

    // IMPORTANT
    HexMesh->bUseComplexAsSimpleCollision = true;

    DebugText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DebugText"));
    DebugText->SetupAttachment(RootComponent);
    DebugText->SetRelativeLocation(FVector(0.0f, 0.0f, DebugTextZOffset));
    DebugText->SetRelativeRotation(FRotator(90.f, 90.f, 0.0f));
    DebugText->SetWorldSize(DebugTextWorldSize);
    DebugText->SetTextRenderColor(DebugTextColor);
    DebugText->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    DebugText->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
    //DebugText->bAlwaysIncludeOnDrawInGame = true;  // Visible in PIE

}

void AHexTile::BeginPlay()
{
    Super::BeginPlay();
    GenerateHexMesh();

    if (BaseMaterial)
    {
        HighlightMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        HexMesh->SetMaterial(0, HighlightMaterial);
        HighlightMaterial->SetVectorParameterValue(TEXT("BaseColor"), NormalColor);
    }
}

void AHexTile::SetTileHighlight(FLinearColor NewColor)
{
    if (HighlightMaterial)
    {
        HighlightMaterial->SetVectorParameterValue(TEXT("BaseColor"), NewColor);
    }
}

void AHexTile::ResetHighlight()
{
    if (HighlightMaterial)
    {
        HighlightMaterial->SetVectorParameterValue(TEXT("BaseColor"), NormalColor);
    }
}

void AHexTile::GenerateHexMesh()
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FProcMeshTangent> Tangents;
    TArray<FLinearColor> VertexColors;

    const float outerRadius = HexFlatToFlat * (2.0f / FMath::Sqrt(3.0f));  // = x 1.1547

    UE_LOG(LogTemp, Log, TEXT("HexTile %s - FlatToFlat: %.1f   OuterRadius: %.3f"),
        *GridCoord.ToString(), HexFlatToFlat, outerRadius);

    const float shrink = 0.9f;
    const float AngleStep = 60.0f;
    for (int i = 0; i < 6; ++i)
    {
        float AngleDeg = 30.0f + (float)i * AngleStep;
        float AngleRad = FMath::DegreesToRadians(AngleDeg);

        float X = (HexFlatToFlat * FMath::Cos(AngleRad)) * shrink; 
        float Y = (HexFlatToFlat * FMath::Sin(AngleRad)) * shrink;

        Vertices.Add(FVector(X, Y, 0.0f));
    }

    // Center vertex (for fan triangulation)
    Vertices.Add(FVector::ZeroVector);  // index 6 = center

    // Triangulate (6 triangles from center)
    for (int i = 0; i < 6; ++i)
    {
        Triangles.Add(6);               // center
        Triangles.Add((i + 1) % 6);     // next (swapped)
        Triangles.Add(i);               // current
    }

    // Normals (all up)
    for (int i = 0; i < Vertices.Num(); ++i)
        Normals.Add(FVector(0, 0, 1));

    // Simple UV (optional)
    for (int i = 0; i < Vertices.Num(); ++i)
        UV0.Add(FVector2D(0.5f, 0.5f));

    HexMesh->CreateMeshSection_LinearColor(
        0,
        Vertices,
        Triangles,
        Normals,
        UV0,
        VertexColors,
        Tangents,
        true // <-- enables collision for this section
    );

    HexMesh->ContainsPhysicsTriMeshData(true);
}

void AHexTile::UpdateDebugTextFromCoord()
{
    if (DebugText)
    {
        int32 S = GridCoord.S();
        FString CoordStr = FString::Printf(TEXT("Q:%d\nR:%d\nS:%d"), GridCoord.Q, GridCoord.R, S);
        DebugText->SetText(FText::FromString(CoordStr));
    }
}

void AHexTile::OnClicked(AActor* TouchedActor, FKey ButtonPressed)
{
    bIsSelected = !bIsSelected;
    // Update material color here (green/red/etc)
    UE_LOG(LogTemp, Warning, TEXT("Tile clicked at Q=%d R=%d"), GridCoord.Q, GridCoord.R);
}

void AHexTile::NotifyActorOnClicked(FKey ButtonPressed)
{
    Super::NotifyActorOnClicked(ButtonPressed);
    GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString(GetName()));
}

void AHexTile::OnBeginMouseOver(UPrimitiveComponent* TouchedComp)
{
    bIsHovered = true;
    // Change material to yellow highlight, etc.
}

void AHexTile::OnEndMouseOver(UPrimitiveComponent* TouchedComp)
{
    bIsHovered = false;
    // Reset material
}