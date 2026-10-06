// Copyright Kyri123 / KMods 2026. All Rights Reserved.



#include "HttpLib/KPCLJsonObject.h"

#include "Async/Async.h"
#include "HAL/ThreadSafeCounter.h"
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
	TWeakObjectPtr<UKPCLJsonObject> WeakThis(this);
	TSharedRef<FThreadSafeCounter, ESPMode::ThreadSafe> CompletionCount =
		MakeShared<FThreadSafeCounter, ESPMode::ThreadSafe>();
	auto CompleteRequest = [WeakThis, CompletionCount, Url, Headers, Method, PostContent](
		FHttpRequestPtr InRequest, FHttpResponsePtr Response, bool bSuccess)
	{
		if (CompletionCount->Increment() != 1)
		{
			return;
		}
		auto ApplyResult = [WeakThis, Url, Headers, Method, PostContent, Response, bSuccess]()
		{
			UKPCLJsonObject* Object = WeakThis.Get();
			if (!Object)
			{
				return;
			}
			UE_LOG(LogKPCL, Warning, TEXT("Request was to %s: %d"), *Url, bSuccess)
			Object->bLastRequestWasSuccessful = false;
			if (bSuccess && Response.IsValid())
			{
				UE_LOG(LogKPCL, Warning, TEXT("Request Content from %s: %s"), *Url, *Response->GetContentAsString())
				Object->bLastRequestWasSuccessful = FJsonSerializer::Deserialize(
					TJsonReaderFactory<>::Create(Response->GetContentAsString()), Object->mJsonObject);
				if (Object->bLastRequestWasSuccessful)
				{
					Object->mRequestHeaders = Headers;
					Object->mRequestUrl = Url;
					Object->mRequestMethod = Method;
					Object->mRequestContent = PostContent;
				}
			}

			Object->OnHttpRequestComplete.Broadcast(Object->bLastRequestWasSuccessful);
		};
		if (IsInGameThread())
		{
			ApplyResult();
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, MoveTemp(ApplyResult));
		}
	};
	Request->OnProcessRequestComplete().BindLambda(CompleteRequest);

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
	if (!Request->ProcessRequest())
	{
		CompleteRequest(Request, nullptr, false);
	}
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

	GetJson()->TryGetStringArrayField(StringField, Result);
	return Result.Num() != 0;
}

bool UKPCLJsonObject::TryGetJsonArray(FString StringField, TArray<UKPCLJsonObject*>& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	Result.Empty();
	const TArray<TSharedPtr<FJsonValue>>* DataArray;
	if (GetJson()->TryGetArrayField(StringField, DataArray))
	{
		for (const TSharedPtr<FJsonValue>& ArrayElement : *DataArray)
		{
			if (ArrayElement)
			{
				UKPCLJsonObject* ElementObj = CreateFromJson(ArrayElement->AsObject());
				Result.Add(ElementObj);
			}
		}
	}
	return Result.Num() > 0;
}

bool UKPCLJsonObject::TryGetJson(FString StringField, UKPCLJsonObject*& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	Result = nullptr;
	TSharedPtr<FJsonValue> Field = GetJson()->TryGetField(StringField);
	if (Field)
	{
		Result = CreateFromJson(Field->AsObject());
	}
	return Result != nullptr;
}

bool UKPCLJsonObject::TryGetJsonObject(FString StringField, UKPCLJsonObject*& Result)
{
	if (!HasJsonObject())
	{
		return false;
	}

	const TSharedPtr<FJsonObject>* Json;
	if (GetJson()->TryGetObjectField(StringField, Json))
	{
		bool Success = false;
		Result = CreateFromJson(*Json);
		return Success;
	}

	return false;
}
