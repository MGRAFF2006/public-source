// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#include "BlueprintFunctionLib/KPCLSwatchTextLib.h"

#include "Containers/StringConv.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

namespace
{
	bool HasValidTextSize(const FString& Text)
	{
		if (Text.IsEmpty() || Text.Len() > UKPCLSwatchTextLib::MaxTextBytes)
		{
			return false;
		}
		return FTCHARToUTF8(*Text).Length() <= UKPCLSwatchTextLib::MaxTextBytes;
	}

	bool HasValidName(const FString& Name)
	{
		if (Name.IsEmpty() || Name.Len() > UKPCLSwatchTextLib::MaxNameLength || Name != Name.TrimStartAndEnd())
		{
			return false;
		}
		for (TCHAR Character : Name)
		{
			if (Character < 32 || Character == 127)
			{
				return false;
			}
		}
		return true;
	}

	bool HasFiniteColor(const FLinearColor& Color)
	{
		return FMath::IsFinite(Color.R) && FMath::IsFinite(Color.G) && FMath::IsFinite(Color.B) &&
			FMath::IsFinite(Color.A);
	}

	TArray<TSharedPtr<FJsonValue>> WriteColor(const FLinearColor& Color)
	{
		return {MakeShared<FJsonValueNumber>(Color.R), MakeShared<FJsonValueNumber>(Color.G),
				MakeShared<FJsonValueNumber>(Color.B), MakeShared<FJsonValueNumber>(Color.A)};
	}

	bool ReadColor(const TSharedPtr<FJsonObject>& Object, const TCHAR* Field, FLinearColor& OutColor)
	{
		const TArray<TSharedPtr<FJsonValue>>* Values = nullptr;
		if (!Object->TryGetArrayField(Field, Values) || Values->Num() != 4)
		{
			return false;
		}
		float Channels[4];
		for (int32 Index = 0; Index < 4; ++Index)
		{
			const TSharedPtr<FJsonValue>& Value = (*Values)[Index];
			if (!Value.IsValid() || Value->Type != EJson::Number)
			{
				return false;
			}
			const double Number = Value->AsNumber();
			if (!FMath::IsFinite(Number) || Number < -MAX_flt || Number > MAX_flt)
			{
				return false;
			}
			Channels[Index] = static_cast<float>(Number);
		}
		OutColor = FLinearColor(Channels[0], Channels[1], Channels[2], Channels[3]);
		return true;
	}
}

bool UKPCLSwatchTextLib::ExportSwatches(const TArray<FCustomSwatchData>& Swatches, FString& OutText, FText& OutError)
{
	OutError = FText::GetEmpty();
	if (Swatches.IsEmpty() || Swatches.Num() > MaxSwatches)
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "InvalidCount", "Choose between 1 and 256 swatches.");
		return false;
	}

	TSet<FName> Names;
	TArray<TSharedPtr<FJsonValue>> Entries;
	for (const FCustomSwatchData& Swatch : Swatches)
	{
		const FString Name = Swatch.mSlotName.ToString();
		if (!HasValidName(Name) || !HasFiniteColor(Swatch.mPrimaryColor) || !HasFiniteColor(Swatch.mSecondaryColor))
		{
			OutError = NSLOCTEXT("KPCLSwatchText", "InvalidSwatch", "Swatches need a name of 1 to 128 characters and finite colors.");
			return false;
		}
		if (Names.Contains(Swatch.mSlotName))
		{
			OutError = NSLOCTEXT("KPCLSwatchText", "DuplicateNames", "Each imported or exported swatch must have a different name.");
			return false;
		}
		Names.Add(Swatch.mSlotName);
		TSharedRef<FJsonObject> Entry = MakeShared<FJsonObject>();
		Entry->SetStringField(TEXT("name"), Name);
		Entry->SetArrayField(TEXT("primary"), WriteColor(Swatch.mPrimaryColor));
		Entry->SetArrayField(TEXT("secondary"), WriteColor(Swatch.mSecondaryColor));
		Entries.Add(MakeShared<FJsonValueObject>(Entry));
	}
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetNumberField(TEXT("version"), 1);
	Root->SetArrayField(TEXT("swatches"), Entries);
	FString Text;
	if (!FJsonSerializer::Serialize(Root, TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Text)) ||
		!HasValidTextSize(Text))
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "ExportFailed", "The selected swatches could not be exported within the text size limit.");
		return false;
	}
	OutText = MoveTemp(Text);
	return true;
}

bool UKPCLSwatchTextLib::ParseSwatches(const FString& Text, TArray<FCustomSwatchData>& OutSwatches, FText& OutError)
{
	OutError = FText::GetEmpty();
	if (!HasValidTextSize(Text))
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "InvalidTextSize", "Swatch text must be nonempty and no larger than 256 KiB.");
		return false;
	}
	TSharedPtr<FJsonObject> Root;
	if (!FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Text), Root) || !Root.IsValid())
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "InvalidJson", "The swatch text is not valid JSON.");
		return false;
	}
	const TSharedPtr<FJsonValue> Version = Root->TryGetField(TEXT("version"));
	const TArray<TSharedPtr<FJsonValue>>* Entries = nullptr;
	if (!Version.IsValid() || Version->Type != EJson::Number || Version->AsNumber() != 1 ||
		!Root->TryGetArrayField(TEXT("swatches"), Entries) || Entries->IsEmpty() || Entries->Num() > MaxSwatches)
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "UnsupportedFormat", "Use version 1 swatch text containing between 1 and 256 swatches.");
		return false;
	}

	TSet<FName> Names;
	TArray<FCustomSwatchData> Parsed;
	for (const TSharedPtr<FJsonValue>& Value : *Entries)
	{
		if (!Value.IsValid() || Value->Type != EJson::Object)
		{
			OutError = NSLOCTEXT("KPCLSwatchText", "InvalidEntry", "Every swatch must contain a name and two colors with four numeric channels.");
			return false;
		}
		TSharedPtr<FJsonObject> Entry = Value->AsObject();
		FString Name;
		FCustomSwatchData Swatch;
		const TSharedPtr<FJsonValue> NameValue = Entry->TryGetField(TEXT("name"));
		if (!NameValue.IsValid() || NameValue->Type != EJson::String ||
			!Entry->TryGetStringField(TEXT("name"), Name) || !HasValidName(Name) ||
			!ReadColor(Entry, TEXT("primary"), Swatch.mPrimaryColor) ||
			!ReadColor(Entry, TEXT("secondary"), Swatch.mSecondaryColor))
		{
			OutError = NSLOCTEXT("KPCLSwatchText", "InvalidEntry", "Every swatch must contain a name and two colors with four numeric channels.");
			return false;
		}
		Swatch.mSlotName = FName(*Name);
		if (Names.Contains(Swatch.mSlotName))
		{
			OutError = NSLOCTEXT("KPCLSwatchText", "DuplicateNames", "Each imported or exported swatch must have a different name.");
			return false;
		}
		Names.Add(Swatch.mSlotName);
		Parsed.Add(Swatch);
	}
	OutSwatches = MoveTemp(Parsed);
	return true;
}
