// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Engine/GameInstance.h"
#include "Misc/AutomationTest.h"
#include "Subsystems/KDFSubsystem.h"
#include "UObject/UObjectGlobals.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKDFIncludeOrderTest, "KDataForge.Loader.IncludeOrder",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKDFIncludeOrderTest::RunTest(const FString& Parameters)
{
	UKDFSubsystem* Subsystem = NewObject<UKDFSubsystem>(NewObject<UGameInstance>());
	const auto AddDocument = [Subsystem](const FString& File, const TArray<FString>& Includes, int32 PackOrder)
	{
		FKDFDocument& Document = Subsystem->mDocuments.AddDefaulted_GetRef();
		Document.mRelativeFile = File;
		Document.mIncludeDependencies = Includes;
		Document.mPackRef = FString::Printf(TEXT("Pack%d"), PackOrder);
		Document.mPackOrder = PackOrder;
	};
	AddDocument(TEXT("A.cdo.yml"), {TEXT("C.cdo.yml")}, 0);
	AddDocument(TEXT("B.cdo.yml"), {TEXT("C.cdo.yml"), TEXT("A.cdo.yml")}, 0);
	AddDocument(TEXT("C.cdo.yml"), {}, 0);
	AddDocument(TEXT("A.cdo.yml"), {}, 1);
	TArray<int32> Order = {0, 1, 2, 3};
	Subsystem->SortStageDocuments(Order);
	TestEqual(TEXT("The prerequisite is first"), Order[0], 2);
	TestEqual(TEXT("Its dependant follows"), Order[1], 0);
	TestEqual(TEXT("Multiple prerequisites are satisfied"), Order[2], 1);
	TestEqual(TEXT("Later packs keep their ordering"), Order[3], 3);

	Subsystem->mDocuments[0].mIncludeDependencies.Reset();
	Subsystem->mDocuments[1].mIncludeDependencies.Reset();
	Order = {2, 1, 0, 3};
	Subsystem->SortStageDocuments(Order);
	TestEqual(TEXT("Independent files retain alphabetical order"), Order[0], 0);
	TestEqual(TEXT("Independent files retain alphabetical order"), Order[1], 1);
	TestEqual(TEXT("Independent files retain alphabetical order"), Order[2], 2);
	return true;
}

#endif
