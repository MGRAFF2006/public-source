// Copyright Kyri123 / KMods 2026. All Rights Reserved.


#include "Replication/KPCLDefaultRCO.h"

#include "Buildable/Conveyor/KPCLBuildableBalanceSplitter.h"
#include "Buildable/KPCLExtractorBase.h"
#include "Buildable/KPCLPressureRegulatorValve.h"
#include "Buildable/KPCLProducerBase.h"
#include "FGCharacterPlayer.h"
#include "FGGameSession.h"
#include "FGPlayerController.h"
#include "GameFramework/GameMode.h"
#include "Looting/KPCLLootChest.h"
#include "Net/UnrealNetwork.h"
#include "Network/Buildings/KPCLNetworkConnectionBuilding.h"
#include "Network/Buildings/KPCLNetworkCore.h"
#include "Network/KPCLNetworkBuildingBase.h"
#include "Subsystem/KPCLDeliveryTaskSubsystem.h"
#include "Subsystem/KPCLFaxitSubsystem.h"

namespace
{
	AFGPlayerController* GetRequestingController(const UKPCLDefaultRCO* RCO)
	{
		AFGPlayerController* Controller = IsValid(RCO) ? RCO->GetOwnerPlayerController() : nullptr;
		return IsValid(Controller) && Controller->HasAuthority() && IsValid(Controller->GetWorld()) &&
			Controller->GetWorld() == RCO->GetWorld()
			? Controller
			: nullptr;
	}

	AFGCharacterPlayer* GetRequestingPlayer(const UKPCLDefaultRCO* RCO)
	{
		AFGPlayerController* Controller = GetRequestingController(RCO);
		AFGCharacterPlayer* Player = IsValid(Controller) ? Cast<AFGCharacterPlayer>(Controller->GetControlledCharacter()) : nullptr;
		return IsValid(Player) && Player->GetWorld() == RCO->GetWorld() ? Player : nullptr;
	}

	bool CanInteractWithBuilding(const UKPCLDefaultRCO* RCO, const AFGBuildable* Building)
	{
		AFGCharacterPlayer* Player = GetRequestingPlayer(RCO);
		return IsValid(Player) && IsValid(Building) && Building->HasAuthority() &&
			Building->GetWorld() == RCO->GetWorld() && Building->GetInteractingPlayers().Contains(Player);
	}

	bool CanAccessInventory(const UKPCLDefaultRCO* RCO, const UFGInventoryComponent* Inventory)
	{
		AFGCharacterPlayer* Player = GetRequestingPlayer(RCO);
		if (!IsValid(Player) || !IsValid(Inventory) || Inventory->GetWorld() != RCO->GetWorld())
		{
			return false;
		}
		AActor* Owner = Inventory->GetOwner();
		AKPCLLootChest* Chest = Cast<AKPCLLootChest>(Owner);
		return Owner == Player || CanInteractWithBuilding(RCO, Cast<AFGBuildable>(Owner)) ||
			(IsValid(Chest) && Chest->GetInventory() == Inventory && Chest->IsPlayerInteracting(Player));
	}

	bool CanUseDebug(const UKPCLDefaultRCO* RCO)
	{
		AFGPlayerController* Controller = GetRequestingController(RCO);
		if (!IsValid(Controller))
		{
			return false;
		}
		AGameMode* GameMode = Controller->GetWorld()->GetAuthGameMode<AGameMode>();
		AFGGameSession* Session = IsValid(GameMode) ? Cast<AFGGameSession>(GameMode->GameSession) : nullptr;
		return Controller->IsLocalController() || (IsValid(Session) && Session->IsPlayerAdmin(Controller));
	}

	bool IsCanonicalOutlineTarget(const UKPCLDefaultRCO* RCO, const AKPCLOutlineSubsystem* Target)
	{
		return CanUseDebug(RCO) && IsValid(Target) && Target == AKPCLOutlineSubsystem::Get(RCO->GetWorld());
	}

	bool IsCanonicalSwatchTarget(const UKPCLDefaultRCO* RCO, const AKPCLSwatchSystem* Target)
	{
		return IsValid(GetRequestingPlayer(RCO)) && IsValid(Target) && Target == AKPCLSwatchSystem::Get(RCO->GetWorld());
	}
}

void UKPCLDefaultRCO::Server_CreateOutlineForActor_Implementation(AKPCLOutlineSubsystem* Subsystem, FOutlineData Data)
{
	if (IsCanonicalOutlineTarget(this, Subsystem) && IsValid(Data.mActorToOutline) &&
		Data.mActorToOutline->GetWorld() == GetWorld())
	{
		Subsystem->MultiCast_CreateOutlineForActor(Data);
		Subsystem->ForceNetUpdate();
	}
}

void UKPCLDefaultRCO::Server_ClearOutlines_Implementation(AKPCLOutlineSubsystem* Subsystem)
{
	if (IsCanonicalOutlineTarget(this, Subsystem))
	{
		Subsystem->MultiCast_ClearOutlines();
		Subsystem->ForceNetUpdate();
	}
}

void UKPCLDefaultRCO::Server_SetOutlineColor_Implementation(AKPCLOutlineSubsystem* Subsystem, FLinearColor Color,
															EOutlineColorSlot ColorSlot)
{
	if (IsCanonicalOutlineTarget(this, Subsystem))
	{
		Subsystem->MultiCast_SetOutlineColor(Color, ColorSlot);
		Subsystem->ForceNetUpdate();
	}
}

void UKPCLDefaultRCO::Server_ClearOutlineForActor_Implementation(AKPCLOutlineSubsystem* Subsystem, AActor* Actor)
{
	if (IsCanonicalOutlineTarget(this, Subsystem) && IsValid(Actor) && Actor->GetWorld() == GetWorld())
	{
		Subsystem->MultiCast_ClearOutlinesForActor(Actor);
		Subsystem->ForceNetUpdate();
	}
}

void UKPCLDefaultRCO::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UKPCLDefaultRCO, mDummy);
}

void UKPCLDefaultRCO::Server_FlushFluids_Implementation(AFGBuildable* Building)
{
	if (!CanInteractWithBuilding(this, Building))
	{
		return;
	}
	if (AKPCLProducerBase* Producer = Cast<AKPCLProducerBase>(Building))
	{
		Producer->FlushFluids();
		Producer->ForceNetUpdate();
		return;
	}

	if (AKPCLExtractorBase* Extractor = Cast<AKPCLExtractorBase>(Building))
	{
		Extractor->FlushFluids();
		Extractor->ForceNetUpdate();
	}
}

void UKPCLDefaultRCO::Server_LootChest_Implementation(AKPCLLootChest* Target, AFGCharacterPlayer* Player)
{
	AFGCharacterPlayer* RequestingPlayer = GetRequestingPlayer(this);
	if (IsValid(Target) && Target->GetWorld() == GetWorld() && IsValid(RequestingPlayer) && Player == RequestingPlayer &&
		Target->HasAuthority() && Target->IsPlayerInteracting(RequestingPlayer))
	{
		Target->Loot(RequestingPlayer);
	}
}

int32 UKPCLDefaultRCO::MoveItemAmount(UFGInventoryComponent* Source, int32 SourceIndex, UFGInventoryComponent* Target,
									  FItemAmount Amount, bool ResizeToFit)
{
	if (ensure(Source && Target && Amount.ItemClass && Amount.Amount > 0))
	{
		FInventoryStack Stack;
		if (Source->GetStackFromIndex(SourceIndex, Stack))
		{
			if (Stack.NumItems >= Amount.Amount || ResizeToFit)
			{
				if (ResizeToFit && Stack.NumItems < Amount.Amount)
				{
					Amount.Amount = Stack.NumItems;
				}

				int32 GrabedAmount = Target->AddStack(FInventoryStack(Amount.Amount, Amount.ItemClass));

				if (GrabedAmount > 0)
				{
					Source->RemoveFromIndex(SourceIndex, GrabedAmount);
				}

				return GrabedAmount;
			}
		}
	}
	return 0;
}

void UKPCLDefaultRCO::Server_MoveItemAmount_Implementation(UFGInventoryComponent* Source, int32 SourceIndex,
														   UFGInventoryComponent* Target, FItemAmount Amount,
														   bool ResizeToFit)
{
	if (CanAccessInventory(this, Source) && CanAccessInventory(this, Target))
	{
		MoveItemAmount(Source, SourceIndex, Target, Amount, ResizeToFit);
	}
}

void UKPCLDefaultRCO::Server_UpdateCustomSwatchData_Implementation(AKPCLSwatchSystem* Target, FCustomSwatchData Data,
																   int32 Idx)
{
	if (IsCanonicalSwatchTarget(this, Target))
	{
		Target->UpdateCustomSwatchData(Data, Idx);
		Target->ForceNetUpdate();
	}
}

void UKPCLDefaultRCO::Server_AddCustomSwatchData_Implementation(AKPCLSwatchSystem* Target, FCustomSwatchData Data)
{
	if (IsCanonicalSwatchTarget(this, Target))
	{
		Target->AddCustomSwatchData(Data);
		Target->ForceNetUpdate();
	}
}

void UKPCLDefaultRCO::Server_RemoveCustomSwatchData_Implementation(AKPCLSwatchSystem* Target, int32 Idx)
{
	if (IsCanonicalSwatchTarget(this, Target))
	{
		Target->RemoveCustomSwatchData(Idx);
		Target->ForceNetUpdate();
	}
}

namespace
{
	bool IsCanonicalDeliveryTaskTarget(const UKPCLDefaultRCO* RCO, const AKPCLDeliveryTaskSubsystem* Target)
	{
		return IsValid(GetRequestingPlayer(RCO)) && IsValid(Target) &&
			Target == AKPCLDeliveryTaskSubsystem::Get(RCO->GetWorld());
	}

	bool IsQueuedTaskAt(const UKPCLDefaultRCO* RCO, AKPCLDeliveryTaskSubsystem* Target, const UKAPIDeliveryTask* Task,
						int32 QueueIndex)
	{
		if (!IsCanonicalDeliveryTaskTarget(RCO, Target) || !IsValid(Task))
		{
			return false;
		}

		const TArray<UKAPIDeliveryTask*> QueuedTasks = Target->GetQueuedTasks();
		return QueuedTasks.IsValidIndex(QueueIndex) && QueuedTasks[QueueIndex] == Task;
	}
}

bool UKPCLDefaultRCO::Server_DeliveryTask_TryToQueue_Validate(AKPCLDeliveryTaskSubsystem* Target,
															  UKAPIDeliveryTask* Task)
{
	return IsCanonicalDeliveryTaskTarget(this, Target);
}

bool UKPCLDefaultRCO::Server_DeliveryTask_TryGrabCurrentTaskItems_Validate(AKPCLDeliveryTaskSubsystem* Target)
{
	return IsCanonicalDeliveryTaskTarget(this, Target);
}

bool UKPCLDefaultRCO::Server_DeliveryTask_TryToRemoveFromQueue_Validate(AKPCLDeliveryTaskSubsystem* Target,
																		UKAPIDeliveryTask* Task)
{
	return IsCanonicalDeliveryTaskTarget(this, Target);
}

bool UKPCLDefaultRCO::Server_DeliveryTask_TryToRemoveFromQueueAt_Validate(AKPCLDeliveryTaskSubsystem* Target,
																		  UKAPIDeliveryTask* Task, int32 QueueIndex)
{
	return IsCanonicalDeliveryTaskTarget(this, Target) && QueueIndex >= 0;
}

bool UKPCLDefaultRCO::Server_DeliveryTask_TryToDequeueFrom_Validate(AKPCLDeliveryTaskSubsystem* Target,
																	UKAPIDeliveryTask* Task, int32 QueueIndex)
{
	return IsCanonicalDeliveryTaskTarget(this, Target) && QueueIndex >= 0;
}

bool UKPCLDefaultRCO::Server_DeliveryTask_TryToMoveQueueEntry_Validate(AKPCLDeliveryTaskSubsystem* Target,
																	   UKAPIDeliveryTask* Task, int32 FromIndex,
																	   int32 ToIndex)
{
	return IsCanonicalDeliveryTaskTarget(this, Target) && FromIndex >= 0 && ToIndex >= 0;
}

void UKPCLDefaultRCO::Server_DeliveryTask_TryToQueue_Implementation(AKPCLDeliveryTaskSubsystem* Target,
																	UKAPIDeliveryTask* Task)
{
	if (IsCanonicalDeliveryTaskTarget(this, Target) && IsValid(Task))
	{
		TArray<FText> ErrorMessages;
		Target->TryToQueueTask(Task, ErrorMessages);
	}
}

void UKPCLDefaultRCO::Server_DeliveryTask_TryGrabCurrentTaskItems_Implementation(
	AKPCLDeliveryTaskSubsystem* Target)
{
	AFGCharacterPlayer* Player = GetRequestingPlayer(this);
	if (IsCanonicalDeliveryTaskTarget(this, Target) && IsValid(Player))
	{
		Target->TryGrabCurrentTaskItemsFromPlayer(Player);
	}
}

void UKPCLDefaultRCO::Server_DeliveryTask_TryToRemoveFromQueue_Implementation(AKPCLDeliveryTaskSubsystem* Target,
																			  UKAPIDeliveryTask* Task)
{
	if (IsCanonicalDeliveryTaskTarget(this, Target) && IsValid(Task))
	{
		Target->TryToRemoveFromQueue(Task);
	}
}

void UKPCLDefaultRCO::Server_DeliveryTask_TryToRemoveFromQueueAt_Implementation(AKPCLDeliveryTaskSubsystem* Target,
																				UKAPIDeliveryTask* Task,
																				int32 QueueIndex)
{
	if (IsQueuedTaskAt(this, Target, Task, QueueIndex))
	{
		Target->TryToRemoveFromQueueAt(QueueIndex);
	}
}

void UKPCLDefaultRCO::Server_DeliveryTask_TryToDequeueFrom_Implementation(AKPCLDeliveryTaskSubsystem* Target,
																		  UKAPIDeliveryTask* Task, int32 QueueIndex)
{
	if (IsQueuedTaskAt(this, Target, Task, QueueIndex))
	{
		Target->TryToDequeueFrom(QueueIndex);
	}
}

void UKPCLDefaultRCO::Server_DeliveryTask_TryToMoveQueueEntry_Implementation(AKPCLDeliveryTaskSubsystem* Target,
																			 UKAPIDeliveryTask* Task, int32 FromIndex,
																			 int32 ToIndex)
{
	if (IsQueuedTaskAt(this, Target, Task, FromIndex) && Target->GetQueuedTasks().IsValidIndex(ToIndex))
	{
		Target->TryToMoveQueueEntry(FromIndex, ToIndex);
	}
}

void UKPCLDefaultRCO::Server_Delivery_SetMinimumStoragePercentage_Implementation(AKPCLNetworkCore* Target,
																				 float NewPercentage)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetMinimumStoragePercentage(NewPercentage);
	}
}

void UKPCLDefaultRCO::Server_Faxit_GrabFromNetwork_Implementation(class AKPCLNetworkCore* Target,
																  AFGCharacterPlayer* Player, FItemAmount Amount)
{
	if (CanInteractWithBuilding(this, Target))
	{
		AFGCharacterPlayer* RequestingPlayer = GetRequestingPlayer(this);
		if (Player == RequestingPlayer)
		{
			Target->GrabFromNetwork(RequestingPlayer, Amount);
		}
	}
}

void UKPCLDefaultRCO::Server_Faxit_StorageFromPlayerToNetwork_Implementation(class AKPCLNetworkCore* Target,
																			 AFGCharacterPlayer* Player,
																			 FItemAmount Amount)
{
	if (CanInteractWithBuilding(this, Target))
	{
		AFGCharacterPlayer* RequestingPlayer = GetRequestingPlayer(this);
		if (Player == RequestingPlayer)
		{
			Target->StorageFromPlayerToNetwork(RequestingPlayer, Amount);
		}
	}
}

void UKPCLDefaultRCO::Server_Faxit_UpdateNetworkName_Implementation(class AKPCLFaxitSubsystem* Target,
																	class AKPCLNetworkCore* Core,
																	const FString& NewName)
{
	if (IsValid(Target) && Target == AKPCLFaxitSubsystem::Get(GetWorld()) &&
		CanInteractWithBuilding(this, Core))
	{
		Target->UpdateNetworkName(Core, NewName);
	}
}

namespace
{
	bool CanProcessFaxitAddBuildingToCore(const UKPCLDefaultRCO* RCO, AKPCLNetworkBuildingBase* Building,
										 AKPCLNetworkCore* Core)
	{
		if (!IsValid(RCO) || !IsValid(Building) || !IsValid(Core) || Building == Core ||
			Building->GetWorld() != RCO->GetWorld() || Core->GetWorld() != RCO->GetWorld())
		{
			return false;
		}

		AFGCharacterPlayer* Player = GetRequestingPlayer(RCO);
		if (!IsValid(Player) || !CanInteractWithBuilding(RCO, Building) || !Core->HasAuthority())
		{
			return false;
		}

		AKPCLFaxitSubsystem* FaxitSubsystem = AKPCLFaxitSubsystem::Get(RCO->GetWorld());
		if (!IsValid(FaxitSubsystem))
		{
			return false;
		}

		FKPCLFaxitNetwork Network;
		if (!FaxitSubsystem->GetNetworkByCore(Core, Network) || !Network.mIsValid || Network.mCore != Core)
		{
			return false;
		}

		return Network.IsActorInNetwork(Building) || FaxitSubsystem->CanAddToNetwork(Network.mNetworkId);
	}
}

bool UKPCLDefaultRCO::Server_Faxit_AddBuildingToCore_Validate(AKPCLNetworkBuildingBase*, AKPCLNetworkCore*)
{

	return true;
}

void UKPCLDefaultRCO::Server_Faxit_AddBuildingToCore_Implementation(AKPCLNetworkBuildingBase* Building,
																	AKPCLNetworkCore* Core)
{
	AKPCLFaxitSubsystem* FaxitSubsystem = AKPCLFaxitSubsystem::Get(this);
	if (IsValid(FaxitSubsystem) && CanProcessFaxitAddBuildingToCore(this, Building, Core))
	{
		FaxitSubsystem->AddBuildingToCore(Building, Core);
	}
}

void UKPCLDefaultRCO::Server_Faxit_SetSpeedOverride_Implementation(class AKPCLNetworkConnectionBuilding* Target,
																   float Value)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetSpeedOverride(Value);
	}
}

void UKPCLDefaultRCO::Server_Faxit_ClearSpeedOverride_Implementation(class AKPCLNetworkConnectionBuilding* Target)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->ClearSpeedOverride();
	}
}

void UKPCLDefaultRCO::Server_Faxit_SetFilterItem_Implementation(class AKPCLNetworkConnectionBuilding* Target,
																TSubclassOf<UFGItemDescriptor> NewItem)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetFilterItem(NewItem);
	}
}

void UKPCLDefaultRCO::Server_Valve_SetOnThreshold_Implementation(AKPCLPressureRegulatorValve* Target,
																 float NewThreshold)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetOnThreshold(NewThreshold);
	}
}

void UKPCLDefaultRCO::Server_Valve_SetOffThreshold_Implementation(AKPCLPressureRegulatorValve* Target,
																  float NewThreshold)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetOffThreshold(NewThreshold);
	}
}

void UKPCLDefaultRCO::Server_Valve_SetThresholds_Implementation(AKPCLPressureRegulatorValve* Target,
																float NewOnThreshold, float NewOffThreshold)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetThresholds(NewOnThreshold, NewOffThreshold);
	}
}

void UKPCLDefaultRCO::Server_Splitter_SetFilteredItems_Implementation(
	AKPCLBuildableBalanceSplitter* Target, int32 Idx, const TArray<TSubclassOf<UFGItemDescriptor>>& Items)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetFilteredItems(Idx, Items);
	}
}

void UKPCLDefaultRCO::Server_Splitter_SetItemsPerMin_Implementation(AKPCLBuildableBalanceSplitter* Target, int32 Idx,
																	float ItemsPerMin)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->SetItemsPerMin(Idx, ItemsPerMin);
	}
}

void UKPCLDefaultRCO::Server_Splitter_RemoveFromFilter_Implementation(AKPCLBuildableBalanceSplitter* Target, int32 Idx,
																	  TSubclassOf<UFGItemDescriptor> Item)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->RemoveFromFilter(Idx, Item);
	}
}

void UKPCLDefaultRCO::Server_Splitter_AddOrSetFilter_Implementation(AKPCLBuildableBalanceSplitter* Target, int32 Idx,
																	TSubclassOf<UFGItemDescriptor> Item)
{
	if (CanInteractWithBuilding(this, Target))
	{
		Target->AddOrSetFilter(Idx, Item);
	}
}
