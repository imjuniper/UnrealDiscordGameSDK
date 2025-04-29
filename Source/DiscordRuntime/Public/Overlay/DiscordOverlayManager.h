// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "DiscordTypes.h"
#include "UObject/Object.h"
#include "DiscordOverlayManager.generated.h"

enum class EDiscordOutputPins : uint8;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscordOverlayToggledSignature, bool, bLocked);

UCLASS(Within=DiscordSubsystem)
class DISCORDRUNTIME_API UDiscordOverlayManager : public UObject
{
	friend class UDiscordSubsystem;
	
	GENERATED_BODY()

private:
	UDiscordOverlayManager();
	void Initialize(discord::OverlayManager* OverlayManager);
	virtual void BeginDestroy() override;

private:
	UPROPERTY()
	TObjectPtr<UDiscordSubsystem> DiscordSubsystem = nullptr;
	
	discord::OverlayManager* Internal_OverlayManager = nullptr;

	int Internal_OnToggleCallback;

public:
	/**
	 * Return whether the user has the overlay enabled or disabled. If the overlay is disabled, all the functionality in
	 * this manager will still work. The calls will instead focus the Discord client and show the modal there instead.
	 */
	UFUNCTION(BlueprintPure, Category="Discord|Overlay")
	bool IsEnabled() const;

	/**
	 * Return whether the overlay is currently locked or unlocked
	 */
	UFUNCTION(BlueprintPure, Category="Discord|Overlay")
	bool IsLocked() const;

	/**
	 * Locks or unlocks input in the overlay. Calling `SetLocked(true)` will also close any modals in the overlay.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Overlay", meta=(WorldContext="WorldContext", Latent, LatentInfo="LatentInfo", ExpandEnumAsExecs="OutputPins"))
	void SetLocked(const UObject* WorldContext, const FLatentActionInfo LatentInfo, const bool bLocked, EDiscordOutputPins& OutputPins);
	
	/**
	 * Locks or unlocks input in the overlay. Calling `SetLocked(true)` will also close any modals in the overlay.
	 */
	void SetLocked(const bool bLocked, TFunction<void(discord::Result)> Callback);
	
	/**
	 * Opens the overlay modal for sending game invitations to users, channels, and servers. If you do not have a valid
	 * activity with all the required fields, this call will error.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Overlay")
	void OpenActivityInvite();

	/**
	 * Opens the overlay modal for joining a Discord guild, given its invite code.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Overlay")
	void OpenGuildInvite(const FString InviteCode);

	/**
	 * Opens the overlay widget for voice settings for the currently connected application. These settings are unique to
	 * each user within the context of your application.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Overlay")
	void OpenVoiceSettings();

public:
	/**
	 * Fires when the overlay is locked or unlocked (a.k.a. opened or closed).
	 */
	UPROPERTY(BlueprintAssignable, Category="Discord|Overlay")
	FOnDiscordOverlayToggledSignature OnToggled;
};
