// Created by Jan Paraska. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Nonogram3DTypes.h"
#include "N3DStatics.generated.h"

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
	static ESaveLoadError SaveNonogram(const UObject* WorldContextObject, const FString& Nonogramname, FIntVector Size, TMap<int32,FColor> Solution);

	static void FindNonogramsOnDrive(const FString& SubForlder, TArray<FNonogram>& LoadedNonograms);

	UFUNCTION(BlueprintCallable, Category = "Nonogram Editor", meta=(WorldContext="WorldContextObject"))
	static void AddNonogramEditorColor(const UObject* WorldContextObject, const FLinearColor& Color);

	UFUNCTION(BlueprintCallable, Category = "Nonogram Editor", meta=(WorldContext="WorldContextObject"))
	static void GetNonogrameditorColors(const UObject* WorldContextObject, TArray<FLinearColor>& EditorColors);

	UFUNCTION(BlueprintCallable, Category = "Nonogram Utils", meta=(WorldContext="WorldContextObject"))
	static bool GetNonogramName(const UObject* WorldContextObject, const int Index, const ENonogramType Type, FString& NonogramName);

	static bool GetNonogram(const UObject* WorldContextObject, const int Index, const ENonogramType Type, FNonogram& Nonogram);

	static bool GetCreatedNonogram(const UObject* WorldContextObject, const FString& NonogramName, FNonogram& Nonogram);

	static UN3DGameInstance* GetN3DGameInstance(const UObject* WorldContextObject);
};
