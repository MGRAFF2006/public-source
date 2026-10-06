// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Structures/KPCLFunctionalStructure.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKPCLPowerPhaseTest, "KPrivateCodeLib.Power.VariablePhase",
								EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKPCLPowerPhaseTest::RunTest(const FString& Parameters)
{
	FPowerOptions Power;
	Power.mVariablePowerTime = 30.f;
	Power.StructureTick(95.f, true);
	TestEqual(TEXT("Long ticks wrap every completed period"), Power.mVariableTimer, 5.f);
	TestTrue(TEXT("Phase remains inside its cycle"), FMath::IsNearlyEqual(Power.mCurrentPowerCurvePercent, 1.f / 6.f));
	Power.StructureTick(25.f, true);
	TestEqual(TEXT("Exact cycle boundary wraps to zero"), Power.mVariableTimer, 0.f);
	Power.StructureTick(3.f, false);
	TestEqual(TEXT("Stopped power does not advance phase"), Power.mVariableTimer, 0.f);
	Power.StructureTick(-1.f, true);
	TestEqual(TEXT("Negative elapsed time does not rewind phase"), Power.mVariableTimer, 0.f);

	Power.mVariablePowerTime = 0.f;
	Power.StructureTick(1.f, true);
	TestEqual(TEXT("Zero period produces a safe phase"), Power.mCurrentPowerCurvePercent, 0.f);
	Power.mVariablePowerTime = -1.f;
	Power.StructureTick(1.f, true);
	TestEqual(TEXT("Negative period produces a safe phase"), Power.mCurrentPowerCurvePercent, 0.f);
	return true;
}

#endif
