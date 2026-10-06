// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "EnumStruc/RssStruc.h"

#include "RssApiClient.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSignDesignFetched, bool, bSuccess, const FRssSignData&, SignData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSignDesignListFetched, bool, bSuccess, const TArray<FRssSignData>&,
											 Designs);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnApiError, int32, ErrorCode, const FString&, ErrorMessage);

DECLARE_MULTICAST_DELEGATE_FiveParams(FRssDesignListRequestComplete, FHttpRequestPtr, bool,
	const TArray<FRssSignData>&, int32, const FString&);
DECLARE_MULTICAST_DELEGATE_FiveParams(FRssDesignRequestComplete, FHttpRequestPtr, bool,
	const FRssSignData&, int32, const FString&);
DECLARE_MULTICAST_DELEGATE_FourParams(FRssLimitsRequestComplete, FHttpRequestPtr, bool, int32, const FString&);

UCLASS()
class RSS_API URssApiClient : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "RSS API", meta = (WorldContext = "WorldContextObject"))
	static URssApiClient* Get(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "RSS API")
	void SetBaseUrl(const FString& NewBaseUrl);

	UFUNCTION(BlueprintCallable, Category = "RSS API")
	void SetBearerToken(const FString& NewToken);

	UFUNCTION(BlueprintCallable, Category = "RSS API", meta = (WorldContext = "WorldContextObject"))
	void FetchOwnDesigns(UObject* WorldContextObject, int32 Limit = 50, int32 Offset = 0);

	UFUNCTION(BlueprintCallable, Category = "RSS API", meta = (WorldContext = "WorldContextObject"))
	void FetchDesignById(UObject* WorldContextObject, const FString& DesignId);

	UFUNCTION(BlueprintCallable, Category = "RSS API", meta = (WorldContext = "WorldContextObject"))
	void FetchPublicDesigns(UObject* WorldContextObject, int32 Limit = 50, int32 Offset = 0);

	UFUNCTION(BlueprintCallable, Category = "RSS API", meta = (WorldContext = "WorldContextObject"))
	void FetchDesignLimits(UObject* WorldContextObject);

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnSignDesignFetched OnSignDesignFetched;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnSignDesignListFetched OnSignDesignListFetched;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnApiError OnApiError;

private:
	friend class URssApiFetchOwnDesignsAction;
	friend class URssApiFetchDesignByIdAction;
	friend class URssApiFetchPublicDesignsAction;
	friend class URssApiFetchDesignLimitsAction;
	friend class FRssApiRequestCorrelationTest;
	FHttpRequestPtr CreateOwnDesignsRequest(int32 Limit, int32 Offset);
	FHttpRequestPtr CreateDesignByIdRequest(const FString& DesignId);
	FHttpRequestPtr CreatePublicDesignsRequest(int32 Limit, int32 Offset);
	FHttpRequestPtr CreateDesignLimitsRequest();
	void StartRequest(FHttpRequestPtr Request);
	void CompleteListRequest(FHttpRequestPtr Request, bool bSuccess, const TArray<FRssSignData>& Designs,
		int32 ErrorCode, const FString& ErrorMessage);
	void CompleteDesignRequest(FHttpRequestPtr Request, bool bSuccess, const FRssSignData& Design,
		int32 ErrorCode, const FString& ErrorMessage);
	void CompleteLimitsRequest(FHttpRequestPtr Request, bool bSuccess, int32 ErrorCode, const FString& ErrorMessage);
	FRssDesignListRequestComplete OnListRequestComplete;
	FRssDesignRequestComplete OnDesignRequestComplete;
	FRssLimitsRequestComplete OnLimitsRequestComplete;


	void OnFetchOwnDesignsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void OnFetchDesignByIdResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void OnFetchPublicDesignsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void OnFetchDesignLimitsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FHttpRequestPtr MakeAuthenticatedRequest(const FString& Endpoint, const FString& QueryParams = FString());

	UPROPERTY()
	FString BaseUrl = TEXT("https://k-mods.com/api/v2/rss");

	UPROPERTY()
	FString BearerToken;

	TMap<FHttpRequestPtr, FString> PendingRequests;

	static TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<URssApiClient>> Instances;
};

UCLASS()
class RSS_API URssApiFetchOwnDesignsAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "RSS API",
			  meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static URssApiFetchOwnDesignsAction* FetchOwnDesigns(UObject* WorldContextObject, int32 Limit = 50,
														 int32 Offset = 0);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnSignDesignListFetched OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnApiError OnFailed;

private:
	friend class FRssApiActionLifecycleTest;
	friend class FRssApiRequestCorrelationTest;
	FHttpRequestPtr Request;
	bool bActivated = false;
	bool bCompleted = false;

	UPROPERTY()
	URssApiClient* ApiClient;

	int32 LimitParam;
	int32 OffsetParam;

	void OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess, const TArray<FRssSignData>& Designs,
		int32 ErrorCode, const FString& ErrorMessage);

	UFUNCTION()
	void OnListFetched(bool bSuccess, const TArray<FRssSignData>& Designs);

	UFUNCTION()
	void OnError(int32 ErrorCode, const FString& ErrorMessage);
};

UCLASS()
class RSS_API URssApiFetchDesignByIdAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "RSS API",
			  meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static URssApiFetchDesignByIdAction* FetchDesignById(UObject* WorldContextObject, const FString& DesignId);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnSignDesignFetched OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnApiError OnFailed;

private:
	friend class FRssApiActionLifecycleTest;
	friend class FRssApiRequestCorrelationTest;
	FHttpRequestPtr Request;
	bool bActivated = false;
	bool bCompleted = false;

	UPROPERTY()
	URssApiClient* ApiClient;

	FString DesignId;

	void OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess, const FRssSignData& Design,
		int32 ErrorCode, const FString& ErrorMessage);

	UFUNCTION()
	void OnDesignFetched(bool bSuccess, const FRssSignData& SignData);

	UFUNCTION()
	void OnError(int32 ErrorCode, const FString& ErrorMessage);
};

UCLASS()
class RSS_API URssApiFetchPublicDesignsAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "RSS API",
			  meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static URssApiFetchPublicDesignsAction* FetchPublicDesigns(UObject* WorldContextObject, int32 Limit = 50,
															   int32 Offset = 0);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnSignDesignListFetched OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnApiError OnFailed;

private:
	friend class FRssApiActionLifecycleTest;
	friend class FRssApiRequestCorrelationTest;
	FHttpRequestPtr Request;
	bool bActivated = false;
	bool bCompleted = false;

	UPROPERTY()
	URssApiClient* ApiClient;

	int32 LimitParam;
	int32 OffsetParam;

	void OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess, const TArray<FRssSignData>& Designs,
		int32 ErrorCode, const FString& ErrorMessage);

	UFUNCTION()
	void OnListFetched(bool bSuccess, const TArray<FRssSignData>& Designs);

	UFUNCTION()
	void OnError(int32 ErrorCode, const FString& ErrorMessage);
};

UCLASS()
class RSS_API URssApiFetchDesignLimitsAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "RSS API",
			  meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static URssApiFetchDesignLimitsAction* FetchDesignLimits(UObject* WorldContextObject);

	virtual void Activate() override;
	virtual void SetReadyToDestroy() override;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnApiError OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "RSS API")
	FOnApiError OnFailed;

private:
	friend class FRssApiActionLifecycleTest;
	friend class FRssApiRequestCorrelationTest;
	FHttpRequestPtr Request;
	bool bActivated = false;
	bool bCompleted = false;

	UPROPERTY()
	URssApiClient* ApiClient;

	void OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess, int32 ErrorCode,
		const FString& ErrorMessage);

	UFUNCTION()
	void OnError(int32 ErrorCode, const FString& ErrorMessage);
};
