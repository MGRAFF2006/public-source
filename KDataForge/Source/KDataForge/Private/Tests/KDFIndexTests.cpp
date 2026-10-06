// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "KDFNode.h"
#include "Misc/AutomationTest.h"
#include "Reflection/KDFOpEngine.h"
#include "Reflection/KDFPropertyPath.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKDFIndexValidationTest, "KDataForge.Reflection.IndexValidation",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKDFIndexValidationTest::RunTest(const FString& Parameters)
{
	FKDFPropertyPath Path;
	FString Error;
	TestTrue(TEXT("An integer index parses"), FKDFPropertyPath::Parse(TEXT("Values[1]"), Path, Error));
	TestEqual(TEXT("Parsed index remains exact"), Path.mSegments.Last().mIndex, 1);
	TestFalse(TEXT("Fractional indices fail"), FKDFPropertyPath::Parse(TEXT("Values[1.5]"), Path, Error));
	TestFalse(TEXT("Overflowed indices fail"), FKDFPropertyPath::Parse(TEXT("Values[4294967296]"), Path, Error));
	TestTrue(TEXT("Quoted numeric map keys remain keys"), FKDFPropertyPath::Parse(TEXT("Values['1.5']"), Path, Error));
	TestTrue(TEXT("The quoted accessor is a map key"), Path.mSegments.Last().mKind == EKDFPathSegmentKind::Key);

	const TSharedRef<FKDFNode> Entry = FKDFNode::MakeMap();
	Entry->SetChild(TEXT("path"), FKDFNode::MakeScalar(TEXT("Values"), true));
	Entry->SetChild(TEXT("op"), FKDFNode::MakeScalar(TEXT("remove_at"), true));
	Entry->SetChild(TEXT("index"), FKDFNode::MakeScalar(TEXT("4294967296"), false));
	FString PathText;
	EKDFOp Op = EKDFOp::Set;
	FKDFOpArgs Args;
	TestFalse(TEXT("Overflowed operation indices fail"), FKDFOpEngine::ParseOpEntry(Entry.Get(), PathText, Op, Args, Error));
	Entry->SetChild(TEXT("index"), FKDFNode::MakeScalar(TEXT("1.5"), false));
	TestFalse(TEXT("Fractional operation indices fail"), FKDFOpEngine::ParseOpEntry(Entry.Get(), PathText, Op, Args, Error));
	return true;
}

#endif
