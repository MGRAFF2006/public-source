// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "BlueprintFunctionLib/KPCLSwatchTextLib.h"
#include "Misc/AutomationTest.h"

#include <limits>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKPCLSwatchTextTest, "KPrivateCodeLib.Swatches.PortableText",
								EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKPCLSwatchTextTest::RunTest(const FString& Parameters)
{
	FCustomSwatchData Original;
	Original.mSlotName = TEXT("Factory");
	Original.mPrimaryColor = FLinearColor(0.25f, 0.5f, 0.75f, 1.f);
	Original.mSecondaryColor = FLinearColor(2.f, -0.5f, 0.f, 1.f);
	FString Text;
	FText Error;
	TArray<FCustomSwatchData> Parsed;
	TestTrue(TEXT("Finite HDR swatches export"), UKPCLSwatchTextLib::ExportSwatches({Original}, Text, Error));
	TestTrue(TEXT("Exported text imports"), UKPCLSwatchTextLib::ParseSwatches(Text, Parsed, Error));
	if (TestEqual(TEXT("One swatch is parsed"), Parsed.Num(), 1))
	{
		TestTrue(TEXT("Name and both linear colors round trip"), Parsed[0] == Original);
	}
	const FString PreviousText = Text;
	TestFalse(TEXT("Duplicate names cannot be exported"), UKPCLSwatchTextLib::ExportSwatches({Original, Original}, Text, Error));
	TestEqual(TEXT("Failed export preserves output"), Text, PreviousText);
	Original.mPrimaryColor.R = std::numeric_limits<float>::quiet_NaN();
	TestFalse(TEXT("Nonfinite colors cannot be exported"), UKPCLSwatchTextLib::ExportSwatches({Original}, Text, Error));
	Original.mPrimaryColor.R = 0.f;
	Original.mSlotName = FName(*FString::ChrN(UKPCLSwatchTextLib::MaxNameLength + 1, TEXT('x')));
	TestFalse(TEXT("Oversized names cannot be exported"), UKPCLSwatchTextLib::ExportSwatches({Original}, Text, Error));

	const TArray<FString> InvalidTexts = {
		TEXT("not json"),
		TEXT(R"({"version":2,"swatches":[{"name":"A","primary":[0,0,0,1],"secondary":[0,0,0,1]}]})"),
		TEXT(R"({"version":"1","swatches":[{"name":"A","primary":[0,0,0,1],"secondary":[0,0,0,1]}]})"),
		TEXT(R"({"version":1,"swatches":[]})"),
		TEXT(R"({"version":1,"swatches":[{"name":"A","primary":[0,0,1],"secondary":[0,0,0,1]}]})"),
		TEXT(R"({"version":1,"swatches":[{"name":"A","primary":["0",0,0,1],"secondary":[0,0,0,1]}]})"),
		TEXT(R"({"version":1,"swatches":[{"name":12,"primary":[0,0,0,1],"secondary":[0,0,0,1]}]})"),
		TEXT(R"({"version":1,"swatches":[{"name":"bad\nname","primary":[0,0,0,1],"secondary":[0,0,0,1]}]})"),
		TEXT(R"({"version":1,"swatches":[{"name":"A","primary":[0,0,0,1],"secondary":[0,0,0,1]},{"name":"a","primary":[1,1,1,1],"secondary":[0,0,0,1]}]})"),
		TEXT(R"({"version":1,"swatches":[{"name":"A","primary":[0,0,0,1],"secondary":[0,0,0,1]},null]})")};
	for (const FString& InvalidText : InvalidTexts)
	{
		Parsed = {Original};
		TestFalse(TEXT("Malformed, unsupported and duplicate payloads are rejected"), UKPCLSwatchTextLib::ParseSwatches(InvalidText, Parsed, Error));
		if (TestEqual(TEXT("Failed parsing preserves output count"), Parsed.Num(), 1))
		{
			TestTrue(TEXT("Failed parsing preserves output values"), Parsed[0] == Original);
		}
		TestFalse(TEXT("Invalid input returns a useful error"), Error.IsEmpty());
	}
	TestFalse(TEXT("Oversized text is rejected before JSON parsing"),
			  UKPCLSwatchTextLib::ParseSwatches(FString::ChrN(UKPCLSwatchTextLib::MaxTextBytes + 1, TEXT(' ')), Parsed, Error));
	return true;
}

#endif
