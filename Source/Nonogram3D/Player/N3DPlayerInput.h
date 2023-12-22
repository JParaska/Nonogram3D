// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "N3DPlayerInput.generated.h"

class UInputAction;

/**
 * 
 */
UCLASS()
class NONOGRAM3D_API UN3DPlayerInput : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> TriggerLookAround;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SelectCube;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAround;
	
};
