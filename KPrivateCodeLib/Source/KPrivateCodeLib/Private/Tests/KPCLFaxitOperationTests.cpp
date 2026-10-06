// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Description/KPCLItemDescriptor.h"
#include "Misc/AutomationTest.h"
#include "Subsystem/KPCLFaxitSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKPCLFaxitInvalidOperationsTest, "KPrivateCodeLib.Faxit.InvalidOperationsPreserveStock",
								EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKPCLFaxitInvalidOperationsTest::RunTest(const FString& Parameters)
{
	const TSubclassOf<UFGItemDescriptor> Item = UKPCLItemDescriptor::StaticClass();
	FKPCLMappedItemAmount Storage;
	Storage.mMaxUniqueItems = 1;
	Storage.mStackMultiplier = 1;
	TestEqual(TEXT("Zero addition is rejected"), Storage.AddItemAmount(Item, 0), 0);
	TestEqual(TEXT("Negative addition is rejected"), Storage.AddItemAmount(Item, -1), 0);
	TestEqual(TEXT("Rejected additions do not allocate slots"), Storage.GetSlotSize(), 0);

	bool bHasItem = false;
	TestEqual(TEXT("Missing lookup returns zero"), Storage.GetAmountConst(Item, bHasItem).Amount, 0);
	TestFalse(TEXT("Missing lookup reports no item"), bHasItem);
	TestEqual(TEXT("Missing lookup does not allocate a slot"), Storage.GetSlotSize(), 0);
	TestEqual(TEXT("Initial stock is accepted"), Storage.AddItemAmount(Item, 1), 1);
	TestEqual(TEXT("Zero removal is rejected"), Storage.RemoveItemAmount(Item, 0), 0);
	TestEqual(TEXT("Negative removal is rejected"), Storage.RemoveItemAmount(Item, -1), 0);
	TestEqual(TEXT("Item-amount overload rejects negative removal"), Storage.RemoveItemAmount(FItemAmount(Item, -1)), 0);
	TestEqual(TEXT("Invalid removals preserve stock"), Storage.GetAmountConst(Item, bHasItem).Amount, 1);
	TestEqual(TEXT("Excess removal is capped at stock"), Storage.RemoveItemAmount(Item, 2), 1);

	Storage.mMaxUniqueItems = 0;
	TestEqual(TEXT("Addition without unique capacity is rejected"), Storage.AddItemAmount(Item, 1), 0);
	TestEqual(TEXT("Capacity rejection does not allocate a slot"), Storage.GetSlotSize(), 0);
	return true;
}

#endif
