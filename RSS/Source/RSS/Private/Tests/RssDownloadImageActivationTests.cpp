// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Widget/RssDownloadImage.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRssDownloadImageActivationTest, "RSS.Images.ActivationDefersTerminalCompletion",
								EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FRssDownloadImageActivationTest::RunTest(const FString& Parameters)
{
	const FString InvalidUrl = TEXT("invalid-url");
	URssDownloadImage* NoCacheAction = URssDownloadImage::DownloadImageNoCache(InvalidUrl);
	TestTrue(TEXT("No-cache factory has not completed before activation"), NoCacheAction->IsRooted());
	NoCacheAction->Activate();
	TestFalse(TEXT("Activated rejected request releases its root"), NoCacheAction->IsRooted());
	NoCacheAction->Activate();
	TestFalse(TEXT("Repeated activation does not restart the request"), NoCacheAction->IsRooted());

	URssDownloadImage* ContextAction = URssDownloadImage::DownloadImage(nullptr, InvalidUrl);
	TestTrue(TEXT("Context factory also defers completion"), ContextAction->IsRooted());
	ContextAction->Activate();
	TestFalse(TEXT("Context factory rejection completes after activation"), ContextAction->IsRooted());

	URssDownloadImage* NativeAction = NewObject<URssDownloadImage>();
	NativeAction->Start(InvalidUrl);
	TestFalse(TEXT("Explicit native start remains synchronous"), NativeAction->IsRooted());
	return true;
}

#endif
