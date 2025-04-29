// Copyright Juniper Bouchard. All Rights Reserved.

#include "Activities/DiscordActivityManager.h"

#include "DiscordLatentAction.h"
#include "DiscordLogChannel.h"
#include "DiscordSubsystem.h"
#include "Discord/activity_manager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DiscordActivityManager)


UDiscordActivityManager::UDiscordActivityManager()
{
	const auto Outer = GetOuter();
	if (Outer->IsA(UDiscordSubsystem::StaticClass()))
	{
		DiscordSubsystem = Cast<UDiscordSubsystem>(GetOuter());
	}
}

void UDiscordActivityManager::Initialize(discord::ActivityManager* ActivityManager)
{
	Internal_ActivityManager = ActivityManager;
	
	Internal_OnJoinCallback = Internal_ActivityManager->OnActivityJoin.Connect([&](const char* Secret)
	{
		OnActivityJoin.Broadcast(Secret);
	});
	
	Internal_OnJoinRequestCallback = Internal_ActivityManager->OnActivityJoinRequest.Connect([&](discord::User const& InviteUser)
	{
		OnActivityJoinRequest.Broadcast(FDiscordUser(InviteUser));
	});
	
	Internal_OnInviteCallback = Internal_ActivityManager->OnActivityInvite.Connect([&](discord::ActivityActionType InviteType,
		discord::User const& InviteUser, discord::Activity const& InviteActivity)
	{
		OnActivityInvite.Broadcast(FDiscordUser(InviteUser), FDiscordActivity(InviteActivity));
	});
}

void UDiscordActivityManager::BeginDestroy()
{
	if (Internal_ActivityManager)
	{
		Internal_ActivityManager->OnActivityJoin.Disconnect(Internal_OnJoinCallback);
		Internal_ActivityManager->OnActivityJoinRequest.Disconnect(Internal_OnJoinRequestCallback);
		Internal_ActivityManager->OnActivityInvite.Disconnect(Internal_OnInviteCallback);
	}
	
	UObject::BeginDestroy();
}

bool UDiscordActivityManager::RegisterCommand(const FString Command)
{
	if (!DiscordSubsystem->IsActive()) return false;
	
	return Internal_ActivityManager->RegisterCommand(TCHAR_TO_UTF8(*Command)) == discord::Result::Ok;
}

bool UDiscordActivityManager::RegisterSteam(const int32 SteamAppID)
{
	if (!DiscordSubsystem->IsActive()) return false;
	
	return Internal_ActivityManager->RegisterSteam(SteamAppID) == discord::Result::Ok;
}

void UDiscordActivityManager::UpdateActivity(const UObject* WorldContext, const FLatentActionInfo LatentInfo,
	const FDiscordActivity NewActivity, EDiscordOutputPins& OutputPins)
{
	FDiscordLatentAction::CreateAndAdd(WorldContext, LatentInfo, OutputPins, [this, NewActivity](auto* Action) mutable
	{
		UpdateActivity(NewActivity, [Action](discord::Result Result) mutable
		{
			Action->FinishOperation(Result == discord::Result::Ok);
		});
	});
}

void UDiscordActivityManager::UpdateActivity(const FDiscordActivity NewActivity, TFunction<void(discord::Result)> Callback)
{
	if (!DiscordSubsystem->IsActive()) {
		Callback(discord::Result::InternalError);
		return;
	}
	
	discord::Activity Activity = NewActivity.ToDiscordType();
	Internal_ActivityManager->UpdateActivity(Activity, Callback);
}

void UDiscordActivityManager::ClearActivity(const UObject* WorldContext, const FLatentActionInfo LatentInfo,
	EDiscordOutputPins& OutputPins)
{
	FDiscordLatentAction::CreateAndAdd(WorldContext, LatentInfo, OutputPins, [this](auto* Action) mutable
	{
		ClearActivity([Action](discord::Result Result) mutable
		{
			Action->FinishOperation(Result == discord::Result::Ok);
		});
	});
}

void UDiscordActivityManager::ClearActivity(TFunction<void(discord::Result)> Callback)
{
	if (!DiscordSubsystem->IsActive()) {
		Callback(discord::Result::InternalError);
		return;
	}
	
	LOG_DISCORD(Warning, "This probably won't work, see issue https://github.com/discord/discord-api-docs/issues/6612");
	Internal_ActivityManager->ClearActivity(Callback);
}

void UDiscordActivityManager::SendRequestReply(const UObject* WorldContext, const FLatentActionInfo LatentInfo,
	const int64 UserID, const EDiscordActivityJoinRequestReplyTypes::Type Reply, EDiscordOutputPins& OutputPins)
{
	FDiscordLatentAction::CreateAndAdd(WorldContext, LatentInfo, OutputPins, [this, UserID, Reply](auto* Action) mutable
	{
		SendRequestReply(UserID, Reply, [Action](discord::Result Result) mutable
		{
			Action->FinishOperation(Result == discord::Result::Ok);
		});
	});
}

void UDiscordActivityManager::SendRequestReply(const int64 UserID, const EDiscordActivityJoinRequestReplyTypes::Type Reply, TFunction<void(discord::Result)> Callback) const
{
	if (!DiscordSubsystem->IsActive()) {
		Callback(discord::Result::InternalError);
		return;
	}
	
	Internal_ActivityManager->SendRequestReply(UserID, static_cast<discord::ActivityJoinRequestReply>(Reply), Callback);
}

void UDiscordActivityManager::SendInvite(const UObject* WorldContext, const FLatentActionInfo LatentInfo,
	const int64 UserID, const FString Content, EDiscordOutputPins& OutputPins)
{
	FDiscordLatentAction::CreateAndAdd(WorldContext, LatentInfo, OutputPins, [this, UserID, Content](auto* Action) mutable
	{
		SendInvite(UserID, Content, [Action](discord::Result Result) mutable
		{
			Action->FinishOperation(Result == discord::Result::Ok);
		});
	});
}

void UDiscordActivityManager::SendInvite(const int64 UserID, const FString Content, TFunction<void(discord::Result)> Callback) const
{
	if (!DiscordSubsystem->IsActive()) {
		Callback(discord::Result::InternalError);
		return;
	}
	
	Internal_ActivityManager->SendInvite(UserID, discord::ActivityActionType::Join, TCHAR_TO_UTF8(*Content), Callback);
}

void UDiscordActivityManager::AcceptInvite(const int64 UserID) const
{
	if (!DiscordSubsystem->IsActive()) return;
	
	Internal_ActivityManager->AcceptInvite(UserID, [](discord::Result Result)
	{
		// This callback apparently never fires
		if (Result != discord::Result::Ok) {
			LOG_DISCORD_ERROR(Result);
		}
	});
}
