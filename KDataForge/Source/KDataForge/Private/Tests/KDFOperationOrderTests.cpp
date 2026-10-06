// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Subsystems/KDFSubsystem.h"
#include "UObject/UObjectGlobals.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKDFOperationOrderTest, "KDataForge.Reporting.ApplicationOrder",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKDFOperationOrderTest::RunTest(const FString& Parameters)
{
	UKDFSubsystem* Subsystem = NewObject<UKDFSubsystem>();
	const auto RecordOperation = [Subsystem](const FString& File)
	{
		FKDFPatchRecord Record;
		Record.mPackRef = TEXT("Pack");
		Record.mSourceFile = File;
		Record.mRootType = TEXT("cdo");
		Record.mOps.AddDefaulted();
		Subsystem->RecordDeferredPatchResult(MoveTemp(Record), {});
	};
	RecordOperation(TEXT("A.cdo.yml"));
	RecordOperation(TEXT("B.cdo.yml"));
	RecordOperation(TEXT("A.cdo.yml"));
	const TArray<FKDFPatchRecord>& Records = Subsystem->GetPatchRecords();
	TestEqual(TEXT("Deferred operations retain the document count"), Records.Num(), 2);
	TestEqual(TEXT("The initial operation is first"), Records[0].mOps[0].mApplicationOrder, int64(0));
	TestEqual(TEXT("An intervening document remains before the later deferred write"), Records[1].mOps[0].mApplicationOrder, int64(1));
	TestEqual(TEXT("The later operation has the final sequence despite record grouping"), Records[0].mOps[1].mApplicationOrder, int64(2));
	return true;
}

#endif
