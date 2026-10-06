// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/RssJsonSerializer.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRSSTemplateSerializationTest, "RSS.Templates.TransactionalSerialization",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FRSSTemplateSerializationTest::RunTest(const FString& Parameters)
{
	FRssSignData Template;
	Template.mSignType = ESignType::RSS_Normal;
	Template.mSignTypeSize = ESignSize::RSS_1x1;
	Template.mTemplateData.mTemplateName = FText::FromString(TEXT("Keep me"));
	TArray<FRssSignData> Templates = {Template};
	const FString Json = FRssJsonSerializer::TemplatesToJson(Templates);
	TestFalse(TEXT("A valid catalog serializes"), Json.IsEmpty());
	TArray<FRssSignData> Parsed;
	TestTrue(TEXT("The valid catalog parses"), FRssJsonSerializer::JsonToTemplates(Json, Parsed));
	TestEqual(TEXT("The template is retained"), Parsed.Num(), 1);
	TSharedPtr<FJsonObject> Root;
	if (!TestTrue(TEXT("Fixture JSON parses"), FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(Json), Root)))
	{
		return false;
	}
	TArray<TSharedPtr<FJsonValue>> Entries = Root->GetArrayField(TEXT("templates"));
	const TSharedPtr<FJsonValue> AdditionalValidEntry = Entries[0];
	Entries.Add(AdditionalValidEntry);
	Entries.Add(MakeShared<FJsonValueNull>());
	Root->SetArrayField(TEXT("templates"), Entries);
	FString LateInvalid;
	FJsonSerializer::Serialize(Root.ToSharedRef(), TJsonWriterFactory<>::Create(&LateInvalid));
	TestFalse(TEXT("A late invalid entry rejects the catalog"), FRssJsonSerializer::JsonToTemplates(LateInvalid, Templates));
	TestEqual(TEXT("Late failure preserves the previous catalog"), Templates.Num(), 1);

	TestFalse(TEXT("A malformed entry rejects the catalog"), FRssJsonSerializer::JsonToTemplates(
		TEXT("{\"schemaVersion\":1,\"templates\":[null]}"), Templates));
	TestEqual(TEXT("A failed parse preserves the previous catalog"), Templates.Num(), 1);
	if (Templates.Num() == 1)
	{
		TestEqual(TEXT("A failed parse preserves the previous name"), Templates[0].mTemplateData.mTemplateName.ToString(),
			FString(TEXT("Keep me")));
	}
	Templates.Add(FRssSignData());
	TestTrue(TEXT("Invalid templates reject the entire export"), FRssJsonSerializer::TemplatesToJson(Templates).IsEmpty());
	TestTrue(TEXT("An empty valid catalog parses"), FRssJsonSerializer::JsonToTemplates(
		TEXT("{\"schemaVersion\":1,\"templates\":[]}"), Templates));
	TestEqual(TEXT("An empty catalog replaces the previous catalog"), Templates.Num(), 0);
	return true;
}

#endif
