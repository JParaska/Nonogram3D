// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "N3DPlayerController.generated.h"

class AN3DNonogram;
class UN3DNonogramList;
class UN3DPlayerInput;

class UInputMappingContext;

/**
 * 
 */
UCLASS()
class NONOGRAM3D_API AN3DPlayerController : public APlayerController
{
	GENERATED_BODY()

#pragma region Properties
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext;
#pragma endregion

#pragma region Methods

public:

	virtual void SetupInputComponent() override;

protected:

	virtual void BeginPlay() override;

#pragma endregion
	
};
