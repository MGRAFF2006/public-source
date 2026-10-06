// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Dom/JsonObject.h"
#include "HttpLib/KPCLJsonObject.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKPCLJsonCollectionTest, "KPrivateCodeLib.Json.ObjectAndCollectionReads",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FKPCLJsonCollectionTest::RunTest(const FString& Parameters)
{
	const TSharedPtr<FJsonObject> Json = MakeShared<FJsonObject>();
	const TSharedPtr<FJsonObject> Child = MakeShared<FJsonObject>();
	Json->SetObjectField(TEXT("object"), Child);
	Json->SetStringField(TEXT("scalar"), TEXT("value"));
	Json->SetArrayField(TEXT("empty"), {});
	Json->SetArrayField(TEXT("mixed"), {MakeShared<FJsonValueObject>(Child), MakeShared<FJsonValueString>(TEXT("wrong"))});
	Json->SetArrayField(TEXT("objects"), {MakeShared<FJsonValueObject>(Child)});
	UKPCLJsonObject* Object = UKPCLJsonObject::CreateFromJson(Json);
	UKPCLJsonObject* Nested = nullptr;
	TestTrue(TEXT("Nested object lookup reports success"), Object->TryGetJsonObject(TEXT("object"), Nested));
	TestTrue(TEXT("The wrapper contains the nested object"), Nested && Nested->GetJson() == Child);
	TestFalse(TEXT("A scalar cannot be wrapped as an object"), Object->TryGetJson(TEXT("scalar"), Nested));
	TestTrue(TEXT("Failure preserves the output object"), Nested && Nested->GetJson() == Child);
	TestTrue(TEXT("The alternate object getter succeeds"), Object->TryGetJson(TEXT("object"), Nested));
	TArray<UKPCLJsonObject*> Objects = {Nested};
	TestFalse(TEXT("Mixed array rejects nonobjects"), Object->TryGetJsonArray(TEXT("mixed"), Objects));
	TestEqual(TEXT("Mixed array failure preserves output"), Objects.Num(), 1);
	TestTrue(TEXT("An empty object array succeeds"), Object->TryGetJsonArray(TEXT("empty"), Objects));
	TestEqual(TEXT("An empty object array clears previous output"), Objects.Num(), 0);
	TestTrue(TEXT("An object array succeeds"), Object->TryGetJsonArray(TEXT("objects"), Objects));
	TestEqual(TEXT("Object array contains one wrapper"), Objects.Num(), 1);
	TArray<FString> Strings = {TEXT("keep")};
	TestFalse(TEXT("A non-string array fails"), Object->TryGetStringArray(TEXT("mixed"), Strings));
	TestEqual(TEXT("A failed string array read preserves output"), Strings.Num(), 1);
	TestTrue(TEXT("An empty string array succeeds"), Object->TryGetStringArray(TEXT("empty"), Strings));
	TestEqual(TEXT("An empty string array clears previous output"), Strings.Num(), 0);
	return true;
}

#endif
