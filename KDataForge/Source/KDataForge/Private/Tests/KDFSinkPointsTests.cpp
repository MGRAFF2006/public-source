// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Handlers/KDFSinkPointsHandler.h"
#include "Misc/AutomationTest.h"
#include "UObject/UObjectGlobals.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKDFSinkPointsRangeTest, "KDataForge.SinkPoints.IntegerRange",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKDFSinkPointsRangeTest::RunTest(const FString& Parameters)
{
	UKDFSinkPointsHandler* Handler = NewObject<UKDFSinkPointsHandler>();
	const auto Validate = [Handler](const FString& Points, const FString& Overridden)
	{
		const TSharedRef<FKDFNode> Document = FKDFNode::MakeMap();
		const TSharedRef<FKDFNode> Entries = FKDFNode::MakeSequence();
		const TSharedRef<FKDFNode> Entry = FKDFNode::MakeMap();
		Entry->SetChild(TEXT("item"), FKDFNode::MakeScalar(TEXT("/Script/FactoryGame.FGItemDescriptor"), true));
		Entry->SetChild(TEXT("points"), FKDFNode::MakeScalar(Points, false));
		if (!Overridden.IsEmpty())
		{
			Entry->SetChild(TEXT("overridden"), FKDFNode::MakeScalar(Overridden, false));
		}
		Entries->AddChild(Entry);
		Document->SetChild(TEXT("entries"), Entries);
		FKDFValidationContext Context;
		return Handler->ValidateDocument(Document.Get(), Context);
	};

	TestTrue(TEXT("Maximum int32 points remain valid"), Validate(TEXT("2147483647"), FString()));
	TestTrue(TEXT("Minimum int32 override remains valid"), Validate(TEXT("0"), TEXT("-2147483648")));
	TestFalse(TEXT("Points above int32 are rejected"), Validate(TEXT("2147483648"), FString()));
	TestFalse(TEXT("Points below int32 are rejected"), Validate(TEXT("-2147483649"), FString()));
	TestFalse(TEXT("Overflowed override is rejected"), Validate(TEXT("0"), TEXT("2147483648")));
	TestFalse(TEXT("Non-integer override is rejected"), Validate(TEXT("0"), TEXT("1.5")));
	return true;
}

#endif
