// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "N3DPlayerController.generated.h"

class AN3DNonogram;
class AN3DPawn;
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

	UPROPERTY(BlueprintReadOnly, Category = "Nonogram")
	TObjectPtr<AN3DPawn> N3DPawn;

	UPROPERTY(BlueprintReadOnly, Category = "Nonogram")
	TObjectPtr<AN3DNonogram> N3DNonogram;
#pragma endregion

#pragma region Methods

public:

	virtual void SetupInputComponent() override;

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

#pragma endregion
	
};
