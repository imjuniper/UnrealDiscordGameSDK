// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "DiscordTypes.h"
#include "DiscordActivity.h" 
#include "UObject/Object.h"
#include "Users/DiscordUser.h"
#include "DiscordActivityManager.generated.h"

enum class EDiscordOutputPins : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscordActivityJoinSignature, FString, Secret);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiscordActivityJoinRequestSignature, FDiscordUser, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDiscordActivityInviteSignature, FDiscordUser, User, FDiscordActivity, Activity);


UCLASS(Within=DiscordSubsystem)
class DISCORDRUNTIME_API UDiscordActivityManager : public UObject
{
	friend class UDiscordSubsystem;
	
	GENERATED_BODY()

private:
	UDiscordActivityManager();
	void Initialize(discord::ActivityManager* ActivityManager);
	virtual void BeginDestroy() override;

private:
	UPROPERTY()
	TObjectPtr<UDiscordSubsystem> DiscordSubsystem = nullptr;
	
	discord::ActivityManager* Internal_ActivityManager = nullptr;

	int Internal_OnJoinCallback;
	int Internal_OnJoinRequestCallback;
	int Internal_OnInviteCallback;

public:
	/**
	 * Returns whether the call was a success. Registers a command by which Discord can launch your game. This might be
	 * a custom protocol, like `my-awesome-game://`, or a path to an executable. It also supports any launch parameters
	 * that may be needed, like `game.exe --full-screen --no-hax`.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Activity", meta=(ReturnDisplayName="Success"))
	bool RegisterCommand(const FString Command);

	/**
	 * Returns whether the call was a success. Used if you are distributing this SDK on Steam. Registers your game's
	 * Steam App ID for the protocol `steam://run-game-id/<id>`.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Activity", meta=(ReturnDisplayName="Success"))
	bool RegisterSteam(const int32 SteamAppID);

	/**
	 * Sets a user's presence in Discord to a new activity. This has a rate limit of 5 updates per 20 seconds.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Activity", meta=(WorldContext="WorldContext", Latent, LatentInfo="LatentInfo", ExpandEnumAsExecs="OutputPins"))
	void UpdateActivity(const UObject* WorldContext, const FLatentActionInfo LatentInfo, const FDiscordActivity NewActivity, EDiscordOutputPins& OutputPins);
	
	/**
	 * Sets a user's presence in Discord to a new activity. This has a rate limit of 5 updates per 20 seconds.
	 */
	void UpdateActivity(const FDiscordActivity NewActivity, TFunction<void(discord::Result)> Callback);

	/**
	 * Clear's a user's presence in Discord to make it show nothing.
	 * This probably won't work, see issue https://github.com/discord/discord-api-docs/issues/6612
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Activity", meta=(WorldContext="WorldContext", Latent, LatentInfo="LatentInfo", ExpandEnumAsExecs="OutputPins"))
	void ClearActivity(const UObject* WorldContext, const FLatentActionInfo LatentInfo, EDiscordOutputPins& OutputPins);

	/**
	 * Clear's a user's presence in Discord to make it show nothing.
	 * This probably won't work, see issue https://github.com/discord/discord-api-docs/issues/6612
	 */
	void ClearActivity(TFunction<void(discord::Result)> Callback);

	/**
	 * Sends a reply to an Ask to Join request.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Activity", meta=(WorldContext="WorldContext", Latent, LatentInfo="LatentInfo", ExpandEnumAsExecs="OutputPins"))
	void SendRequestReply(const UObject* WorldContext, const FLatentActionInfo LatentInfo, const int64 UserID, const EDiscordActivityJoinRequestReplyTypes::Type Reply, EDiscordOutputPins& OutputPins);

	/**
	 * Sends a reply to an Ask to Join request.
	 */
	void SendRequestReply(const int64 UserID, const EDiscordActivityJoinRequestReplyTypes::Type Reply, TFunction<void(discord::Result)> Callback) const;
	
	/**
	 * Sends a game invite to a given user. If you do not have a valid activity with all the required fields,
	 * this call will error. 
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Activity", meta=(WorldContext="WorldContext", Latent, LatentInfo="LatentInfo", ExpandEnumAsExecs="OutputPins"))
	void SendInvite(const UObject* WorldContext, const FLatentActionInfo LatentInfo, const int64 UserID, const FString Content, EDiscordOutputPins& OutputPins);

	/**
	 * Sends a game invite to a given user. If you do not have a valid activity with all the required fields,
	 * this call will error.
	 */
	void SendInvite(const int64 UserID, const FString Content, TFunction<void(discord::Result)> Callback) const;

	/**
	 * Accepts a game invitation from a given User ID.
	 */
	UFUNCTION(BlueprintCallable, Category="Discord|Activity")
	void AcceptInvite(const int64 UserID) const;

public:
	/**
	 * Fires when a user accepts a join chat invite or clicks the Join button on a user's profile.
	 */
	UPROPERTY(BlueprintAssignable, Category="Discord|User")
	FOnDiscordActivityJoinSignature OnActivityJoin;

	/**
	 * Fires when a user asks to join the current user's game.
	 */
	UPROPERTY(BlueprintAssignable, Category="Discord|User")
	FOnDiscordActivityJoinRequestSignature OnActivityJoinRequest;

	/**
	 * Fires when the user receives a join invite.
	 */
	UPROPERTY(BlueprintAssignable, Category="Discord|User")
	FOnDiscordActivityInviteSignature OnActivityInvite;
};
