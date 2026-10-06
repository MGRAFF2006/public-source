// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Curves/CurveFloat.h"
#include "Misc/AutomationTest.h"
#include "Reflection/KDFVanillaCache.h"
#include "UObject/UObjectGlobals.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKDFSnapshotAliasTest, "KDataForge.Reflection.SnapshotAliases",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKDFSnapshotAliasTest::RunTest(const FString& Parameters)
{
	UCurveFloat* Curve = NewObject<UCurveFloat>();
	FKDFVanillaCache Cache;
	Cache.RecordSnapshot(Curve, TEXT("floatcurve"), TEXT("baseline"));
	Cache.RecordSnapshot(Curve, TEXT("FloatCurve"), TEXT("later"));
	TestTrue(TEXT("Exporter finds snapshots recorded using an alias"), Cache.HasSnapshotUnderProperty(Curve, TEXT("FloatCurve")));
	const FString* Snapshot = Cache.FindSnapshot(Curve, TEXT("FLOATCURVE"));
	TestNotNull(TEXT("Case-insensitive lookup finds the baseline"), Snapshot);
	if (Snapshot != nullptr)
	{
		TestEqual(TEXT("Aliases do not replace the initial baseline"), *Snapshot, FString(TEXT("baseline")));
	}
	return true;
}

#endif
