// Copyright Kyri123 / KMods 2026. All Rights Reserved.



#include "Replication/KLDefaultRCO.h"

#include <Net/UnrealNetwork.h>

#include "Buildable/KlBuildableCleaner.h"
#include "Buildable/ModularMiner/KLMMBuildableBase.h"
#include "Buildable/Slugs/KLBuildableSlugBreeder.h"
#include "Buildable/Slugs/KLBuildableSlugHatchingModule.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"

namespace
{
	bool CanInteractWithBuilding(const UKLDefaultRCO* RCO, AFGBuildable* Building)
	{
		if (!IsValid(RCO) || !IsValid(Building) || !Building->HasAuthority() || Building->GetWorld() != RCO->GetWorld())
		{
			return false;
		}
		const AFGPlayerController* Controller = RCO->GetTypedOuter<AFGPlayerController>();
		if (!IsValid(Controller) || !Controller->HasAuthority() || Controller->GetWorld() != RCO->GetWorld())
		{
			return false;
		}
		AFGCharacterPlayer* Player = Cast<AFGCharacterPlayer>(Controller->GetControlledCharacter());
		if (!IsValid(Player) || Player->GetWorld() != RCO->GetWorld())
		{
			return false;
		}

		AFGBuildable* InteractionBuilding = Building;
		if (const AKPCLModularBuildingBase* Modular = Cast<AKPCLModularBuildingBase>(Building))
		{
			AFGBuildable* Master = Modular->GetMasterBuildable();
			if (Modular->mShouldUseUiFromMaster && IsValid(Master) && Master != Building)
			{
				InteractionBuilding = Master;
			}
		}
		return InteractionBuilding->HasAuthority() && InteractionBuilding->GetWorld() == RCO->GetWorld() &&
			InteractionBuilding->GetInteractingPlayers().Contains(Player);
	}
}

void UKLDefaultRCO::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UKLDefaultRCO, mDummy2);
}

void UKLDefaultRCO::Server_SetCleanerRecipe_Implementation(AFGBuildable* Building,
														   UKAPICleanerItemDescription* NewRecipe)
{
	if (!CanInteractWithBuilding(this, Building))
	{
		return;
	}

	if (AKLBuildableCleaner* Cleaner = Cast<AKLBuildableCleaner>(Building))
	{
		Cleaner->SetCleanerRecipe(NewRecipe);
	}
}

void UKLDefaultRCO::Server_SetTargetHumidity_Implementation(AFGBuildable* Building, float NewTargetHumidity)
{
	if (!CanInteractWithBuilding(this, Building) || !FMath::IsFinite(NewTargetHumidity))
	{
		return;
	}

	if (AKLBuildableSlugHatchingModule* Module = Cast<AKLBuildableSlugHatchingModule>(Building))
	{
		Module->ApplyNewHumidity(NewTargetHumidity);
		Module->ForceNetUpdate();
		return;
	}

	if (AKLBuildableSlugBreeder* Terrarium = Cast<AKLBuildableSlugBreeder>(Building))
	{
		Terrarium->ApplyNewHumidity(NewTargetHumidity);
		Terrarium->ForceNetUpdate();
	}
}

void UKLDefaultRCO::Server_SetTargetTemperature_Implementation(AFGBuildable* Building, float NewTargetTemperature)
{
	if (!CanInteractWithBuilding(this, Building) || !FMath::IsFinite(NewTargetTemperature))
	{
		return;
	}

	if (AKLBuildableSlugHatchingModule* Module = Cast<AKLBuildableSlugHatchingModule>(Building))
	{
		Module->ApplyNewTemperature(NewTargetTemperature);
		Module->ForceNetUpdate();
		return;
	}

	if (AKLBuildableSlugBreeder* Terrarium = Cast<AKLBuildableSlugBreeder>(Building))
	{
		Terrarium->ApplyNewTemperature(NewTargetTemperature);
		Terrarium->ForceNetUpdate();
	}
}

void UKLDefaultRCO::Server_SetNewModuleTime_Implementation(AFGBuildable* Building, EKAPISlugTime NewTime)
{
	if (!CanInteractWithBuilding(this, Building))
	{
		return;
	}

	if (AKLBuildableSlugHatchingModule* Module = Cast<AKLBuildableSlugHatchingModule>(Building))
	{
		Module->ApplyNewOverwriteTime(NewTime);
		Module->ForceNetUpdate();
	}
}
