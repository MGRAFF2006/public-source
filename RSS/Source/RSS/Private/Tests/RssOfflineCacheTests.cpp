// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Cache/RssImageCache.h"
#include "Misc/AutomationTest.h"
#include "Subsystem/RSSImageSubsystem.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRssOfflineCacheTest, "RSS.Images.OfflineMemoryCache",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FRssOfflineCacheTest::RunTest(const FString& Parameters)
{
	URssImageCacheManager* Cache = NewObject<URssImageCacheManager>();
	FRssImageCacheConfig Config;
	Config.bEnableDiskCache = false;
	Cache->Initialize(Config);
	const FString Url = TEXT("https://cached-image.invalid/image.png");
	UTexture2DDynamic* Texture = NewObject<UTexture2DDynamic>();
	Cache->AddToMemoryCache(Url, Texture, 4, false);
	TestTrue(TEXT("Cached URLs remain eligible without resolving their hostname"), ARSSImageSubsystem::IsUrlValid(Url));
	TestTrue(TEXT("A cached request succeeds before DNS validation"), Cache->RequestImage(Url));
	UTexture2DDynamic* Retrieved = nullptr;
	TestTrue(TEXT("The cached texture remains available"), Cache->GetCachedImage(Url, Retrieved));
	TestTrue(TEXT("The original texture is returned"), Retrieved == Texture);
	TestFalse(TEXT("Unsafe literal destinations remain rejected"), Cache->RequestImage(TEXT("https://127.0.0.1/image.png")));
	Cache->Shutdown();
	return true;
}

#endif
