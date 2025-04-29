// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "DiscordTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "Runtime/Launch/Resources/Version.h"
#include "DiscordSubsystem.generated.h"

class UDiscordActivityManager;
class UDiscordUserManager;
class UDiscordOverlayManager;

#define DISCORD_UE_VERSION (ENGINE_MAJOR_VERSION * 100 + ENGINE_MINOR_VERSION)


UCLASS()
class UDiscordSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	// Begin USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem interface

	// Begin FTickableGameObject interface
	virtual void Tick(const float DeltaTime) override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual ETickableTickType GetTickableTickType() const override;
#if DISCORD_UE_VERSION < 505
	virtual bool IsAllowedToTick() const override;
#endif
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDiscordSubsystem, STATGROUP_Tickables); }
	// End FTickableGameObject interface

	/**
	 * Returns whether the subsystem is currently initialized. Will usually return false if the Client ID isn't set in
	 * settings, if the Discord SDK binaries are missing or if the Client failed to initialize.
	 */
	UFUNCTION(BlueprintPure, Category = "Discord")
	bool IsActive() const;

	/**
	 * Returns the current instance of Discord Activity Manager.
	 */
	UFUNCTION(BlueprintPure, Category="Discord")
	UDiscordActivityManager* GetActivityManager() const { check(ActivityManager); return ActivityManager; }

	/**
	 * Returns the current instance of Discord User Manager.
	 */
	UFUNCTION(BlueprintPure, Category="Discord")
	UDiscordUserManager* GetUserManager() const { check(UserManager); return UserManager; }

	/**
	 * Returns the current instance of Discord Overlay Manager.
	 */
	UFUNCTION(BlueprintPure, Category="Discord")
	UDiscordOverlayManager* GetOverlayManager() const { check(OverlayManager); return OverlayManager; }

private:
	discord::Core* Core = nullptr;
	
	UPROPERTY()
	TObjectPtr<UDiscordActivityManager> ActivityManager;
	
	UPROPERTY()
	TObjectPtr<UDiscordUserManager> UserManager;

	UPROPERTY()
	TObjectPtr<UDiscordOverlayManager> OverlayManager;
};
