// Copyright Kyri123 / KMods 2026. All Rights Reserved.



#include "HttpLib/KPCLJsonObject.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "KPrivateCodeLibModule.h"
#include "Logging/StructuredLog.h"
#include "Misc/ConfigCacheIni.h"

UKPCLJsonObject* UKPCLJsonObject::Create()
{
	UKPCLJsonObject* JsonObject = NewObject<UKPCLJsonObject>();
	return JsonObject;
}

UKPCLJsonObject* UKPCLJsonObject::CreateFromJson(TSharedPtr<FJsonObject> JsonObject)
{
	UKPCLJsonObject* Object = Create();
	if (Object)
	{
		Object->SetJson(JsonObject);
	}
	return Object;
}

void UKPCLJsonObject::CreateJsonFromUrl(FString Url, TMap<FString, FString> Headers, EHttpRequest Method,
										FString PostContent)
{

	FHttpModule& HttpModule = FHttpModule::Get();
	HttpModule.ToggleNullHttp(false);
	HttpModule.UpdateConfigs();

	FHttpRequestRef Request = HttpModule.CreateRequest();
	Request->OnProcessRequestComplete().BindLambda(
		[this, Url, Headers, Method, PostContent](FHttpRequestPtr InRequest, FHttpResponsePtr Response, bool bSuccess)
		{
			UE_LOG(LogKPCL, Warning, TEXT("Request was to %s: %d"), *Url, bSuccess)
			bLastRequestWasSuccessful = false;
			if (bSuccess)
			{
				UE_LOG(LogKPCL, Warning, TEXT("Request Content from %s: %s"), *Url, *Response->GetContentAsString())
				bLastRequestWasSuccessful = FJsonSerializer::Deserialize(
					TJsonReaderFactory<>::Create(Response->GetContentAsString()), mJsonObject);
				if (bLastRequestWasSuccessful)
				{
					mRequestHeaders = Headers;
					mRequestUrl = Url;
					mRequestMethod = Method;
					mRequestContent = PostContent;
				}
			}

			if (OnHttpRequestComplete.IsBound())
			{
				OnHttpRequestComplete.Broadcast(bLastRequestWasSuccessful);
			}
		});

	Request->SetURL(Url);
	for (auto Header : Headers)
	{
		if (Header.Key != "Accepts" && Header.Key != "Content-Type" && Header.Key != "User-Agent")
		{
			if (!Header.Key.IsEmpty() && !Header.Value.IsEmpty())
			{
				Request->SetHeader(Header.Key, Header.Value);
			}
		}
	}

	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));

	Request->SetVerb(Method == EHttpRequest::GET ? "GET" : "POST");
	Request->SetContentAsString(PostContent);
	Request->ProcessRequest();
}

bool UKPCLJsonObject::TryRedoRequest()
{
	if (WasLastRequestSuccessful())
	{
		CreateJsonFromUrl(mRequestUrl, mRequestHeaders, mRequestMethod, mRequestContent);
		return true;
	}
	return false;
}

bool UKPCLJsonObject::TryGetStringJson(FString StringField, FString& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	return GetJson()->TryGetStringField(StringField, Result);
}

bool UKPCLJsonObject::TryGetBool(FString StringField, bool& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	return GetJson()->TryGetBoolField(StringField, Result);
}

bool UKPCLJsonObject::TryGetFloat(FString StringField, float& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	double Num;
	const bool Success = GetJson()->TryGetNumberField(StringField, Num);
	Result = Num;
	return Success;
}

bool UKPCLJsonObject::TryGetInt(FString StringField, int& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	double Num;
	const bool Success = GetJson()->TryGetNumberField(StringField, Num);
	Result = Num;
	return Success;
}

bool UKPCLJsonObject::TryGetStringArray(FString StringField, TArray<FString>& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	TArray<FString> Parsed;
	if (!GetJson()->TryGetStringArrayField(StringField, Parsed))
	{
		return false;
	}
	Result = MoveTemp(Parsed);
	return true;
}

bool UKPCLJsonObject::TryGetJsonArray(FString StringField, TArray<UKPCLJsonObject*>& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* DataArray = nullptr;
	if (!GetJson()->TryGetArrayField(StringField, DataArray) || !DataArray)
	{
		return false;
	}
	for (const TSharedPtr<FJsonValue>& ArrayElement : *DataArray)
	{
		if (!ArrayElement.IsValid() || ArrayElement->Type != EJson::Object || !ArrayElement->AsObject().IsValid())
		{
			return false;
		}
	}
	TArray<UKPCLJsonObject*> Parsed;
	for (const TSharedPtr<FJsonValue>& ArrayElement : *DataArray)
	{
		Parsed.Add(CreateFromJson(ArrayElement->AsObject()));
	}
	Result = MoveTemp(Parsed);
	return true;
}

bool UKPCLJsonObject::TryGetJson(FString StringField, UKPCLJsonObject*& Result)
{
	return TryGetJsonObject(StringField, Result);
}

bool UKPCLJsonObject::TryGetJsonObject(FString StringField, UKPCLJsonObject*& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	const TSharedPtr<FJsonObject>* Json = nullptr;
	if (GetJson()->TryGetObjectField(StringField, Json) && Json && Json->IsValid())
	{
		Result = CreateFromJson(*Json);
		return Result != nullptr;
	}

	return false;
}
