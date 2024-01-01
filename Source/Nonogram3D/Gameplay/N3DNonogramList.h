// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Nonogram3DTypes.h"
#include "N3DNonogramList.generated.h"

/**
 * 
 */
UCLASS()
class NONOGRAM3D_API UN3DNonogramList : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FNonogram> Nonograms;
};
