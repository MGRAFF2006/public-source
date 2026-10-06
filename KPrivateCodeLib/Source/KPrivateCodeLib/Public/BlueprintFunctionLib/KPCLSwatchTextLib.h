// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Subsystem/KPCLSwatchSystem.h"

#include "KPCLSwatchTextLib.generated.h"

UCLASS()
class KPRIVATECODELIB_API UKPCLSwatchTextLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr int32 MaxTextBytes = 256 * 1024;
	static constexpr int32 MaxSwatches = 256;
	static constexpr int32 MaxNameLength = 128;

	UFUNCTION(BlueprintCallable, Category = "KMods|SwatchText")
	static bool ExportSwatches(const TArray<FCustomSwatchData>& Swatches, FString& OutText, FText& OutError);

	UFUNCTION(BlueprintCallable, Category = "KMods|SwatchText")
	static bool ParseSwatches(const FString& Text, TArray<FCustomSwatchData>& OutSwatches, FText& OutError);
};
