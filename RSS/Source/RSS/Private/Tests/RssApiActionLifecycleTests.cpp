// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Api/RssApiClient.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRssApiActionLifecycleTest, "RSS.Api.ActionCompletion",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRssApiActionLifecycleTest::RunTest(const FString& Parameters)
{
	URssApiClient* Client = NewObject<URssApiClient>();
	URssApiFetchOwnDesignsAction* Own = NewObject<URssApiFetchOwnDesignsAction>();
	Own->ApiClient = Client;
	Client->OnSignDesignListFetched.AddDynamic(Own, &URssApiFetchOwnDesignsAction::OnListFetched);
	Client->OnApiError.AddDynamic(Own, &URssApiFetchOwnDesignsAction::OnError);
	Client->OnSignDesignListFetched.Broadcast(true, TArray<FRssSignData>());
	TestTrue(TEXT("Own action completed"), Own->bCompleted);
	TestFalse(TEXT("Own list listener removed"), Client->OnSignDesignListFetched.IsAlreadyBound(
		Own, &URssApiFetchOwnDesignsAction::OnListFetched));
	TestFalse(TEXT("Own error listener removed"), Client->OnApiError.IsAlreadyBound(
		Own, &URssApiFetchOwnDesignsAction::OnError));

	URssApiFetchDesignByIdAction* ById = NewObject<URssApiFetchDesignByIdAction>();
	ById->ApiClient = Client;
	Client->OnSignDesignFetched.AddDynamic(ById, &URssApiFetchDesignByIdAction::OnDesignFetched);
	Client->OnApiError.AddDynamic(ById, &URssApiFetchDesignByIdAction::OnError);
	Client->OnApiError.Broadcast(503, TEXT("Fixture failure"));
	TestTrue(TEXT("By-id action completed after error"), ById->bCompleted);
	TestFalse(TEXT("By-id listener removed after error"), Client->OnSignDesignFetched.IsAlreadyBound(
		ById, &URssApiFetchDesignByIdAction::OnDesignFetched));
	TestFalse(TEXT("By-id error listener removed"), Client->OnApiError.IsAlreadyBound(
		ById, &URssApiFetchDesignByIdAction::OnError));

	URssApiFetchPublicDesignsAction* Public = URssApiFetchPublicDesignsAction::FetchPublicDesigns(nullptr);
	Public->Activate();
	TestTrue(TEXT("Missing-client public action completed"), Public->bCompleted);
	URssApiFetchDesignLimitsAction* Limits = URssApiFetchDesignLimitsAction::FetchDesignLimits(nullptr);
	Limits->Activate();
	TestTrue(TEXT("Missing-client limits action completed"), Limits->bCompleted);
	return true;
}

#endif
