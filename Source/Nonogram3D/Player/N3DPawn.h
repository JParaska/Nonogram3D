// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "N3DPawn.generated.h"

class AN3DNonogram;
class AN3DDirectionControlHint;
class UN3DPlayerInput;

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class NONOGRAM3D_API AN3DPawn : public APawn
{
	GENERATED_BODY()

#pragma region Properties

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera",  meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MinSpringArmLength = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float MaxSpingArmLength = 7000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraZoomStep = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraZoomSpeed = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float DesiredCameraZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UN3DPlayerInput> PlayerInputConfig;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Nonogram")
	TObjectPtr<AN3DNonogram> Nonogram;

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "Nonogram")
	TObjectPtr<AN3DDirectionControlHint> DirectionControlHint;

#pragma endregion

#pragma region Methods
public:
	
	AN3DPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	AN3DNonogram* GetNonogram() const { return Nonogram; }

protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void TriggerLookAround(const FInputActionValue& Input);

	UFUNCTION()
	void LookAround(const FInputActionValue& Input);

	UFUNCTION()
	void CameraZoom(const FInputActionValue& Input);

#pragma endregion
};
