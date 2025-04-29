// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "DiscordSettings.generated.h"


UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Discord Game SDK Settings"))
class UDiscordSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Whether to relaunch the app if Discord is not open. */
	UPROPERTY(Category="Configuration",Config,  EditDefaultsOnly, BlueprintReadOnly)
	bool bRequireDiscord = false;
	
	/** This can be obtained from Discord Developer Portal. */
	UPROPERTY(Category="Configuration", Config, EditDefaultsOnly, BlueprintReadOnly)
	int64 ClientID = -1;

	/** The time to wait before assuming a callback failed. */
	UPROPERTY(Category="Configuration", Config, EditDefaultsOnly, BlueprintReadOnly, meta=(Units="Seconds"))
	float TimeoutSeconds = 5.f;
};
