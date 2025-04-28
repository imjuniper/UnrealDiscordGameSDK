// Copyright Juniper Bouchard. All Rights Reserved.

#include "Users/DiscordUserManager.h"

#include "DiscordLatentAction.h"
#include "DiscordLogChannel.h"
#include "DiscordSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DiscordUserManager)


UDiscordUserManager::UDiscordUserManager()
{
	const auto Outer = GetOuter();
	if (Outer->IsA(UDiscordSubsystem::StaticClass()))
	{
		DiscordSubsystem = Cast<UDiscordSubsystem>(GetOuter());
	}
}

void UDiscordUserManager::Initialize(discord::UserManager* UserManager)
{
	Internal_UserManager = UserManager;

	Internal_OnCurrentUserUpdateCallback = Internal_UserManager->OnCurrentUserUpdate.Connect([&]
	{
		FDiscordUser UpdatedUser;
		if (GetCurrentUser(UpdatedUser))
		{
			OnCurrentUserUpdated.Broadcast(UpdatedUser);
		}
	});
}

void UDiscordUserManager::BeginDestroy()
{
	if (Internal_UserManager)
	{
		Internal_UserManager->OnCurrentUserUpdate.Disconnect(Internal_OnCurrentUserUpdateCallback);
	}
	
	UObject::BeginDestroy();
}

bool UDiscordUserManager::GetCurrentUser(FDiscordUser& User) const
{
	if (!DiscordSubsystem->IsActive()) return false;
	
	discord::User DiscordUser;
	const auto Result = Internal_UserManager->GetCurrentUser(&DiscordUser);

	if (Result != discord::Result::Ok)
	{
		LOG_DISCORD_ERROR(Result);
		return false;
	}

	User = FDiscordUser(DiscordUser);
	return true;
}

void UDiscordUserManager::GetUser(const UObject* WorldContext, const FLatentActionInfo LatentInfo, const int64 UserID, FDiscordUser& User, EDiscordOutputPins& OutputPins) const
{
	FDiscordLatentAction::CreateAndAdd(WorldContext, LatentInfo, OutputPins, [this, UserID, &User](auto* Action) mutable
	{
		GetUser(UserID, [&User, Action](discord::Result Result, discord::User const& ResultUser) mutable
		{
			User = FDiscordUser(ResultUser);
			Action->FinishOperation(Result == discord::Result::Ok);
		});
	});
}

void UDiscordUserManager::GetUser(const int64 UserID, TFunction<void(discord::Result, discord::User const&)> Callback) const
{
	if (!DiscordSubsystem->IsActive())
	{
		Callback(discord::Result::InternalError, discord::User{});
		return;
	}
	
	Internal_UserManager->GetUser(UserID, Callback);
}

TEnumAsByte<EDiscordPremiumTypes::Type> UDiscordUserManager::GetCurrentUserPremiumType() const
{
	if (!DiscordSubsystem->IsActive()) return EDiscordPremiumTypes::None;
	
	discord::PremiumType CurrentUserPremiumType;
	const auto Result = Internal_UserManager->GetCurrentUserPremiumType(&CurrentUserPremiumType);

	if (Result != discord::Result::Ok)
	{
		LOG_DISCORD_ERROR(Result);
	}

	return static_cast<EDiscordPremiumTypes::Type>(CurrentUserPremiumType);
}

bool UDiscordUserManager::CurrentUserHasFlag(const EDiscordUserFlags::Type Flag) const
{
	if (!DiscordSubsystem->IsActive()) return false;
	
	if (Flag == EDiscordUserFlags::None) return false;

	bool bHasFlag = false;
	const auto Result = Internal_UserManager->CurrentUserHasFlag(static_cast<discord::UserFlag>(Flag << 1), &bHasFlag);

	if (Result != discord::Result::Ok)
	{
		LOG_DISCORD_ERROR(Result);
	}

	return bHasFlag;
}

