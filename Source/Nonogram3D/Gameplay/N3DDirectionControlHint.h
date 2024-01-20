// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "N3DDirectionControlHint.generated.h"

class UArrowComponent;
class USpringArmComponent;
class USceneCaptureComponent2D;
class UWidgetComponent ;

UCLASS()
class NONOGRAM3D_API AN3DDirectionControlHint : public AActor
{
	GENERATED_BODY()

#pragma region Properties
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowW;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowS;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowD;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowA;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowE;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UArrowComponent> ArrowQ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|UI")
	TObjectPtr<UWidgetComponent > KeyW;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|UI")
	TObjectPtr<UWidgetComponent > KeyS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|UI")
	TObjectPtr<UWidgetComponent > KeyD;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|UI")
	TObjectPtr<UWidgetComponent > KeyA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|UI")
	TObjectPtr<UWidgetComponent > KeyE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components|UI")
	TObjectPtr<UWidgetComponent > KeyQ;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components|Scene Capture")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, Category = "Components|Scene Capture")
	TObjectPtr<USceneCaptureComponent2D> SceneCapture;


#pragma endregion
	
public:	
	// Sets default values for this actor's properties
	AN3DDirectionControlHint();

};
