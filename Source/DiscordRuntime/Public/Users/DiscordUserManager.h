// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "DiscordTypes.h"
#include "DiscordUser.h"
#include "UObject/Object.h"
#include "DiscordUserManager.generated.h"

enum class EDiscordOutputPins : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscordCurrentUserUpdatedSignature, FDiscordUser, User);


UCLASS(Within=DiscordSubsystem)
class DISCORDRUNTIME_API UDiscordUserManager : public UObject
{
	friend class UDiscordSubsystem;
	
	GENERATED_BODY()

private:
	UDiscordUserManager();
	void Initialize(discord::UserManager* UserManager);
	virtual void BeginDestroy() override;

private:
	UPROPERTY()
	TObjectPtr<UDiscordSubsystem> DiscordSubsystem = nullptr;
	
	discord::UserManager* Internal_UserManager = nullptr;

	int Internal_OnCurrentUserUpdateCallback;

public:
	/**
	 * Fetch information about the currently connected user account. Returns whether the called was a success.
	 */
	UFUNCTION(BlueprintPure, Category="Discord|User", meta=(ReturnDisplayName="Success"))
	bool GetCurrentUser(FDiscordUser& User) const;

	/**
	 * Get user information for a given User ID.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Discord|User", meta=(WorldContext="WorldContext", Latent, LatentInfo="LatentInfo", ExpandEnumAsExecs="OutputPins"))
	void GetUser(const UObject* WorldContext, const FLatentActionInfo LatentInfo, const int64 UserID, FDiscordUser& User, EDiscordOutputPins& OutputPins) const;

	/**
	 * Get user information for a given User ID.
	 */
	void GetUser(const int64 UserID, TFunction<void(discord::Result, discord::User const&)> Callback) const;

	/**
	 * Fetch information about the currently connected user account. Returns whether the called was a success.
	 */
	UFUNCTION(BlueprintPure, Category="Discord|User")
	TEnumAsByte<EDiscordPremiumTypes::Type> GetCurrentUserPremiumType() const;

	/**
	 * Returns whether the current user has the flag.
	 */
	UFUNCTION(BlueprintPure, Category="Discord|User")
	bool CurrentUserHasFlag(const EDiscordUserFlags::Type Flag) const;

public:
	/**
	 * Fires when the `User` struct of the currently connected user changes. They may have changed their avatar,
	 * username, or something else.
	 */
	UPROPERTY(BlueprintAssignable, Category="Discord|User")
	FOnDiscordCurrentUserUpdatedSignature OnCurrentUserUpdated;
};
