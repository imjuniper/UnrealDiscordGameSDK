// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class DiscordRuntime : ModuleRules
{
	public DiscordRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange( new string[] { "Core", "CoreUObject", "DeveloperSettings", "Engine", "Projects" } );

		SetupDiscordSdk();
	}

	private void SetupDiscordSdk()
	{
		PrivateIncludePaths.Add(ModuleDirectory + "/ThirdParty");
		
		var BinariesPath = Path.Combine(PluginDirectory, "Binaries/ThirdParty/DiscordGameSdk", Target.Platform.ToString());

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			PublicAdditionalLibraries.Add(Path.Combine(BinariesPath, "discord_game_sdk.dll.lib"));
			RuntimeDependencies.Add(Path.Combine(BinariesPath, "discord_game_sdk.dll"));
			PublicDelayLoadDLLs.Add("discord_game_sdk.dll");
		}
		else if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PublicAdditionalLibraries.Add(Path.Combine(BinariesPath, Target.Architecture.ToString(), "discord_game_sdk.dylib"));
			RuntimeDependencies.Add(Path.Combine(BinariesPath, Target.Architecture.ToString(), "discord_game_sdk.dylib"));
		}
		else if (Target.Platform == UnrealTargetPlatform.Linux)
		{
			PublicAdditionalLibraries.Add(Path.Combine(BinariesPath, "discord_game_sdk.so"));
			RuntimeDependencies.Add(Path.Combine(BinariesPath, "discord_game_sdk.so"));
		}
	}
}
