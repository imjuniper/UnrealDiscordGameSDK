// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "Discord/types.h"
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
	
	explicit FDiscordActivityTimestamps(discord::ActivityTimestamps const& Timestamps)
	{
		Start = Timestamps.GetStart();
		End = Timestamps.GetEnd();
	}

	discord::ActivityTimestamps ToDiscordType() const
	{
		discord::ActivityTimestamps Timestamps;
		
		Timestamps.SetStart(Start);
		Timestamps.SetEnd(End);
		
		return Timestamps;
	}

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
	
	explicit FDiscordActivityAssets(discord::ActivityAssets const& Assets)
	{
		LargeImageKey = Assets.GetLargeImage();
		LargeImageText = Assets.GetLargeText();
		SmallImageKey = Assets.GetSmallImage();
		SmallImageText = Assets.GetSmallText();
	}

	discord::ActivityAssets ToDiscordType() const
	{
		discord::ActivityAssets Assets;
		
		Assets.SetLargeImage(TCHAR_TO_UTF8(*LargeImageKey));
		Assets.SetLargeText(TCHAR_TO_UTF8(*LargeImageText));
		Assets.SetSmallImage(TCHAR_TO_UTF8(*SmallImageKey));
		Assets.SetSmallText(TCHAR_TO_UTF8(*SmallImageText));
		
		return Assets;
	}

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
	
	explicit FDiscordActivityParty(discord::ActivityParty const& Party)
	{
		ID = Party.GetId();
		CurrentSize = Party.GetSize().GetCurrentSize();
		MaxSize = Party.GetSize().GetMaxSize();
	}

	discord::ActivityParty ToDiscordType() const
	{
		discord::ActivityParty Party;
		
		Party.SetId(TCHAR_TO_UTF8(*ID));
		Party.GetSize().SetCurrentSize(CurrentSize);
		Party.GetSize().SetMaxSize(MaxSize);
		
		return Party;
	}

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
	
	explicit FDiscordActivitySecrets(discord::ActivitySecrets const& Secrets)
	{
		Match = Secrets.GetMatch();
		Join = Secrets.GetJoin();
	}

	discord::ActivitySecrets ToDiscordType() const
	{
		discord::ActivitySecrets Secrets;
		
		Secrets.SetMatch(TCHAR_TO_UTF8(*Match));
		Secrets.SetJoin(TCHAR_TO_UTF8(*Join));
		
		return Secrets;
	}

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

	explicit FDiscordActivity(discord::Activity const& Activity)
	{
		ApplicationID = Activity.GetApplicationId();
		Name = Activity.GetName();
		State = Activity.GetState();
		Details = Activity.GetDetails();
		bInstance = Activity.GetInstance();
		Timestamps = FDiscordActivityTimestamps(Activity.GetTimestamps());
		Assets = FDiscordActivityAssets(Activity.GetAssets());
		Party = FDiscordActivityParty(Activity.GetParty());
		Secrets = FDiscordActivitySecrets(Activity.GetSecrets());
	}

	discord::Activity ToDiscordType() const
	{
		discord::Activity Activity{};

		if (ApplicationID != -1)
		{
			Activity.SetApplicationId(ApplicationID);
			Activity.SetName(TCHAR_TO_UTF8(*Name));
		}
		Activity.SetState(TCHAR_TO_UTF8(*State));
		Activity.SetDetails(TCHAR_TO_UTF8(*Details));
		Activity.SetInstance(bInstance);

		// Activity Timestamps
		Activity.GetTimestamps().SetStart(Timestamps.Start);
		Activity.GetTimestamps().SetEnd(Timestamps.End);
	
		// Activity Assets
		Activity.GetAssets().SetLargeImage(TCHAR_TO_UTF8(*Assets.LargeImageKey));
		Activity.GetAssets().SetLargeText(TCHAR_TO_UTF8(*Assets.LargeImageText));
		Activity.GetAssets().SetSmallImage(TCHAR_TO_UTF8(*Assets.SmallImageKey));
		Activity.GetAssets().SetSmallText(TCHAR_TO_UTF8(*Assets.SmallImageText));
	
		// Activity Party
		Activity.GetParty().SetId(TCHAR_TO_UTF8(*Party.ID));
		Activity.GetParty().GetSize().SetCurrentSize(Party.CurrentSize);
		Activity.GetParty().GetSize().SetMaxSize(Party.MaxSize);
	
		// Activity Secrets
		Activity.GetSecrets().SetMatch(TCHAR_TO_UTF8(*Secrets.Match));
		Activity.GetSecrets().SetJoin(TCHAR_TO_UTF8(*Secrets.Join));
		
		return Activity;
	}
	
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