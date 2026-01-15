// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HexCoord.generated.h"

USTRUCT(BlueprintType)
struct HEXGRID_API FHexCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Q; // column
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 R; // row

	FHexCoord() : Q(0), R(0) {}
	FHexCoord(int32 InQ, int32 InR) : Q(InQ), R(InR) {}

	FORCEINLINE int32 S() const
	{
		return -Q - R;
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("(%d, %d)"), Q, R);
	}

	bool operator==(const FHexCoord& Other) const
	{
		return Q == Other.Q && R == Other.R;
	}

	FHexCoord operator+(const FHexCoord& Other) const
	{
		return FHexCoord(Q + Other.Q, R + Other.R);
	}

    static TArray<FHexCoord> GetHexLine(const FHexCoord& Start, const FHexCoord& End);

	static const TArray<FHexCoord>& Directions();
};


struct FCubeCoord
{
    int32 X, Y, Z;

    FCubeCoord() : X(0), Y(0), Z(0) {}
    FCubeCoord(int32 InX, int32 InY, int32 InZ) : X(InX), Y(InY), Z(InZ) {}

    // Convert from axial
    explicit FCubeCoord(const FHexCoord& Axial)
        : X(Axial.Q), Y(Axial.R), Z(Axial.S())
    {
    }

    // Convert back to axial
    FHexCoord ToAxial() const
    {
        return FHexCoord(X, Y);
    }

    FORCEINLINE bool operator==(const FCubeCoord& Other) const
    {
        return X == Other.X && Y == Other.Y && Z == Other.Z;
    }
};

FORCEINLINE FCubeCoord CubeLerp(const FCubeCoord& A, const FCubeCoord& B, float t)
{
    return FCubeCoord(
        FMath::RoundToInt(A.X + (B.X - A.X) * t),
        FMath::RoundToInt(A.Y + (B.Y - A.Y) * t),
        FMath::RoundToInt(A.Z + (B.Z - A.Z) * t)
    );
}

FORCEINLINE FCubeCoord CubeRound(float fx, float fy, float fz)
{
    int32 rx = FMath::RoundToInt(fx);
    int32 ry = FMath::RoundToInt(fy);
    int32 rz = FMath::RoundToInt(fz);

    float x_diff = FMath::Abs(static_cast<float>(rx) - fx);
    float y_diff = FMath::Abs(static_cast<float>(ry) - fy);
    float z_diff = FMath::Abs(static_cast<float>(rz) - fz);

    if (x_diff > y_diff && x_diff > z_diff)
    {
        rx = -ry - rz;
    }
    else if (y_diff > z_diff)
    {
        ry = -rx - rz;
    }
    else
    {
        rz = -rx - ry;
    }

    return FCubeCoord(rx, ry, rz);
}


// Needed for hash containers like TMap and TSet
//		https://en.cppreference.com/w/cpp/language/adl.html
FORCEINLINE uint32 GetTypeHash(const FHexCoord& C)
{
	return HashCombine(::GetTypeHash(C.Q), ::GetTypeHash(C.R));
}

FORCEINLINE int32 AxialDistance(const FHexCoord& A, const FHexCoord& B)
{
	return (FMath::Abs(A.Q - B.Q)
		+ FMath::Abs(A.R - B.R)
		+ FMath::Abs(A.S() - B.S())) / 2;
}

