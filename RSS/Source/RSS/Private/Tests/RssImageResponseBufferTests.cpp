// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Http/RssImageResponseBuffer.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRssImageResponseBufferTest, "RSS.Images.BoundedHttpReceive",
								EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::EngineFilter)

bool FRssImageResponseBufferTest::RunTest(const FString& Parameters)
{
	uint8 Bytes[] = {1, 2, 3, 4, 5, 6, 7, 8};
	FRssImageResponseBuffer Chunked(8);
	Chunked.Serialize(Bytes, 2);
	Chunked.Serialize(Bytes + 2, 3);
	Chunked.Serialize(Bytes + 5, 3);
	TestFalse(TEXT("Chunked receive accepts the exact limit"), Chunked.IsError());
	TestEqual(TEXT("All chunks are retained"), Chunked.GetData().Num(), 8);
	for (int32 Index = 0; Index < Chunked.GetData().Num(); ++Index)
	{
		TestEqual(TEXT("Chunk order is preserved"), Chunked.GetData()[Index], Bytes[Index]);
	}
	Chunked.Serialize(Bytes, 1);
	TestTrue(TEXT("First byte beyond the limit fails"), Chunked.IsError());
	TestEqual(TEXT("Overflow does not grow the buffer"), Chunked.GetData().Num(), 8);

	FRssImageResponseBuffer Oversized(8);
	Oversized.Serialize(Bytes, MAX_int64);
	TestTrue(TEXT("Unrepresentable count fails before copying"), Oversized.IsError());
	TestTrue(TEXT("Oversized chunk leaves no partial content"), Oversized.GetData().IsEmpty());
	Oversized.Serialize(Bytes, 1);
	TestTrue(TEXT("Archive errors are sticky"), Oversized.GetData().IsEmpty());

	FRssImageResponseBuffer Negative(8);
	Negative.Serialize(Bytes, -1);
	TestTrue(TEXT("Negative byte counts fail"), Negative.IsError());

	FRssImageResponseBuffer NullData(8);
	NullData.Serialize(nullptr, 0);
	TestFalse(TEXT("An empty chunk needs no source pointer"), NullData.IsError());
	NullData.Serialize(nullptr, 1);
	TestTrue(TEXT("Nonempty chunks require a source pointer"), NullData.IsError());
	TestTrue(TEXT("Invalid source does not append content"), NullData.GetData().IsEmpty());

	FRssImageResponseBuffer InvalidLimit(0);
	TestTrue(TEXT("Nonpositive receive limits fail closed"), InvalidLimit.IsError());
	return true;
}

#endif
