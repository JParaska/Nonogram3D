// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "N3DPawn.generated.h"

UCLASS()
class NONOGRAM3D_API AN3DPawn : public APawn
{
	GENERATED_BODY()

public:
	
	AN3DPawn();

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	virtual void BeginPlay() override;

};
