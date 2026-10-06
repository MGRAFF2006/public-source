// Copyright Kyri123 / KMods 2026. All Rights Reserved.

#include "Reflection/KDFVanillaCache.h"
#include "Reflection/KDFPropertyPath.h"

namespace
{
	FString CanonicalSnapshotPath(const UObject* Object, const FString& PropertyPath)
	{
		FKDFPropertyPath Path;
		FString Error;
		if (IsValid(Object) && FKDFPropertyPath::Parse(PropertyPath, Path, Error))
		{
			const FString& RootName = Path.mSegments[0].mName;
			if (const FProperty* RootProperty =
				FKDFPropertyResolver::FindPropertyByNameFlexible(Object->GetClass(), RootName))
			{
				return RootProperty->GetName() + PropertyPath.RightChop(RootName.Len());
			}
		}
		return PropertyPath;
	}
}


void FKDFVanillaCache::RecordSnapshot(const UObject* Object, const FString& PropertyPath, const FString& ExportedValue)
{
	TMap<FString, FString>& ObjectSnapshots = mSnapshots.FindOrAdd(FObjectKey(Object));
	const FString CanonicalPath = CanonicalSnapshotPath(Object, PropertyPath);
	if (!ObjectSnapshots.Contains(CanonicalPath))
	{
		ObjectSnapshots.Add(CanonicalPath, ExportedValue);
	}
}

const FString* FKDFVanillaCache::FindSnapshot(const UObject* Object, const FString& PropertyPath) const
{
	const TMap<FString, FString>* ObjectSnapshots = mSnapshots.Find(FObjectKey(Object));
	return ObjectSnapshots ? ObjectSnapshots->Find(CanonicalSnapshotPath(Object, PropertyPath)) : nullptr;
}

const TMap<FString, FString>* FKDFVanillaCache::FindObjectSnapshots(const UObject* Object) const
{
	return mSnapshots.Find(FObjectKey(Object));
}

bool FKDFVanillaCache::HasAnySnapshot(const UObject* Object) const { return mSnapshots.Contains(FObjectKey(Object)); }

bool FKDFVanillaCache::HasSnapshotUnderProperty(const UObject* Object, const FString& PropertyName) const
{
	const TMap<FString, FString>* ObjectSnapshots = mSnapshots.Find(FObjectKey(Object));
	if (ObjectSnapshots == nullptr)
	{
		return false;
	}
	for (const TTuple<FString, FString>& Snapshot : *ObjectSnapshots)
	{
		const FString& Path = Snapshot.Key;
		if (Path.Len() < PropertyName.Len() || !Path.StartsWith(PropertyName, ESearchCase::CaseSensitive))
		{
			continue;
		}
		if (Path.Len() == PropertyName.Len() || Path[PropertyName.Len()] == TEXT('[') ||
			Path[PropertyName.Len()] == TEXT('.'))
		{
			return true;
		}
	}
	return false;
}

void FKDFVanillaCache::Reset() { mSnapshots.Reset(); }
