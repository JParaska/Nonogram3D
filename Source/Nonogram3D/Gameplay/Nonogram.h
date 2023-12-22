// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nonogram.generated.h"

UCLASS()
class NONOGRAM3D_API ANonogram : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ANonogram();

protected:
	
	virtual void BeginPlay() override;

};
