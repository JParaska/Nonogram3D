// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "N3DNonogramInput.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class NONOGRAM3D_API UN3DNonogramInput : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SelectionX;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SelectionY;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SelectionZ;
};
