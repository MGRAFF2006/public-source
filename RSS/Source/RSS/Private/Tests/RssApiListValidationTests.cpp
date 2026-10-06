// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Api/RssApiClient.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/AutomationTest.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/RssJsonSerializer.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRssApiListValidationTest, "RSS.Api.ListValidation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRssApiListValidationTest::RunTest(const FString& Parameters)
{
	FRssSignData Design;
	Design.mSignType = ESignType::RSS_Normal;
	Design.mSignTypeSize = ESignSize::RSS_1x1;
	Design.mTemplateData.mTemplateName = FText::FromString(TEXT("Keep me"));
	TArray<FRssSignData> Designs = {Design};
	for (const FString& Malformed : {FString(TEXT("not json")), FString(TEXT("{}")),
		FString(TEXT("{\"data\":null}")), FString(TEXT("{\"data\":[null]}")),
		FString(TEXT("{\"data\":[{}]}")), FString(TEXT("{\"data\":[{\"signData\":{}}]}"))})
	{
		TestFalse(TEXT("Malformed lists fail"), URssApiClient::ParseDesignList(Malformed, Designs));
		TestEqual(TEXT("Failure preserves the prior list"), Designs.Num(), 1);
	}

	TSharedPtr<FJsonObject> DesignRoot;
	if (!TestTrue(TEXT("Valid fixture serializes"), FJsonSerializer::Deserialize(
		TJsonReaderFactory<>::Create(FRssJsonSerializer::SignDataToJson(Design)), DesignRoot)))
	{
		return false;
	}
	TArray<TSharedPtr<FJsonValue>> Data = {MakeShared<FJsonValueObject>(DesignRoot)};
	TSharedRef<FJsonObject> Response = MakeShared<FJsonObject>();
	Response->SetArrayField(TEXT("data"), Data);
	FString Content;
	FJsonSerializer::Serialize(Response, TJsonWriterFactory<>::Create(&Content));
	TestTrue(TEXT("Valid design list parses"), URssApiClient::ParseDesignList(Content, Designs));
	TestEqual(TEXT("Design retained"), Designs.Num(), 1);
	Data.Add(MakeShared<FJsonValueNull>());
	Response->SetArrayField(TEXT("data"), Data);
	Content.Empty();
	FJsonSerializer::Serialize(Response, TJsonWriterFactory<>::Create(&Content));
	TestFalse(TEXT("Late invalid entry rejects the entire list"), URssApiClient::ParseDesignList(Content, Designs));
	TestEqual(TEXT("Late failure preserves prior list"), Designs.Num(), 1);
	TestTrue(TEXT("Empty list is valid"), URssApiClient::ParseDesignList(TEXT("{\"data\":[]}"), Designs));
	TestEqual(TEXT("Empty list replaces previous list"), Designs.Num(), 0);
	return true;
}

#endif
