// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#include "Api/RssApiClient.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "HttpModule.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"

#include "Engine/World.h"
#include "Serialization/RssJsonSerializer.h"

DECLARE_LOG_CATEGORY_EXTERN(RSSApiClientLog, Log, All)
DEFINE_LOG_CATEGORY(RSSApiClientLog)

namespace
{
	bool DeserializeSignDataObject(const TSharedPtr<FJsonObject>& SignDataObject, FRssSignData& OutSignData)
	{
		if (!SignDataObject.IsValid())
		{
			return false;
		}

		TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
		RootObject->SetNumberField(TEXT("schemaVersion"), 1);
		RootObject->SetObjectField(TEXT("signData"), SignDataObject);

		FString JsonString;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
		return FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer) &&
			FRssJsonSerializer::JsonToSignData(JsonString, OutSignData);
	}
}

TMap<TWeakObjectPtr<UWorld>, TWeakObjectPtr<URssApiClient>> URssApiClient::Instances;

URssApiClient* URssApiClient::Get(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	TWeakObjectPtr<UWorld> WorldKey(World);
	if (Instances.Contains(WorldKey) && Instances[WorldKey].IsValid())
	{
		return Instances[WorldKey].Get();
	}

	URssApiClient* NewClient = NewObject<URssApiClient>();
	Instances.Add(WorldKey, NewClient);
	return NewClient;
}

void URssApiClient::SetBaseUrl(const FString& NewBaseUrl)
{
	BaseUrl = NewBaseUrl;
	UE_LOG(RSSApiClientLog, Log, TEXT("API base URL set to: %s"), *BaseUrl);
}

void URssApiClient::SetBearerToken(const FString& NewToken)
{
	BearerToken = NewToken;
	if (!NewToken.IsEmpty())
	{
		UE_LOG(RSSApiClientLog, Log, TEXT("Bearer token configured"));
	}
	else
	{
		UE_LOG(RSSApiClientLog, Log, TEXT("Bearer token cleared"));
	}
}

void URssApiClient::FetchOwnDesigns(UObject* WorldContextObject, int32 Limit, int32 Offset)
{
	StartRequest(CreateOwnDesignsRequest(Limit, Offset));
}

FHttpRequestPtr URssApiClient::CreateOwnDesignsRequest(int32 Limit, int32 Offset)
{
	return MakeAuthenticatedRequest(TEXT("/designs"), &URssApiClient::OnFetchOwnDesignsResponse,
		FString::Printf(TEXT("?limit=%d&offset=%d"), Limit, Offset));
}

void URssApiClient::FetchDesignById(UObject* WorldContextObject, const FString& DesignId)
{
	StartRequest(CreateDesignByIdRequest(DesignId));
}

FHttpRequestPtr URssApiClient::CreateDesignByIdRequest(const FString& DesignId)
{
	return MakeAuthenticatedRequest(BuildDesignEndpoint(DesignId), &URssApiClient::OnFetchDesignByIdResponse);
}

void URssApiClient::FetchPublicDesigns(UObject* WorldContextObject, int32 Limit, int32 Offset)
{
	StartRequest(CreatePublicDesignsRequest(Limit, Offset));
}

FHttpRequestPtr URssApiClient::CreatePublicDesignsRequest(int32 Limit, int32 Offset)
{
	return MakeAuthenticatedRequest(TEXT("/designs/public"), &URssApiClient::OnFetchPublicDesignsResponse,
		FString::Printf(TEXT("?limit=%d&offset=%d"), Limit, Offset));
}

void URssApiClient::FetchDesignLimits(UObject* WorldContextObject)
{
	StartRequest(CreateDesignLimitsRequest());
}

FHttpRequestPtr URssApiClient::CreateDesignLimitsRequest()
{
	return MakeAuthenticatedRequest(TEXT("/limits"), &URssApiClient::OnFetchDesignLimitsResponse);
}

void URssApiClient::StartRequest(FHttpRequestPtr Request)
{
	if (Request.IsValid() && !Request->ProcessRequest())
	{
		Request->OnProcessRequestComplete().ExecuteIfBound(Request, nullptr, false);
	}
}

FString URssApiClient::BuildDesignEndpoint(const FString& DesignId)
{
	return FString::Printf(TEXT("/designs/%s"), *FGenericPlatformHttp::UrlEncode(DesignId));
}

FHttpRequestPtr URssApiClient::MakeAuthenticatedRequest(const FString& Endpoint, FResponseHandler ResponseHandler,
	const FString& QueryParams)
{
	FHttpModule& HttpModule = FHttpModule::Get();
	FHttpRequestPtr Request = HttpModule.CreateRequest();

	FString Url = BaseUrl + Endpoint + QueryParams;
	Request->SetURL(Url);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	if (!BearerToken.IsEmpty())
	{
		FString AuthHeader = FString::Printf(TEXT("Bearer %s"), *BearerToken);
		Request->SetHeader(TEXT("Authorization"), AuthHeader);
	}

	UE_LOG(RSSApiClientLog, Log, TEXT("Making request to: %s"), *Url);

	Request->OnProcessRequestComplete().BindUObject(this, ResponseHandler);

	PendingRequests.Add(Request, Endpoint);
	return Request;
}

void URssApiClient::OnFetchOwnDesignsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
											  bool bSucceeded)
{
	TArray<FRssSignData> Designs;

	if (bSucceeded && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
		UE_LOG(RSSApiClientLog, Verbose, TEXT("OwnDesigns response: %s"), *ResponseStr);

		TSharedPtr<FJsonObject> RootObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
		if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* DesignArray = nullptr;
			if (RootObject->TryGetArrayField(TEXT("data"), DesignArray) && DesignArray != nullptr)
			{
				for (const TSharedPtr<FJsonValue>& Value : *DesignArray)
				{
					if (Value.IsValid() && Value->Type == EJson::Object)
					{
						TSharedPtr<FJsonObject> DesignObj = Value->AsObject();
						const TSharedPtr<FJsonObject>* SignDataObj = nullptr;
						if (DesignObj.IsValid() && DesignObj->TryGetObjectField(TEXT("signData"), SignDataObj) &&
							SignDataObj != nullptr)
						{
							FRssSignData SignData;
							if (DeserializeSignDataObject(*SignDataObj, SignData))
							{
								Designs.Add(SignData);
							}
						}
					}
				}
			}
		}

		CompleteListRequest(HttpRequest, true, Designs, 0, FString());
		return;
	}

	int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
	FString ErrorMsg = bSucceeded ? TEXT("Unknown error") : TEXT("Request failed");
	CompleteListRequest(HttpRequest, false, Designs, ErrorCode, ErrorMsg);
}

void URssApiClient::OnFetchDesignByIdResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
											  bool bSucceeded)
{
	FRssSignData SignData;

	if (bSucceeded && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
		UE_LOG(RSSApiClientLog, Verbose, TEXT("DesignById response: %s"), *ResponseStr);

		TSharedPtr<FJsonObject> RootObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
		if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
		{
			const TSharedPtr<FJsonObject>* DataObj = nullptr;
			if (RootObject->TryGetObjectField(TEXT("data"), DataObj) && DataObj != nullptr)
			{
				const TSharedPtr<FJsonObject>* SignDataObj = nullptr;
				if ((*DataObj).IsValid() && (*DataObj)->TryGetObjectField(TEXT("signData"), SignDataObj) &&
					SignDataObj != nullptr)
				{
					if (DeserializeSignDataObject(*SignDataObj, SignData))
					{
						CompleteDesignRequest(HttpRequest, true, SignData, 0, FString());
						return;
					}
				}
			}
		}
	}

	int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
	FString ErrorMsg = bSucceeded ? TEXT("Failed to parse response") : TEXT("Request failed");
	CompleteDesignRequest(HttpRequest, false, SignData, ErrorCode, ErrorMsg);
}

void URssApiClient::OnFetchPublicDesignsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
												 bool bSucceeded)
{
	TArray<FRssSignData> Designs;

	if (bSucceeded && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
		UE_LOG(RSSApiClientLog, Verbose, TEXT("PublicDesigns response size: %d bytes"), ResponseStr.Len());

		TSharedPtr<FJsonObject> RootObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
		if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
		{
			const TArray<TSharedPtr<FJsonValue>>* DesignArray = nullptr;
			if (RootObject->TryGetArrayField(TEXT("data"), DesignArray) && DesignArray != nullptr)
			{
				for (const TSharedPtr<FJsonValue>& Value : *DesignArray)
				{
					if (Value.IsValid() && Value->Type == EJson::Object)
					{
						TSharedPtr<FJsonObject> DesignObj = Value->AsObject();
						const TSharedPtr<FJsonObject>* SignDataObj = nullptr;
						if (DesignObj.IsValid() && DesignObj->TryGetObjectField(TEXT("signData"), SignDataObj) &&
							SignDataObj != nullptr)
						{
							FRssSignData SignData;
							if (DeserializeSignDataObject(*SignDataObj, SignData))
							{
								Designs.Add(SignData);
							}
						}
					}
				}
			}
		}

		CompleteListRequest(HttpRequest, true, Designs, 0, FString());
		return;
	}

	int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
	FString ErrorMsg = bSucceeded ? TEXT("Unknown error") : TEXT("Request failed");
	CompleteListRequest(HttpRequest, false, Designs, ErrorCode, ErrorMsg);
}

void URssApiClient::OnFetchDesignLimitsResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse,
												bool bSucceeded)
{
	if (bSucceeded && HttpResponse.IsValid() && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		FString ResponseStr = HttpResponse->GetContentAsString();
		UE_LOG(RSSApiClientLog, Verbose, TEXT("DesignLimits response: %s"), *ResponseStr);

		TSharedPtr<FJsonObject> RootObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
		if (FJsonSerializer::Deserialize(Reader, RootObject) && RootObject.IsValid())
		{
			if (RootObject->HasField(TEXT("data")))
			{

				CompleteLimitsRequest(HttpRequest, true, 200, TEXT("Limits fetched"));
				return;
			}
		}
	}

	int32 ErrorCode = HttpResponse.IsValid() ? HttpResponse->GetResponseCode() : 0;
	FString ErrorMsg = bSucceeded ? TEXT("Failed to parse response") : TEXT("Request failed");
	CompleteLimitsRequest(HttpRequest, false, ErrorCode, ErrorMsg);
}

void URssApiClient::CompleteListRequest(FHttpRequestPtr Request, bool bSuccess,
	const TArray<FRssSignData>& Designs, int32 ErrorCode, const FString& ErrorMessage)
{
	if (!PendingRequests.Remove(Request))
	{
		return;
	}
	OnListRequestComplete.Broadcast(Request, bSuccess, Designs, ErrorCode, ErrorMessage);
	if (!bSuccess)
	{
		OnApiError.Broadcast(ErrorCode, ErrorMessage);
	}
	OnSignDesignListFetched.Broadcast(bSuccess, Designs);
}

void URssApiClient::CompleteDesignRequest(FHttpRequestPtr Request, bool bSuccess, const FRssSignData& Design,
	int32 ErrorCode, const FString& ErrorMessage)
{
	if (!PendingRequests.Remove(Request))
	{
		return;
	}
	OnDesignRequestComplete.Broadcast(Request, bSuccess, Design, ErrorCode, ErrorMessage);
	if (!bSuccess)
	{
		OnApiError.Broadcast(ErrorCode, ErrorMessage);
	}
	OnSignDesignFetched.Broadcast(bSuccess, Design);
}

void URssApiClient::CompleteLimitsRequest(FHttpRequestPtr Request, bool bSuccess, int32 ErrorCode,
	const FString& ErrorMessage)
{
	if (!PendingRequests.Remove(Request))
	{
		return;
	}
	OnLimitsRequestComplete.Broadcast(Request, bSuccess, ErrorCode, ErrorMessage);
	OnApiError.Broadcast(ErrorCode, ErrorMessage);
}

URssApiFetchOwnDesignsAction* URssApiFetchOwnDesignsAction::FetchOwnDesigns(UObject* WorldContextObject, int32 Limit,
																			int32 Offset)
{
	URssApiFetchOwnDesignsAction* Action = NewObject<URssApiFetchOwnDesignsAction>();
	Action->RegisterWithGameInstance(WorldContextObject);
	Action->ApiClient = URssApiClient::Get(WorldContextObject);
	Action->LimitParam = Limit;
	Action->OffsetParam = Offset;
	return Action;
}

void URssApiFetchOwnDesignsAction::Activate()
{
	if (bActivated || bCompleted)
	{
		return;
	}
	bActivated = true;
	if (!IsValid(ApiClient))
	{
		bCompleted = true;
		OnFailed.Broadcast(400, TEXT("API client not available"));
		SetReadyToDestroy();
		return;
	}

	Request = ApiClient->CreateOwnDesignsRequest(LimitParam, OffsetParam);
	ApiClient->OnListRequestComplete.AddUObject(this, &URssApiFetchOwnDesignsAction::OnRequestComplete);
	ApiClient->StartRequest(Request);
}

void URssApiFetchOwnDesignsAction::OnListFetched(bool bSuccess, const TArray<FRssSignData>& Designs)
{
	if (bCompleted)
	{
		return;
	}
	bCompleted = true;
	if (bSuccess)
	{
		OnSuccess.Broadcast(true, Designs);
	}
	else
	{
		OnFailed.Broadcast(500, TEXT("Failed to fetch designs"));
	}
	SetReadyToDestroy();
}

void URssApiFetchOwnDesignsAction::OnError(int32 ErrorCode, const FString& ErrorMessage)
{
	if (bCompleted)
	{
		return;
	}
	bCompleted = true;
	OnFailed.Broadcast(ErrorCode, ErrorMessage);
	SetReadyToDestroy();
}

void URssApiFetchOwnDesignsAction::SetReadyToDestroy()
{
	bCompleted = true;
	if (IsValid(ApiClient))
	{
		ApiClient->OnListRequestComplete.RemoveAll(this);
		ApiClient->OnSignDesignListFetched.RemoveDynamic(this, &URssApiFetchOwnDesignsAction::OnListFetched);
		ApiClient->OnApiError.RemoveDynamic(this, &URssApiFetchOwnDesignsAction::OnError);
	}
	Request.Reset();
	Super::SetReadyToDestroy();
}

void URssApiFetchOwnDesignsAction::OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess,
	const TArray<FRssSignData>& Designs,
	int32 ErrorCode, const FString& ErrorMessage)
{
	if (CompletedRequest != Request || bCompleted)
	{
		return;
	}
	if (bSuccess)
	{
		OnListFetched(true, Designs);
	}
	else
	{
		OnError(ErrorCode, ErrorMessage);
	}
}

URssApiFetchDesignByIdAction* URssApiFetchDesignByIdAction::FetchDesignById(UObject* WorldContextObject,
																			const FString& DesignId)
{
	URssApiFetchDesignByIdAction* Action = NewObject<URssApiFetchDesignByIdAction>();
	Action->RegisterWithGameInstance(WorldContextObject);
	Action->ApiClient = URssApiClient::Get(WorldContextObject);
	Action->DesignId = DesignId;
	return Action;
}

void URssApiFetchDesignByIdAction::Activate()
{
	if (bActivated || bCompleted)
	{
		return;
	}
	bActivated = true;
	if (!IsValid(ApiClient))
	{
		bCompleted = true;
		OnFailed.Broadcast(400, TEXT("API client not available"));
		SetReadyToDestroy();
		return;
	}

	Request = ApiClient->CreateDesignByIdRequest(DesignId);
	ApiClient->OnDesignRequestComplete.AddUObject(this, &URssApiFetchDesignByIdAction::OnRequestComplete);
	ApiClient->StartRequest(Request);
}

void URssApiFetchDesignByIdAction::OnDesignFetched(bool bSuccess, const FRssSignData& SignData)
{
	if (bCompleted)
	{
		return;
	}
	bCompleted = true;
	if (bSuccess)
	{
		OnSuccess.Broadcast(true, SignData);
	}
	else
	{
		OnFailed.Broadcast(500, TEXT("Failed to fetch design"));
	}
	SetReadyToDestroy();
}

void URssApiFetchDesignByIdAction::OnError(int32 ErrorCode, const FString& ErrorMessage)
{
	if (bCompleted)
	{
		return;
	}
	bCompleted = true;
	OnFailed.Broadcast(ErrorCode, ErrorMessage);
	SetReadyToDestroy();
}

void URssApiFetchDesignByIdAction::SetReadyToDestroy()
{
	bCompleted = true;
	if (IsValid(ApiClient))
	{
		ApiClient->OnDesignRequestComplete.RemoveAll(this);
		ApiClient->OnSignDesignFetched.RemoveDynamic(this, &URssApiFetchDesignByIdAction::OnDesignFetched);
		ApiClient->OnApiError.RemoveDynamic(this, &URssApiFetchDesignByIdAction::OnError);
	}
	Request.Reset();
	Super::SetReadyToDestroy();
}

void URssApiFetchDesignByIdAction::OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess,
	const FRssSignData& Design,
	int32 ErrorCode, const FString& ErrorMessage)
{
	if (CompletedRequest != Request || bCompleted)
	{
		return;
	}
	if (bSuccess)
	{
		OnDesignFetched(true, Design);
	}
	else
	{
		OnError(ErrorCode, ErrorMessage);
	}
}

URssApiFetchPublicDesignsAction* URssApiFetchPublicDesignsAction::FetchPublicDesigns(UObject* WorldContextObject,
																					 int32 Limit, int32 Offset)
{
	URssApiFetchPublicDesignsAction* Action = NewObject<URssApiFetchPublicDesignsAction>();
	Action->RegisterWithGameInstance(WorldContextObject);
	Action->ApiClient = URssApiClient::Get(WorldContextObject);
	Action->LimitParam = Limit;
	Action->OffsetParam = Offset;
	return Action;
}

void URssApiFetchPublicDesignsAction::Activate()
{
	if (bActivated || bCompleted)
	{
		return;
	}
	bActivated = true;
	if (!IsValid(ApiClient))
	{
		bCompleted = true;
		OnFailed.Broadcast(400, TEXT("API client not available"));
		SetReadyToDestroy();
		return;
	}

	Request = ApiClient->CreatePublicDesignsRequest(LimitParam, OffsetParam);
	ApiClient->OnListRequestComplete.AddUObject(this, &URssApiFetchPublicDesignsAction::OnRequestComplete);
	ApiClient->StartRequest(Request);
}

void URssApiFetchPublicDesignsAction::OnListFetched(bool bSuccess, const TArray<FRssSignData>& Designs)
{
	if (bCompleted)
	{
		return;
	}
	bCompleted = true;
	if (bSuccess)
	{
		OnSuccess.Broadcast(true, Designs);
	}
	else
	{
		OnFailed.Broadcast(500, TEXT("Failed to fetch designs"));
	}
	SetReadyToDestroy();
}

void URssApiFetchPublicDesignsAction::OnError(int32 ErrorCode, const FString& ErrorMessage)
{
	if (bCompleted)
	{
		return;
	}
	bCompleted = true;
	OnFailed.Broadcast(ErrorCode, ErrorMessage);
	SetReadyToDestroy();
}

void URssApiFetchPublicDesignsAction::SetReadyToDestroy()
{
	bCompleted = true;
	if (IsValid(ApiClient))
	{
		ApiClient->OnListRequestComplete.RemoveAll(this);
		ApiClient->OnSignDesignListFetched.RemoveDynamic(this, &URssApiFetchPublicDesignsAction::OnListFetched);
		ApiClient->OnApiError.RemoveDynamic(this, &URssApiFetchPublicDesignsAction::OnError);
	}
	Request.Reset();
	Super::SetReadyToDestroy();
}

void URssApiFetchPublicDesignsAction::OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess,
	const TArray<FRssSignData>& Designs,
	int32 ErrorCode, const FString& ErrorMessage)
{
	if (CompletedRequest != Request || bCompleted)
	{
		return;
	}
	if (bSuccess)
	{
		OnListFetched(true, Designs);
	}
	else
	{
		OnError(ErrorCode, ErrorMessage);
	}
}

URssApiFetchDesignLimitsAction* URssApiFetchDesignLimitsAction::FetchDesignLimits(UObject* WorldContextObject)
{
	URssApiFetchDesignLimitsAction* Action = NewObject<URssApiFetchDesignLimitsAction>();
	Action->RegisterWithGameInstance(WorldContextObject);
	Action->ApiClient = URssApiClient::Get(WorldContextObject);
	return Action;
}

void URssApiFetchDesignLimitsAction::Activate()
{
	if (bActivated || bCompleted)
	{
		return;
	}
	bActivated = true;
	if (!IsValid(ApiClient))
	{
		bCompleted = true;
		OnFailed.Broadcast(400, TEXT("API client not available"));
		SetReadyToDestroy();
		return;
	}

	Request = ApiClient->CreateDesignLimitsRequest();
	ApiClient->OnLimitsRequestComplete.AddUObject(this, &URssApiFetchDesignLimitsAction::OnRequestComplete);
	ApiClient->StartRequest(Request);
}

void URssApiFetchDesignLimitsAction::OnError(int32 ErrorCode, const FString& ErrorMessage)
{
	if (bCompleted)
	{
		return;
	}
	bCompleted = true;
	if (ErrorCode == 200)
	{
		OnSuccess.Broadcast(200, TEXT("Limits fetched successfully"));
	}
	else
	{
		OnFailed.Broadcast(ErrorCode, ErrorMessage);
	}
	SetReadyToDestroy();
}

void URssApiFetchDesignLimitsAction::SetReadyToDestroy()
{
	bCompleted = true;
	if (IsValid(ApiClient))
	{
		ApiClient->OnLimitsRequestComplete.RemoveAll(this);
		ApiClient->OnApiError.RemoveDynamic(this, &URssApiFetchDesignLimitsAction::OnError);
	}
	Request.Reset();
	Super::SetReadyToDestroy();
}

void URssApiFetchDesignLimitsAction::OnRequestComplete(FHttpRequestPtr CompletedRequest, bool bSuccess,
	int32 ErrorCode, const FString& ErrorMessage)
{
	if (CompletedRequest != Request || bCompleted)
	{
		return;
	}
	bCompleted = true;
	if (bSuccess)
	{
		OnSuccess.Broadcast(200, TEXT("Limits fetched successfully"));
	}
	else
	{
		OnFailed.Broadcast(ErrorCode, ErrorMessage);
	}
	SetReadyToDestroy();
}
