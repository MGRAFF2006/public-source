// Copyright Kyri123 / KMods 2026. All Rights Reserved.



#include "Utils/KBFLUtilItemCreaterBuildable.h"

#include "Cpp/KBFLCppInventoryHelper.h"
#include "FGCharacterPlayer.h"
#include "FGFactoryConnectionComponent.h"
#include "FGGameSession.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "FGPlayerController.h"
#include "FGPowerInfoComponent.h"

#include "Net/UnrealNetwork.h"

namespace
{
	AFGCharacterPlayer* GetRequestingCharacter(const UKBFLDefaultRCO* RCO)
	{
		const AFGPlayerController* Controller = IsValid(RCO) ? RCO->GetTypedOuter<AFGPlayerController>() : nullptr;
		if (!IsValid(Controller) || !Controller->HasAuthority() || Controller->GetWorld() != RCO->GetWorld())
		{
			return nullptr;
		}
		AFGCharacterPlayer* Player = Cast<AFGCharacterPlayer>(Controller->GetControlledCharacter());
		return IsValid(Player) && Player->GetWorld() == RCO->GetWorld() ? Player : nullptr;
	}

	bool HasDebugPermission(const UKBFLDefaultRCO* RCO)
	{
		const AFGPlayerController* Controller = IsValid(RCO) ? RCO->GetTypedOuter<AFGPlayerController>() : nullptr;
		if (!IsValid(Controller) || !Controller->HasAuthority() || Controller->GetWorld() != RCO->GetWorld())
		{
			return false;
		}
		if (Controller->IsLocalController())
		{
			return true;
		}
		const AGameModeBase* GameMode = IsValid(Controller->GetWorld()) ? Controller->GetWorld()->GetAuthGameMode() : nullptr;
		const AFGGameSession* Session = IsValid(GameMode) ? Cast<AFGGameSession>(GameMode->GameSession) : nullptr;
		return IsValid(Session) && Session->IsPlayerAdmin(Controller);
	}

	bool CanConfigureDebugBuilding(const UKBFLDefaultRCO* RCO, AKBFLUtilItemCreaterBuildable* Building)
	{
		AFGCharacterPlayer* Player = GetRequestingCharacter(RCO);
		return IsValid(Player) && IsValid(Building) && Building->HasAuthority() &&
			Building->GetWorld() == Player->GetWorld() && HasDebugPermission(RCO) &&
			Building->GetInteractingPlayers().Contains(Player);
	}
}

AKBFLUtilItemCreaterBuildable::AKBFLUtilItemCreaterBuildable() :
	mPipeInput(nullptr), mPipeOutput(nullptr), mBeltInput(nullptr), mBeltOutput(nullptr)
{
	mPowerProduction = 1000000000.f;
}

void AKBFLUtilItemCreaterBuildable::Factory_Tick(float dt)
{
	if (HasAuthority())
	{
		UKBFLCppInventoryHelper::PullBeltChildClass(GetStorageInventory(), 3, dt, UFGItemDescriptor::StaticClass(),
													mBeltInput);
		UKBFLCppInventoryHelper::PullAllFromPipe(GetStorageInventory(), 2, dt, mPipeInput);
		DestroyItems();
		CreateItems();
		UKBFLCppInventoryHelper::PushPipe(GetStorageInventory(), 1, dt, mPipeOutput);
		mBeltOutput->SetInventory(GetStorageInventory());
	}

	Super::Factory_Tick(dt);
}

void AKBFLUtilItemCreaterBuildable::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Component : Components)
	{

		if (UFGFactoryConnectionComponent* ConveyorConnection = Cast<UFGFactoryConnectionComponent>(Component))
		{
			if (ConveyorConnection->GetDirection() == EFactoryConnectionDirection::FCD_INPUT)
			{
				mBeltInput = ConveyorConnection;
			}
			else if (ConveyorConnection->GetDirection() == EFactoryConnectionDirection::FCD_OUTPUT)
			{
				mBeltOutput = ConveyorConnection;
				mBeltOutput->SetInventoryAccessIndex(0);
			}
		}

		if (UFGPipeConnectionFactory* PipeConnection = Cast<UFGPipeConnectionFactory>(Component))
		{
			if (PipeConnection->GetPipeConnectionType() == EPipeConnectionType::PCT_CONSUMER)
			{
				mPipeInput = PipeConnection;
			}
			else if (PipeConnection->GetPipeConnectionType() == EPipeConnectionType::PCT_PRODUCER)
			{
				mPipeOutput = PipeConnection;
			}

			if (!PipeConnection->mApplyAdditionalPressure && HasAuthority())
			{
				PipeConnection->mApplyAdditionalPressure = true;
			}
		}
	}
}

void AKBFLUtilItemCreaterBuildable::BeginPlay()
{
	Super::BeginPlay();

	if (GetStorageInventory() && HasAuthority())
	{
		GetStorageInventory()->Resize(4);

		if (GetPowerInfo())
		{
			GetPowerInfo()->SetBaseProduction(mPowerProduction);
		}
	}
}

void AKBFLUtilItemCreaterBuildable::DestroyItems()
{
	if (GetStorageInventory())
	{
		FInventoryStack Stack;

		GetStorageInventory()->GetStackFromIndex(2, Stack);
		if (Stack.HasItems())
		{
			GetStorageInventory()->RemoveAllFromIndex(2);
		}

		GetStorageInventory()->GetStackFromIndex(3, Stack);
		if (Stack.HasItems())
		{
			GetStorageInventory()->RemoveAllFromIndex(3);
		}
	}
}

void AKBFLUtilItemCreaterBuildable::CreateItems()
{
	if (GetStorageInventory())
	{
		if (mBeltItemClassToGenerate)
		{
			UKBFLCppInventoryHelper::StoreItemAmountInInventory(GetStorageInventory(), 0, mBeltItemClassToGenerate);
		}
		if (mPipeItemClassToGenerate)
		{
			UKBFLCppInventoryHelper::StoreItemAmountInInventory(GetStorageInventory(), 1, mPipeItemClassToGenerate,
																200);
		}
	}
}

void AKBFLUtilItemCreaterBuildable::SetBeltItem(TSubclassOf<UFGItemDescriptor> Item)
{
	if (!HasAuthority())
	{
		if (AFGPlayerController* PlayerController = Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (UKBFLDefaultRCO* RCO = PlayerController->GetRemoteCallObjectOfClass<UKBFLDefaultRCO>())
			{
				RCO->Server_CheatBuilding_SetBeltItem(this, Item);
			}
		}
		return;
	}

	mBeltItemClassToGenerate = Item;
	mPropertyReplicator.MarkPropertyDirty(FName("mBeltItemClassToGenerate"));

	FInventoryStack Stack;
	GetStorageInventory()->GetStackFromIndex(0, Stack);
	if (Stack.HasItems())
	{
		GetStorageInventory()->RemoveAllFromIndex(0);
	}
}

void AKBFLUtilItemCreaterBuildable::SetPipeItem(TSubclassOf<UFGItemDescriptor> Item)
{
	if (!HasAuthority())
	{
		if (AFGPlayerController* PlayerController = Cast<AFGPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			if (UKBFLDefaultRCO* RCO = PlayerController->GetRemoteCallObjectOfClass<UKBFLDefaultRCO>())
			{
				RCO->Server_CheatBuilding_SetPipeItem(this, Item);
			}
		}
		return;
	}

	mPipeItemClassToGenerate = Item;
	mPropertyReplicator.MarkPropertyDirty(FName("mPipeItemClassToGenerate"));

	FInventoryStack Stack;
	GetStorageInventory()->GetStackFromIndex(1, Stack);
	if (Stack.HasItems())
	{
		GetStorageInventory()->RemoveAllFromIndex(1);
	}
}

void AKBFLUtilItemCreaterBuildable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AKBFLUtilItemCreaterBuildable::GetConditionalReplicatedProps(TArray<FFGCondReplicatedProperty>& outProps) const
{
	Super::GetConditionalReplicatedProps(outProps);

	FG_DOREPCONDITIONAL(ThisClass, mBeltItemClassToGenerate);
	FG_DOREPCONDITIONAL(ThisClass, mPipeItemClassToGenerate);
}

void UKBFLDefaultRCO::Server_AddStackToCharacter_Implementation(AFGCharacterPlayer* Player,
																TSubclassOf<UFGItemDescriptor> ItemToAdd)
{
	if (IsValid(Player) && Player == GetRequestingCharacter(this) && HasDebugPermission(this) && IsValid(ItemToAdd))
	{
		FInventoryStack Stack = FInventoryStack(UFGItemDescriptor::GetStackSize(ItemToAdd), ItemToAdd);
		if (Player->GetInventory())
		{
			Player->GetInventory()->AddStack(Stack, true);
		}
		Player->ForceNetUpdate();
	}
}

void UKBFLDefaultRCO::Server_CheatBuilding_SetBeltItem_Implementation(AKBFLUtilItemCreaterBuildable* Building,
																	  TSubclassOf<UFGItemDescriptor> ItemToSet)
{
	if (CanConfigureDebugBuilding(this, Building) && IsValid(ItemToSet))
	{
		Building->SetBeltItem(ItemToSet);
		Building->ForceNetUpdate();
	}
}

void UKBFLDefaultRCO::Server_CheatBuilding_SetPipeItem_Implementation(AKBFLUtilItemCreaterBuildable* Building,
																	  TSubclassOf<UFGItemDescriptor> ItemToSet)
{
	if (CanConfigureDebugBuilding(this, Building) && IsValid(ItemToSet))
	{
		Building->SetPipeItem(ItemToSet);
		Building->ForceNetUpdate();
	}
}

void UKBFLDefaultRCO::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UKBFLDefaultRCO, bDummy);
}

void UKBFLDefaultRCO::Server_ClearCharacterInventory_Implementation(AFGCharacterPlayer* Player)
{
	if (IsValid(Player) && Player == GetRequestingCharacter(this) && HasDebugPermission(this))
	{
		if (Player->GetInventory())
		{
			Player->GetInventory()->Empty();
			Player->ForceNetUpdate();
		}
	}
}
