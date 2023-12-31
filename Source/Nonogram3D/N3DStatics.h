// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Nonogram3DTypes.h"
#include "N3DStatics.generated.h"

#define DEFAULT_PROJECT_NAME "Nonogram3D"
#define CREATED_NONOGRAMS "MyCreated"
#define DOWNLOADED_NONOGRAMS "Downloaded"

class AN3DNonogram;

/**
 * 
 */
UCLASS()
class NONOGRAM3D_API UN3DStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Nonogram Editor", meta=(WorldContext="WorldContextObject"))
	static ESaveLoadError SaveNonogram(const UObject* WorldContextObject, const FString& Nonogramname, const TMap<int32, FColor> Solution);
};
