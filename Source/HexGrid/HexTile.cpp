#include "HexTile.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/MaterialInstanceDynamic.h"

AHexTile::AHexTile()
{
    PrimaryActorTick.bCanEverTick = false;

    HexMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("HexMesh"));
    RootComponent = HexMesh;

    HexMesh->SetCollisionProfileName(TEXT("BlockAll"));
    HexMesh->SetGenerateOverlapEvents(true);

    // Enable mouse interaction
    HexMesh->SetNotifyRigidBodyCollision(true);
    HexMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AHexTile::BeginPlay()
{
    Super::BeginPlay();

    GenerateHexMesh();

    if (BaseMaterial)
    {
        UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        HexMesh->SetMaterial(0, DynMat);
        // Example: DynMat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor::Red);
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

    const float outerRadius = HexFlatToFlat * (2.0f / FMath::Sqrt(3.0f));  // = × 1.1547

    UE_LOG(LogTemp, Log, TEXT("HexTile %s - FlatToFlat: %.1f   OuterRadius: %.3f"),
        *GridCoord.ToString(), HexFlatToFlat, outerRadius);

    const float shrink = 0.9f;
    const float AngleStep = 60.0f;
    for (int i = 0; i < 6; ++i)
    {
        float AngleDeg = 0.0f + (float)i * AngleStep;
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
        Triangles.Add(6);                    // center
        Triangles.Add((i + 1) % 6);          // next (swapped)
        Triangles.Add(i);                    // current
    }

    // Normals (all up)
    for (int i = 0; i < Vertices.Num(); ++i)
        Normals.Add(FVector(0, 0, 1));

    // Simple UV (optional)
    for (int i = 0; i < Vertices.Num(); ++i)
        UV0.Add(FVector2D(0.5f, 0.5f));

    HexMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
}

void AHexTile::OnClicked(AActor* TouchedActor, FKey ButtonPressed)
{
    bIsSelected = !bIsSelected;
    // Update material color here (green/red/etc)
    UE_LOG(LogTemp, Warning, TEXT("Tile clicked at Q=%d R=%d"), GridCoord.Q, GridCoord.R);
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