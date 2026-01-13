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

	static const TArray<FHexCoord>& Directions();
};

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