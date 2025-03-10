// Copyright Jakob Bouchard. All Rights Reserved.

#pragma once

#include "Discord/types.h"
#include "DiscordUser.generated.h"


UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor=true))
namespace EDiscordUserFlags
{
	enum Type
	{
		None = 0                UMETA(Hidden),
		Partner = 1             UMETA(DisplayName="Discord Partner"),
		HypeSquadEvents = 2     UMETA(DisplayName="HypeSquad Events participant"),
		HypeSquadHouse1 = 32    UMETA(DisplayName="House Bravery"),
		HypeSquadHouse2 = 64    UMETA(DisplayName="House Brilliance"),
		HypeSquadHouse3 = 128   UMETA(DisplayName="House Balance"),
	};
}


UENUM(BlueprintType)
namespace EDiscordPremiumTypes
{
	enum Type
	{
		None     UMETA(DisplayName="No Nitro"),
		Tier1    UMETA(DisplayName="Nitro Classic"),
		Tier2    UMETA(DisplayName="Nitro"),
		Tier3    UMETA(DisplayName="Nitro Basic"),
	};
}


USTRUCT(BlueprintType)
struct FDiscordUser
{
	GENERATED_BODY()

public:
	FDiscordUser() = default;
	
	explicit FDiscordUser(discord::User const& User)
	{
		ID = User.GetId();
		Username = User.GetUsername();
		DEPRECATED_Discriminator = User.GetDiscriminator();
		Avatar = User.GetAvatar();
		bIsBot = User.GetBot();
	}

	discord::User ToDiscordType() const
	{
		discord::User User;
		
		User.SetId(ID);
		User.SetUsername(TCHAR_TO_UTF8(*Username));
		User.SetDiscriminator(TCHAR_TO_UTF8(*DEPRECATED_Discriminator));
		User.SetAvatar(TCHAR_TO_UTF8(*Avatar));
		User.SetBot(bIsBot);
		
		return User;
	}

	/**
	 * The user's ID.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Discord|User")
	int64 ID = -1;

	/**
	 * Their username. This is NOT the display name.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Discord|User")
	FString Username = TEXT("INVALID_USER");

	/**
	 * The user's unique discriminator. This will only exist on "legacy" usernames,
	 * new usernames will have this as "0".
	 *
	 * See their help center article: https://support.discord.com/hc/en-us/articles/12620128861463-New-Usernames-Display-Names
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Discord|User", meta=(DisplayName="Discriminator", DeprecatedProperty, DeprecationMessage="Discord no longer uses discriminators. Some accounts may still have them though."))
	FString DEPRECATED_Discriminator = TEXT("INVALID_DISCRIMINATOR");

	/**
	 * The hash of the user's avatar. Not super useful on its own.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Discord|User")
	FString Avatar = TEXT("INVALID_AVATAR");

	/**
	 * Whether the user is a bot.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Discord|User")
	bool bIsBot = false;
};
