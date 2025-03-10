// Copyright Jakob Bouchard. All Rights Reserved.

#include "DiscordRuntime.h"


FDiscordRuntimeModule& FDiscordRuntimeModule::Get()
{
	return FModuleManager::Get().GetModuleChecked<FDiscordRuntimeModule>("DiscordRuntime");
}

void FDiscordRuntimeModule::StartupModule()
{
	FString PathToDll = FPaths::ProjectPluginsDir() / TEXT("Discord/Binaries/ThirdParty/DiscordGameSdk") / FPlatformProcess::GetBinariesSubdirectory();
	const FString DllName = FString::Printf(TEXT("discord_game_sdk.%s"), FPlatformProcess::GetModuleExtension());

#if PLATFORM_MAC
#if PLATFORM_CPU_ARM_FAMILY
	PathToDll = FPaths::Combine(*PathToDll, TEXT("arm64"));
#else
	PathToDll = FPaths::Combine(*PathToDll, TEXT("x64"));
#endif
#endif
	
	DiscordGameSdkDllHandle = FPlatformProcess::GetDllHandle(*FPaths::Combine(*PathToDll, *DllName));
}

void FDiscordRuntimeModule::ShutdownModule()
{
	if (DiscordGameSdkDllHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(DiscordGameSdkDllHandle);
		DiscordGameSdkDllHandle = nullptr;
	}
}

bool FDiscordRuntimeModule::IsSdkAvailable() const
{
	return DiscordGameSdkDllHandle != nullptr;
}
	
IMPLEMENT_MODULE(FDiscordRuntimeModule, DiscordRuntime)
