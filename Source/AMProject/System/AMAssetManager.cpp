// Fill out your copyright notice in the Description page of Project Settings.

#include "AMAssetManager.h"
#include "AMProject/AMLogChannels.h"

UAMAssetManager::UAMAssetManager()
{
}

UAMAssetManager& UAMAssetManager::Get()
{
	check(GEngine);

	if (UAMAssetManager* Singleton = Cast<UAMAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}

	UE_LOG(LogAM, Fatal,
	       TEXT("invalid AssetManagerClassname in DefaultEngine.ini(project settings); it must be AMAssetManager"));

	return *NewObject<UAMAssetManager>();
}

bool UAMAssetManager::ShouldLogAssetLoads()
{
	const TCHAR* CommandLineContent = FCommandLine::Get();
	return FParse::Param(CommandLineContent, TEXT("LogAssetLoads"));
}

UObject* UAMAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (AssetPath.IsValid())
	{
		TUniquePtr<FScopeLogTime> LogTimePtr;
		if (ShouldLogAssetLoads())
		{
			LogTimePtr = MakeUnique<FScopeLogTime>(
				*FString::Printf(TEXT("synchronous loaded assets [%s]"), *AssetPath.ToString()), nullptr,
				FScopeLogTime::ScopeLog_Seconds);
		}

		if (UAssetManager::IsValid())
		{
			return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath);
		}

		return AssetPath.TryLoad();
	}

	return nullptr;
}

void UAMAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (ensureAlways(Asset))
	{
		FScopeLock Lock(&SyncObject);
		LoadedAssets.Add(Asset);
	}
}
