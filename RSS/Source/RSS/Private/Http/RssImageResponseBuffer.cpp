// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#include "Http/RssImageResponseBuffer.h"

void FRssImageResponseBuffer::Serialize(void* Data, int64 Length)
{
	if (IsError() || Length < 0 || Length > static_cast<int64>(mLimit) - mData.Num() || (Length > 0 && !Data))
	{
		SetError();
		return;
	}
	if (Length > 0)
	{
		mData.Append(static_cast<const uint8*>(Data), static_cast<int32>(Length));
	}
}
