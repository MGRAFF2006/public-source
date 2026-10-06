// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Api/RssApiClient.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRssApiDesignEndpointTest, "RSS.Api.DesignEndpoint",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRssApiDesignEndpointTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Reserved words remain design IDs"),
		URssApiClient::BuildDesignEndpoint(TEXT("public-limits")), FString(TEXT("/designs/public-limits")));
	TestEqual(TEXT("Path and query delimiters are encoded"),
		URssApiClient::BuildDesignEndpoint(TEXT("a/b?c#d %")), FString(TEXT("/designs/a%2Fb%3Fc%23d%20%25")));
	return true;
}

#endif
