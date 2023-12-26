// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nonogram.generated.h"


class UInstancedStaticMeshComponent;
class UN3DNonogramColorScheme;

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

#pragma endregion
	
public:	
	
	ANonogram();

protected:
	
	virtual void BeginPlay() override;

};
