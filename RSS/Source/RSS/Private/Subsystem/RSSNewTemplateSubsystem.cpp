// Copyright Kyri123 / KMods 2026. All Rights Reserved.



#include "Subsystem/RSSNewTemplateSubsystem.h"

#include "Serialization/RssJsonSerializer.h"
#include "Subsystem/RSSImageSubsystem.h"
#include "Subsystem/RSSTemplateSubsystem.h"
#include "HAL/FileManager.h"
#include "RssBlueprintFunctionLibrary.h"

namespace
{
	bool ParseTemplates(UObject* WorldContext, const FString& Content, FRssTemplates& OutTemplates)
	{
		FRssTemplates Parsed;
		const FString Trimmed = Content.TrimStartAndEnd();
		if (Trimmed.StartsWith(TEXT("{")))
		{
			if (!FRssJsonSerializer::JsonToTemplates(Content, Parsed.mTemplates))
			{
				return false;
			}
		}
		else
		{
			if (!ARSSImageSubsystem::CheckString(Content, {"mTemplates"}))
			{
				return false;
			}
			const TCHAR* End = FRssTemplates::StaticStruct()->ImportText(*Content, &Parsed, WorldContext,
				PPF_Copy | PPF_Delimited | PPF_IncludeTransient, nullptr, "FRssTemplates");
			if (!End || !FString(End).TrimStartAndEnd().IsEmpty())
			{
				return false;
			}
		}
		for (const FRssSignData& Template : Parsed.mTemplates)
		{
			if (!URssBlueprintFunctionLibrary::IsSignDataSafe(Template))
			{
				return false;
			}
		}
		OutTemplates = MoveTemp(Parsed);
		return true;
	}
}

ARSSNewTemplateSubsystem::ARSSNewTemplateSubsystem()
{

	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	ReplicationPolicy = ESubsystemReplicationPolicy::SpawnLocal;
}

void ARSSNewTemplateSubsystem::BeginPlay() { Super::BeginPlay(); }

bool ARSSNewTemplateSubsystem::GenerateFromFileInVar()
{
	FString FileContent;
	FRssTemplates Parsed;
	if (!ReadTemplatesFromFile(FileContent) || !ParseTemplates(this, FileContent, Parsed))
	{
		return false;
	}
	mTemplates = MoveTemp(Parsed);
	return true;
}

bool ARSSNewTemplateSubsystem::ReadTemplatesFromFile(FString& FileContent)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString FilePath = GetFilePath();

	if (FileManager.FileExists(*FilePath))
	{
		return FFileHelper::LoadFileToString(FileContent, *FilePath, FFileHelper::EHashOptions::None);
	}

	return false;
}

FString ARSSNewTemplateSubsystem::GetFilePath()
{
	FString file = FPaths::ProjectConfigDir();
	if (mAddFilePath != "")
	{
		file.Append(mAddFilePath);
	}
	file.Append(mFileName);

	return file;
}

bool ARSSNewTemplateSubsystem::MigrateFromOld()
{
	if (!HasAuthority())
	{
		return false;
	}
	ARSSTemplateSubsystem* Subsystem = ARSSTemplateSubsystem::GetOldRSSTemplateSubsystem(this);
	if (!IsValid(Subsystem) || Subsystem->HasAnyFlags(RF_ClassDefaultObject) ||
		Subsystem->GetWorld() != GetWorld() || !Subsystem->HasAuthority())
	{
		return false;
	}
	FRssTemplates Migrated = mTemplates;
	if (IFileManager::Get().FileExists(*GetFilePath()))
	{
		FString Content;
		if (!ReadTemplatesFromFile(Content) || !ParseTemplates(this, Content, Migrated))
		{
			return false;
		}
	}
	for (const FRssSignData& Template : Subsystem->mTemplates)
	{
		if (!URssBlueprintFunctionLibrary::IsSignDataSafe(Template))
		{
			return false;
		}
	}
	const FRssTemplates PreviousTemplates = mTemplates;
	Migrated.mTemplates.Append(Subsystem->mTemplates);
	mTemplates = MoveTemp(Migrated);
	if (!SaveTemplatesToFile())
	{
		mTemplates = PreviousTemplates;
		return false;
	}
	Subsystem->ClearTemplates();
	return true;
}

bool ARSSNewTemplateSubsystem::SaveTemplatesToFile()
{
	const FString Data = FRssJsonSerializer::TemplatesToJson(mTemplates.mTemplates);
	if (Data.IsEmpty())
	{
		return false;
	}
	const FString FilePath = GetFilePath();

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	const FString BackupPath = FilePath + TEXT(".bak");
	if (FileManager.FileExists(*FilePath) && !FileManager.FileExists(*BackupPath))
	{
		if (!FileManager.CopyFile(*BackupPath, *FilePath))
		{

			UE_LOG(LogTemp, Error, TEXT("RSS: failed to back up %s aborting save"), *FilePath);
			return false;
		}
	}

	const FString TemporaryPath = FilePath + TEXT(".tmp-") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
	IFileManager& Files = IFileManager::Get();
	if (!FFileHelper::SaveStringToFile(Data, *TemporaryPath))
	{
		Files.Delete(*TemporaryPath);
		return false;
	}
	if (!Files.Move(*FilePath, *TemporaryPath, true, false, false, true))
	{
		UE_LOG(LogTemp, Error, TEXT("RSS: template replacement failed; staged catalog path: %s"), *TemporaryPath);
		return false;
	}
	return true;
}

FString ARSSNewTemplateSubsystem::TemplatesToString(UObject* WorldContext, FRssTemplates Templates)
{
	const UScriptStruct* Struct = Templates.StaticStruct();
	FString Output = TEXT("");
	Struct->ExportText(Output, &Templates, new FRssTemplates, WorldContext,
					   (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr);
	return Output;
}

FRssTemplates ARSSNewTemplateSubsystem::StringToTemplates(UObject* WorldContext, FString Templates)
{
	FRssTemplates OutStruc = FRssTemplates();

	static UScriptStruct* Struct = OutStruc.StaticStruct();
	Struct->ImportText(*Templates, &OutStruc, WorldContext, (PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr,
					   "FRssTemplates");

	return OutStruc;
}

TArray<FRssTemplateSignData> ARSSNewTemplateSubsystem::GetTemplates(ESignType SignType)
{
	TArray<FRssTemplateSignData> Array;
	for (int i = 0; i < mTemplates.mTemplates.Num(); ++i)
	{
		FRssTemplateSignData Info;
		Info.mIndex = i;
		Info.mSignData = mTemplates.mTemplates[i];
		if (mTemplates.mTemplates[i].mSignType == SignType)
		{
			Array.Add(Info);
		}
	}
	return Array;
}

TArray<FRssTemplateSignData> ARSSNewTemplateSubsystem::GetAllTemplates()
{
	TArray<FRssTemplateSignData> Array;
	for (int i = 0; i < mTemplates.mTemplates.Num(); ++i)
	{
		FRssTemplateSignData Info;
		Info.mIndex = i;
		Info.mSignData = mTemplates.mTemplates[i];
		Array.Add(Info);
	}

	return Array;
}

TArray<FRssTemplateSignData> ARSSNewTemplateSubsystem::GetTemplatesOfSignSize(ESignSize SignSize,
																			  ESignType SignType) const
{
	TArray<FRssTemplateSignData> Array;
	for (int i = 0; i < mTemplates.mTemplates.Num(); ++i)
	{
		FRssTemplateSignData Info;
		Info.mIndex = i;
		Info.mSignData = mTemplates.mTemplates[i];
		if (mTemplates.mTemplates[i].mSignTypeSize == SignSize && mTemplates.mTemplates[i].mSignType == SignType)
		{
			Array.Add(Info);
		}
	}
	return Array;
}

TArray<FRssTemplateSignData> ARSSNewTemplateSubsystem::GetAllTemplatesOfSignSize(ESignSize SignSize) const
{
	TArray<FRssTemplateSignData> Array;
	for (int i = 0; i < mTemplates.mTemplates.Num(); ++i)
	{
		FRssTemplateSignData Info;
		Info.mIndex = i;
		Info.mSignData = mTemplates.mTemplates[i];
		if (mTemplates.mTemplates[i].mSignTypeSize == SignSize)
		{
			Array.Add(Info);
		}
	}
	return Array;
}

void ARSSNewTemplateSubsystem::RenameTemplate(const FText& NewName, int TemplateIdx)
{
	if (mTemplates.mTemplates.IsValidIndex(TemplateIdx))
	{
		mTemplates.mTemplates[TemplateIdx].mTemplateData.mTemplateName = NewName;
		SaveTemplatesToFile();
	}
}

void ARSSNewTemplateSubsystem::AddTemplate(FRssSignData Data)
{
	mTemplates.mTemplates.Add(Data);
	SaveTemplatesToFile();
}

void ARSSNewTemplateSubsystem::RemoveTemplate(int TemplateIdx)
{
	if (!mTemplates.mTemplates.IsValidIndex(TemplateIdx))
	{
		return;
	}
	mTemplates.mTemplates.RemoveAt(TemplateIdx);
	SaveTemplatesToFile();
}
