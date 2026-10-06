// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Dom/JsonObject.h"
#include "HttpLib/KPCLJsonObject.h"
#include "Misc/AutomationTest.h"

#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKPCLJsonNumericTest, "KPrivateCodeLib.Json.NumericReads",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKPCLJsonNumericTest::RunTest(const FString& Parameters)
{
	const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
	UKPCLJsonObject* Object = UKPCLJsonObject::CreateFromJson(Json);
	int Integer = 17;
	float Float = 3.f;
	TestFalse(TEXT("A missing integer fails"), Object->TryGetInt(TEXT("missing"), Integer));
	TestEqual(TEXT("A failed integer read preserves output"), Integer, 17);
	TestFalse(TEXT("A missing float fails"), Object->TryGetFloat(TEXT("missing"), Float));
	TestEqual(TEXT("A failed float read preserves output"), Float, 3.f);
	Json->SetStringField(TEXT("wrong"), TEXT("12"));
	TestFalse(TEXT("Strings are not integers"), Object->TryGetInt(TEXT("wrong"), Integer));
	TestFalse(TEXT("Strings are not floats"), Object->TryGetFloat(TEXT("wrong"), Float));
	Json->SetNumberField(TEXT("large"), 1.e100);
	TestFalse(TEXT("Integer overflow is rejected"), Object->TryGetInt(TEXT("large"), Integer));
	TestFalse(TEXT("Float overflow is rejected"), Object->TryGetFloat(TEXT("large"), Float));
	TestEqual(TEXT("Overflow preserves integer output"), Integer, 17);
	TestEqual(TEXT("Overflow preserves float output"), Float, 3.f);
	Json->SetNumberField(TEXT("nonfinite"), std::numeric_limits<double>::quiet_NaN());
	TestFalse(TEXT("NaN is not an integer"), Object->TryGetInt(TEXT("nonfinite"), Integer));
	TestFalse(TEXT("NaN is not a float"), Object->TryGetFloat(TEXT("nonfinite"), Float));
	Json->SetNumberField(TEXT("nonfinite"), std::numeric_limits<double>::infinity());
	TestFalse(TEXT("Infinity is not an integer"), Object->TryGetInt(TEXT("nonfinite"), Integer));
	TestFalse(TEXT("Infinity is not a float"), Object->TryGetFloat(TEXT("nonfinite"), Float));
	TestEqual(TEXT("Nonfinite numbers preserve integer output"), Integer, 17);
	TestEqual(TEXT("Nonfinite numbers preserve float output"), Float, 3.f);
	Json->SetNumberField(TEXT("max"), MAX_int32);
	TestTrue(TEXT("Maximum integer succeeds"), Object->TryGetInt(TEXT("max"), Integer));
	TestEqual(TEXT("Maximum integer remains exact"), Integer, MAX_int32);
	Json->SetNumberField(TEXT("min"), MIN_int32);
	TestTrue(TEXT("Minimum integer succeeds"), Object->TryGetInt(TEXT("min"), Integer));
	TestEqual(TEXT("Minimum integer remains exact"), Integer, MIN_int32);
	Json->SetNumberField(TEXT("fraction"), 1.5);
	TestTrue(TEXT("Existing fractional truncation remains supported"), Object->TryGetInt(TEXT("fraction"), Integer));
	TestEqual(TEXT("Fractional integer conversion truncates"), Integer, 1);
	TestTrue(TEXT("A valid float succeeds"), Object->TryGetFloat(TEXT("fraction"), Float));
	TestEqual(TEXT("A valid float retains its value"), Float, 1.5f);
	return true;
}

#endif
