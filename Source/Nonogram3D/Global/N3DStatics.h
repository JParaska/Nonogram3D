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
class UN3DGameInstance;

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

	UFUNCTION(BlueprintCallable, Category = "Nonogram Editor", meta=(WorldContext="WorldContextObject"))
	static void AddNonogramEditorColor(const UObject* WorldContextObject, const FLinearColor& Color);

	UFUNCTION(BlueprintCallable, Category = "Nonogram Editor", meta=(WorldContext="WorldContextObject"))
	static void GetNonogrameditorColors(const UObject* WorldContextObject, TArray<FLinearColor>& EditorColors);

	UFUNCTION(BlueprintCallable, Category = "Nonogram Utils", meta=(WorldContext="WorldContextObject"))
	static bool GetNonogramName(const UObject* WorldContextObject, const int Index, FString& NonogramName);

	static bool GetNonogram(const UObject* WorldContextObject, const int Index, FNonogram& Nonogram);

	static UN3DGameInstance* GetN3DGameInstance(const UObject* WorldContextObject);
};
