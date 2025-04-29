// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "DiscordTypes.h"
#include "DiscordActivity.generated.h"


UENUM(BlueprintType)
namespace EDiscordActivityJoinRequestReplyTypes
{
	enum Type
	{
		No,
		Yes,
		Ignore,
	};
}


USTRUCT(BlueprintType)
struct FDiscordActivityTimestamps
{
	GENERATED_BODY()

public:
	FDiscordActivityTimestamps() = default;
	
	explicit FDiscordActivityTimestamps(discord::ActivityTimestamps const& Timestamps);

	discord::ActivityTimestamps ToDiscordType() const;

	/**
	 * Unix timestamp. Send this to have an "elapsed" timer.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity", meta=(ClampMin="0", UIMin="0"))
	int64 Start = 0;
	
	/**
	 * Unix timestamp. Send this to have a "remaining" timer.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity", meta=(ClampMin="0", UIMin="0"))
	int64 End = 0;
};


USTRUCT(BlueprintType)
struct FDiscordActivityAssets
{
	GENERATED_BODY()

public:
	FDiscordActivityAssets() = default;
	
	explicit FDiscordActivityAssets(discord::ActivityAssets const& Assets);

	discord::ActivityAssets ToDiscordType() const;

	/**
	 * Key for the large image.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString LargeImageKey;
	
	/**
	 * Hover text for the large image.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString LargeImageText;
	
	/**
	 * Key for the small image.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString SmallImageKey;
	
	/**
	 * Hover text for the small image.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString SmallImageText;
};


USTRUCT(BlueprintType)
struct FDiscordActivityParty
{
	GENERATED_BODY()

public:
	FDiscordActivityParty() = default;
	
	explicit FDiscordActivityParty(discord::ActivityParty const& Party);

	discord::ActivityParty ToDiscordType() const;

	/**
	 * A unique identifier for this party.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString ID;
	
	/**
	 * The current size of the party. Setting to 0 means no party.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity", meta=(ClampMin="0", UIMin="0"))
	int32 CurrentSize = 0;
	
	/**
	 * The max possible size of the party. Setting to 0 means no party.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity", meta=(ClampMin="0", UIMin="0"))
	int32 MaxSize = 0;
};


USTRUCT(BlueprintType)
struct FDiscordActivitySecrets
{
	GENERATED_BODY()

public:
	FDiscordActivitySecrets() = default;
	
	explicit FDiscordActivitySecrets(discord::ActivitySecrets const& Secrets);

	discord::ActivitySecrets ToDiscordType() const;

	/**
	 * Unique hash for the given match context.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString Match;

	/**
	 * Unique hash for chat invites and Ask to Join.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString Join;
};


USTRUCT(BlueprintType)
struct FDiscordActivity
{
	GENERATED_BODY()

public:
	FDiscordActivity() = default;

	explicit FDiscordActivity(discord::Activity const& Activity);

	discord::Activity ToDiscordType() const;

	/**
	 * Your application ID. This is a read-only field.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Discord|Activity")
	int64 ApplicationID = -1;

	/**
	 * Name of the application. This is a read-only field.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Discord|Activity")
	FString Name = "None";

	/**
	 * The player's current party status.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString State;

	/**
	 * What the player is currently doing.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FString Details;

	/**
	 * Helps create elapsed/remaining timestamps on a player's profile.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FDiscordActivityTimestamps Timestamps;

	/**
	 * Assets to display on the player's profile.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FDiscordActivityAssets Assets;

	/**
	 * Information about the player's party.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FDiscordActivityParty Party;

	/**
	 * Secret passwords for joining the player's game.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	FDiscordActivitySecrets Secrets;

	/**
	 * Whether this activity is an instanced context, like a match.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Discord|Activity")
	bool bInstance = false;
};