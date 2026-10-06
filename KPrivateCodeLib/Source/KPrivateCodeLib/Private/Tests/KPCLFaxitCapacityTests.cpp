// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Description/KPCLItemDescriptor.h"
#include "Misc/AutomationTest.h"
#include "Subsystem/KPCLFaxitSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKPCLFaxitCapacityStockTest, "KPrivateCodeLib.Faxit.CapacityPreservesStock",
								EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKPCLFaxitCapacityStockTest::RunTest(const FString& Parameters)
{
	const TSubclassOf<UFGItemDescriptor> Item = UKPCLItemDescriptor::StaticClass();
	const int32 StackSize = UFGItemDescriptor::GetStackSize(Item);
	if (!TestTrue(TEXT("Native descriptor has a positive stack size"), StackSize > 0))
	{
		return false;
	}

	FKPCLMappedItemAmount Storage;
	Storage.mMaxUniqueItems = 1;
	Storage.mStackMultiplier = 2;
	TestEqual(TEXT("Initial stock is accepted"), Storage.AddItemAmount(Item, StackSize * 2), StackSize * 2);

	Storage.mMaxUniqueItems = 0;
	Storage.mStackMultiplier = 1;
	Storage.CleanUpEmptySlots({});
	bool bHasItem = false;
	TestEqual(TEXT("Removing unique capacity preserves stock"), Storage.GetAmountConst(Item, bHasItem).Amount,
			  StackSize * 2);
	TestTrue(TEXT("Over-capacity stock remains accessible"), bHasItem);
	TestEqual(TEXT("Over-capacity additions are rejected"), Storage.AddItemAmount(Item, 1), 0);
	TestEqual(TEXT("Rejected addition preserves stock"), Storage.GetAmountConst(Item, bHasItem).Amount, StackSize * 2);
	TestEqual(TEXT("Stock can still be removed"), Storage.RemoveItemAmount(Item, StackSize * 2), StackSize * 2);
	TestEqual(TEXT("Empty slot is released"), Storage.GetSlotSize(), 0);
	TestFalse(TEXT("New item slots remain blocked without capacity"), Storage.CanAddNewItemSlot(Item));
	return true;
}

#endif
