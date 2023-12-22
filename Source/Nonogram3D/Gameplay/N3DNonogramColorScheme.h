// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "N3DNonogramColorScheme.generated.h"

USTRUCT(Blueprintable)
struct FN3DColorScheme {

	GENERATED_BODY()

	// Per instance custom data 0, 1, 2
	UPROPERTY(EditAnywhere)
	FColor Color = FColor::White;

	// Per instance custom data 3
	UPROPERTY(EditAnywhere)
	float ColorIntensity = 10.0f;

	// Per instance custom data 4
	UPROPERTY(EditAnywhere)
	float EdgeSharpness = 30.0f;

	// Per instance custom data 5
	UPROPERTY(EditAnywhere)
	float EdgeSubtraction = 0.1f;
};

/**
 * 
 */
UCLASS()
class NONOGRAM3D_API UN3DNonogramColorScheme : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FN3DColorScheme FilledActive;

	UPROPERTY(EditAnywhere)
	FN3DColorScheme FilledInactive;

	UPROPERTY(EditAnywhere)
	FN3DColorScheme EmptyActive;

	UPROPERTY(EditAnywhere)
	FN3DColorScheme EmptyInactive;
};
