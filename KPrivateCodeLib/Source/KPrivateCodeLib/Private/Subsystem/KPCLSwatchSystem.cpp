// Copyright Kyri123 / KMods 2026. All Rights Reserved.



#include "Subsystem/KPCLSwatchSystem.h"

#include "BFL/KBFL_Player.h"
#include "BFL/KBFL_Util.h"
#include "BlueprintFunctionLib/KPCLSwatchTextLib.h"
#include "Net/UnrealNetwork.h"
#include "Replication/KPCLDefaultRCO.h"

void AKPCLSwatchSystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKPCLSwatchSystem, mCustomSwatchData);
}

AKPCLSwatchSystem::AKPCLSwatchSystem()
{
	mShouldSave = true;
	PrimaryActorTick.bCanEverTick = true;
	ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnServer_Replicate;
}

AKPCLSwatchSystem* AKPCLSwatchSystem::Get(UObject* worldContext)
{
	return UKBFL_Util::GetSubsystem<AKPCLSwatchSystem>(worldContext);
}

void AKPCLSwatchSystem::AddCustomSwatchData(FCustomSwatchData Data)
{
	if (HasAuthority())
	{
		mCustomSwatchData.Add(Data);
		OnRep_CustomSwatchData();
	}
	else if (UKPCLDefaultRCO* RCO = UKPCLDefaultRCO::Get(GetWorld()))
	{
		RCO->Server_AddCustomSwatchData(this, Data);
	}
}

void AKPCLSwatchSystem::RemoveCustomSwatchData(int32 Idx)
{
	if (HasAuthority())
	{
		if (mCustomSwatchData.IsValidIndex(Idx))
		{
			const FCustomSwatchData OldCustomSwatchData = mCustomSwatchData[Idx];
			TArray<FCustomSwatchData> NewArray;

			bool WasRemove = false;
			for (FCustomSwatchData CustomSwatchData : mCustomSwatchData)
			{
				if (CustomSwatchData != OldCustomSwatchData || WasRemove)
				{
					NewArray.Add(CustomSwatchData);
				}
				else
				{
					WasRemove = true;
				}
			}

			mCustomSwatchData = NewArray;
			OnRep_CustomSwatchData();
		}
	}
	else if (UKPCLDefaultRCO* RCO = UKPCLDefaultRCO::Get(GetWorld()))
	{
		RCO->Server_RemoveCustomSwatchData(this, Idx);
	}
}

void AKPCLSwatchSystem::UseCustomSwatchData(int32 Idx)
{
	AFGPlayerState* State = UKBFL_Player::GetFgPlayerState(GetWorld());
	if (State && mCustomSwatchData.IsValidIndex(Idx))
	{
		FFactoryCustomizationColorSlot Slot = State->GetCustomColorData();
		Slot.PrimaryColor = mCustomSwatchData[Idx].mPrimaryColor;
		Slot.SecondaryColor = mCustomSwatchData[Idx].mSecondaryColor;
		State->SetPlayerCustomizationSlotData(Slot);
	}
}

void AKPCLSwatchSystem::UpdateCustomSwatchData(FCustomSwatchData Data, int32 Idx)
{
	if (HasAuthority())
	{
		if (mCustomSwatchData.IsValidIndex(Idx))
		{
			mCustomSwatchData[Idx] = Data;
			OnRep_CustomSwatchData();
		}
	}
	else if (UKPCLDefaultRCO* RCO = UKPCLDefaultRCO::Get(GetWorld()))
	{
		RCO->Server_UpdateCustomSwatchData(this, Data, Idx);
	}
}

TArray<FCustomSwatchData> AKPCLSwatchSystem::GetCustomSwatchData() const { return mCustomSwatchData; }

bool AKPCLSwatchSystem::ExportCustomSwatchText(const TArray<int32>& Indices, FString& OutText, FText& OutError) const
{
	OutError = FText::GetEmpty();
	if (Indices.IsEmpty() || Indices.Num() > UKPCLSwatchTextLib::MaxSwatches)
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "InvalidCount", "Choose between 1 and 256 swatches.");
		return false;
	}
	TArray<FCustomSwatchData> Swatches;
	for (int32 Index : Indices)
	{
		if (!mCustomSwatchData.IsValidIndex(Index))
		{
			OutError = NSLOCTEXT("KPCLSwatchText", "InvalidSelection", "One of the selected swatches no longer exists.");
			return false;
		}
		Swatches.Add(mCustomSwatchData[Index]);
	}
	return UKPCLSwatchTextLib::ExportSwatches(Swatches, OutText, OutError);
}

bool AKPCLSwatchSystem::ImportCustomSwatchText(const FString& Text, FText& OutError)
{
	OutError = FText::GetEmpty();
	if (!HasAuthority())
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "AuthorityRequired", "Swatches must be imported on the host or server.");
		return false;
	}
	TArray<FCustomSwatchData> Swatches;
	if (!UKPCLSwatchTextLib::ParseSwatches(Text, Swatches, OutError))
	{
		return false;
	}
	if (mCustomSwatchData.Num() > UKPCLSwatchTextLib::MaxSwatches - Swatches.Num())
	{
		OutError = NSLOCTEXT("KPCLSwatchText", "CollectionFull", "Import would exceed the limit of 256 saved swatches.");
		return false;
	}
	for (const FCustomSwatchData& Swatch : Swatches)
	{
		if (mCustomSwatchData.ContainsByPredicate([&Swatch](const FCustomSwatchData& Existing)
												   { return Existing.mSlotName == Swatch.mSlotName; }))
		{
			OutError = NSLOCTEXT("KPCLSwatchText", "ExistingName", "A swatch with an imported name already exists. Rename or remove it first.");
			return false;
		}
	}
	mCustomSwatchData.Append(Swatches);
	OnRep_CustomSwatchData();
	ForceNetUpdate();
	return true;
}

void AKPCLSwatchSystem::OnRep_CustomSwatchData()
{
	if (mOnSwatchDataUpdated.IsBound())
	{
		mOnSwatchDataUpdated.Broadcast();
	}
}
