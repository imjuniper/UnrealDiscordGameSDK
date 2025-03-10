// Copyright Jakob Bouchard. All Rights Reserved.

using UnrealBuildTool;

public class DiscordEditor : ModuleRules
{
    public DiscordEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateIncludePaths.Add(ModuleDirectory + "/ThirdParty");

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "HTTP",
                "Slate",
                "SlateCore",
                "UnrealEd"
            }
        );
    }
}