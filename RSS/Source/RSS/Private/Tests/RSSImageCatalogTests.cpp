// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Serialization/RssJsonSerializer.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRSSImageCatalogTest, "RSS.Images.TransactionalCatalogParse",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FRSSImageCatalogTest::RunTest(const FString& Parameters)
{
	FRssCustomDatas Catalog;
	FRssCustomSignUrlData& Previous = Catalog.mInformations.AddDefaulted_GetRef();
	Previous.mUrl = TEXT("https://example.com/keep.png");
	TestFalse(TEXT("A schema document without the catalog array is rejected"), FRssJsonSerializer::JsonToCustomData(
		TEXT("{\"schemaVersion\":1}"), Catalog));
	TestFalse(TEXT("A template document is not an image catalog"), FRssJsonSerializer::JsonToCustomData(
		TEXT("{\"schemaVersion\":1,\"templates\":[]}"), Catalog));
	TestFalse(TEXT("Entries require a URL"), FRssJsonSerializer::JsonToCustomData(
		TEXT("{\"schemaVersion\":1,\"informations\":[{}]}"), Catalog));
	TestFalse(TEXT("Entries require a nonempty URL"), FRssJsonSerializer::JsonToCustomData(
		TEXT("{\"schemaVersion\":1,\"informations\":[{\"url\":\"\"}]}"), Catalog));
	TestFalse(TEXT("A late malformed entry rejects the entire catalog"), FRssJsonSerializer::JsonToCustomData(
		TEXT("{\"schemaVersion\":1,\"informations\":[{\"url\":\"https://example.com/new.png\"},null]}"), Catalog));
	TestEqual(TEXT("Failure preserves the previous entry count"), Catalog.mInformations.Num(), 1);
	if (Catalog.mInformations.Num() == 1)
	{
		TestEqual(TEXT("Failure preserves the previous entry"), Catalog.mInformations[0].mUrl,
			FString(TEXT("https://example.com/keep.png")));
	}
	TestTrue(TEXT("A valid empty catalog succeeds"), FRssJsonSerializer::JsonToCustomData(
		TEXT("{\"schemaVersion\":1,\"informations\":[]}"), Catalog));
	TestEqual(TEXT("A valid empty catalog clears previous entries"), Catalog.mInformations.Num(), 0);
	return true;
}

#endif
