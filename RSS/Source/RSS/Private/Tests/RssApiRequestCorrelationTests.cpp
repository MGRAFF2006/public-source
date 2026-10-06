// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Api/RssApiClient.h"
#include "HttpModule.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRssApiRequestCorrelationTest, "RSS.Api.RequestCorrelation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRssApiRequestCorrelationTest::RunTest(const FString& Parameters)
{
	URssApiClient* Client = NewObject<URssApiClient>();
	URssApiFetchOwnDesignsAction* Own = NewObject<URssApiFetchOwnDesignsAction>();
	URssApiFetchPublicDesignsAction* Public = NewObject<URssApiFetchPublicDesignsAction>();
	Own->ApiClient = Client;
	Public->ApiClient = Client;
	Own->Request = Client->CreateOwnDesignsRequest(50, 0);
	Public->Request = Client->CreatePublicDesignsRequest(50, 0);
	Client->OnListRequestComplete.AddUObject(Own, &URssApiFetchOwnDesignsAction::OnRequestComplete);
	Client->OnListRequestComplete.AddUObject(Public, &URssApiFetchPublicDesignsAction::OnRequestComplete);
	const FHttpRequestPtr PublicRequest = Public->Request;
	Client->CompleteListRequest(PublicRequest, true, TArray<FRssSignData>(), 0, FString());
	TestTrue(TEXT("Public request completes its action"), Public->bCompleted);
	TestFalse(TEXT("Public result does not finish own-design action"), Own->bCompleted);
	TestTrue(TEXT("Own request remains pending"), Client->PendingRequests.Contains(Own->Request));
	Client->OnFetchOwnDesignsResponse(Own->Request, nullptr, false);
	TestTrue(TEXT("Own request failure completes its action"), Own->bCompleted);
	TestEqual(TEXT("Completed requests are removed"), Client->PendingRequests.Num(), 0);
	int32 RepeatedNotifications = 0;
	Client->OnListRequestComplete.AddLambda([&RepeatedNotifications](FHttpRequestPtr, bool,
		const TArray<FRssSignData>&, int32, const FString&) { ++RepeatedNotifications; });
	Client->CompleteListRequest(PublicRequest, true, TArray<FRssSignData>(), 0, FString());
	TestEqual(TEXT("Repeated completion is ignored"), RepeatedNotifications, 0);
	Client->OnListRequestComplete.Clear();

	URssApiFetchDesignByIdAction* First = NewObject<URssApiFetchDesignByIdAction>();
	URssApiFetchDesignByIdAction* Second = NewObject<URssApiFetchDesignByIdAction>();
	First->ApiClient = Client;
	Second->ApiClient = Client;
	First->Request = Client->CreateDesignByIdRequest(TEXT("first"));
	Second->Request = Client->CreateDesignByIdRequest(TEXT("second"));
	Client->OnDesignRequestComplete.AddUObject(First, &URssApiFetchDesignByIdAction::OnRequestComplete);
	Client->OnDesignRequestComplete.AddUObject(Second, &URssApiFetchDesignByIdAction::OnRequestComplete);
	Client->CompleteDesignRequest(Second->Request, true, FRssSignData(), 0, FString());
	TestTrue(TEXT("Second ID completes"), Second->bCompleted);
	TestFalse(TEXT("First ID waits for its own response"), First->bCompleted);
	Client->OnFetchDesignByIdResponse(First->Request, nullptr, false);
	TestTrue(TEXT("First ID completes after its response"), First->bCompleted);

	URssApiFetchDesignLimitsAction* Limits = NewObject<URssApiFetchDesignLimitsAction>();
	Limits->ApiClient = Client;
	Limits->Request = Client->CreateDesignLimitsRequest();
	Client->OnLimitsRequestComplete.AddUObject(Limits, &URssApiFetchDesignLimitsAction::OnRequestComplete);
	Client->OnApiError.Broadcast(200, TEXT("Unrelated malformed response"));
	TestFalse(TEXT("Global errors cannot finish limits action"), Limits->bCompleted);
	Client->CompleteLimitsRequest(Limits->Request, false, 200, TEXT("Malformed limits"));
	TestTrue(TEXT("Limits response completes its own action"), Limits->bCompleted);
	return true;
}

#endif
