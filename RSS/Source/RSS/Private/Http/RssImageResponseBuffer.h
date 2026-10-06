// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/Archive.h"

class FRssImageResponseBuffer final : public FArchive
{
public:
	explicit FRssImageResponseBuffer(int32 Limit) : mLimit(Limit)
	{
		if (Limit <= 0)
		{
			SetError();
		}
	}

	virtual void Serialize(void* Data, int64 Length) override;
	const TArray<uint8>& GetData() const { return mData; }

private:
	int32 mLimit;
	TArray<uint8> mData;
};
