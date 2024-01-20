// Created by Jan Paraska. All rights reserved


#include "N3DDirectionControlHint.h"

#include "Components/ArrowComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AN3DDirectionControlHint::AN3DDirectionControlHint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ArrowW = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowW"));
	ArrowW->SetupAttachment(RootComponent);

	ArrowS = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowS"));
	ArrowS->SetupAttachment(RootComponent);

	ArrowD = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowD"));
	ArrowD->SetupAttachment(RootComponent);

	ArrowA = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowA"));
	ArrowA->SetupAttachment(RootComponent);

	ArrowE = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowE"));
	ArrowE->SetupAttachment(RootComponent);

	ArrowQ = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowQ"));
	ArrowQ->SetupAttachment(RootComponent);

	KeyW = CreateDefaultSubobject<UWidgetComponent>(TEXT("KeyW"));
	KeyW->SetupAttachment(ArrowW);

	KeyS = CreateDefaultSubobject<UWidgetComponent>(TEXT("KeyS"));
	KeyS->SetupAttachment(ArrowS);

	KeyD = CreateDefaultSubobject<UWidgetComponent>(TEXT("KeyD"));
	KeyD->SetupAttachment(ArrowD);

	KeyA = CreateDefaultSubobject<UWidgetComponent>(TEXT("KeyA"));
	KeyA->SetupAttachment(ArrowA);

	KeyE = CreateDefaultSubobject<UWidgetComponent>(TEXT("KeyE"));
	KeyE->SetupAttachment(ArrowE);

	KeyQ = CreateDefaultSubobject<UWidgetComponent>(TEXT("KeyQ"));
	KeyQ->SetupAttachment(ArrowQ);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(SpringArm);
}
