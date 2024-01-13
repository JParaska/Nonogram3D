// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "N3DSaveGameTypes.h"
#include "N3DSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NONOGRAM3D_API UN3DSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere)
	TArray<FSaveNonogramStatus> Nonograms;
	
};
