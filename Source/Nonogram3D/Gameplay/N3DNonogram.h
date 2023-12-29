// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "N3DNonogram.generated.h"

class UN3DNonogramColorScheme;
class UN3DNonogramInput;

class APlayerController;
class UDataTable;
class UInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class ESelectionType : uint8
{
	X,
	Y,
	Z
};

USTRUCT(BlueprintType)
struct FSelectionPlane
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TSet<int> Plane;
};

USTRUCT(BlueprintType)
struct FSelectionCollection
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TMap<int, FSelectionPlane> Collection;
};

UCLASS()
class NONOGRAM3D_API AN3DNonogram : public AActor
{
	GENERATED_BODY()

#pragma region Properties

protected:

	UPROPERTY(VisibleAnywhere, Category = "Scene")
	TObjectPtr<UInstancedStaticMeshComponent> CubeInstances;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FVector CubeScale = FVector(0.99f);

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TObjectPtr<UN3DNonogramColorScheme> ColorScheme;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TObjectPtr<UDataTable> TestSolution; // TODO remove

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UN3DNonogramInput> NonogramInput;

	UPROPERTY(VisibleAnywhere)
	FIntVector CurrentSize;

	UPROPERTY(VisibleAnywhere)
	TMap<FIntVector, int> Cubes;

	UPROPERTY(VisibleAnywhere)
	TMap<ESelectionType, FSelectionCollection> SelectionCollection;

	TPair<ESelectionType, int> CurrentSelection;

	TObjectPtr<APlayerController> Controller;

	TMap<int32, FColor> CurrentSolution;
	TSet<int32> SelectedCubes;

#pragma endregion
	
public:	
	
	AN3DNonogram();

	virtual void EnableInput(APlayerController* PlayerController) override;

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void SelectionX(const FInputActionValue& Input);

	UFUNCTION()
	void SelectionY(const FInputActionValue& Input);

	UFUNCTION()
	void SelectionZ(const FInputActionValue& Input);

	UFUNCTION()
	void SelectCube();

private:

	void ResetSelectionCollection(const FIntVector& Size);

	FSelectionCollection CreateSelectionCollection(const int Size);

	void AddInstanceToCollection(const FIntVector& Index, const int InstanceIndex);

	void SelectNext(const ESelectionType Selection, const bool bNext);

	void Select(const ESelectionType Selection, const int Index);

	void SetSolution(UDataTable* SolutionDataTable);

	/** Returns true if all (and only) cubes in solution are selected */
	bool CheckSolution() const;
};
