// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nonogram.generated.h"

class UN3DNonogramColorScheme;
class UN3DNonogramInput;

class APlayerController;
class UInstancedStaticMeshComponent;

UCLASS()
class NONOGRAM3D_API ANonogram : public AActor
{
	GENERATED_BODY()

#pragma region Properties

protected:

	UPROPERTY(VisibleAnywhere, Category = "Scene")
	TObjectPtr<UInstancedStaticMeshComponent> CubeInstances;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntVector, int> Cubes;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FVector CubeScale = FVector(0.99f);

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TObjectPtr<UN3DNonogramColorScheme> ColorScheme;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UN3DNonogramInput> NonogramInput;

#pragma endregion
	
public:	
	
	ANonogram();

	virtual void EnableInput(class APlayerController* PlayerController) override;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void SelectionX(const FInputActionValue& Input);

	UFUNCTION()
	void SelectionY(const FInputActionValue& Input);

	UFUNCTION()
	void SelectionZ(const FInputActionValue& Input);
};
