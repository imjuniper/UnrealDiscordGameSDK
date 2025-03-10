// Copyright Jakob Bouchard. All Rights Reserved.

#include "Overlay/DiscordOverlayManager.h"

#include "DiscordLatentAction.h"
#include "DiscordLogChannel.h"
#include "DiscordSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DiscordOverlayManager)


UDiscordOverlayManager::UDiscordOverlayManager()
{
	const auto Outer = GetOuter();
	if (Outer->IsA(UDiscordSubsystem::StaticClass()))
	{
		DiscordSubsystem = Cast<UDiscordSubsystem>(GetOuter());
	}
}

void UDiscordOverlayManager::Initialize(discord::OverlayManager* OverlayManager)
{
	Internal_OverlayManager = OverlayManager;

	Internal_OnToggleCallback = Internal_OverlayManager->OnToggle.Connect([&](const bool bLocked)
	{
		OnToggled.Broadcast(bLocked);
	});
}

void UDiscordOverlayManager::BeginDestroy()
{
	if (Internal_OverlayManager)
	{
		Internal_OverlayManager->OnToggle.Disconnect(Internal_OnToggleCallback);
	}
	
	UObject::BeginDestroy();
}

bool UDiscordOverlayManager::IsEnabled() const
{
	if (!DiscordSubsystem->IsActive()) return false;
	
	bool bReturnValue;
	Internal_OverlayManager->IsEnabled(&bReturnValue);
	return bReturnValue;
}

bool UDiscordOverlayManager::IsLocked() const
{
	if (!DiscordSubsystem->IsActive()) return false;
	
	bool bReturnValue;
	Internal_OverlayManager->IsLocked(&bReturnValue);
	return bReturnValue;
}

void UDiscordOverlayManager::SetLocked(const UObject* WorldContext, const FLatentActionInfo LatentInfo,
	const bool bLocked, EDiscordOutputPins& OutputPins)
{
	FDiscordLatentAction::CreateAndAdd(WorldContext, LatentInfo, OutputPins, [this, bLocked](auto* Action) mutable
	{
		SetLocked(bLocked, [Action](discord::Result Result) mutable
		{
			Action->FinishOperation(Result == discord::Result::Ok);
		});
	});
}

void UDiscordOverlayManager::SetLocked(const bool bLocked, TFunction<void(discord::Result)> Callback)
{
	if (!DiscordSubsystem->IsActive()) {
		Callback(discord::Result::InternalError);
		return;
	}
	
	Internal_OverlayManager->SetLocked(bLocked, Callback);
}

void UDiscordOverlayManager::OpenActivityInvite()
{
	Internal_OverlayManager->OpenActivityInvite(discord::ActivityActionType::Join, [](discord::Result Result)
	{
		// This callback apparently never fires
		if (Result != discord::Result::Ok)
		{
			LOG_DISCORD_ERROR(Result);
		}
	});
}

void UDiscordOverlayManager::OpenGuildInvite(const FString InviteCode)
{
	if (!DiscordSubsystem->IsActive()) return;
	
	Internal_OverlayManager->OpenGuildInvite(TCHAR_TO_UTF8(*InviteCode), [](discord::Result Result)
	{
		// This callback apparently never fires
		if (Result != discord::Result::Ok)
		{
			LOG_DISCORD_ERROR(Result);
		}
	});
}

void UDiscordOverlayManager::OpenVoiceSettings()
{
	if (!DiscordSubsystem->IsActive()) return;
	
	Internal_OverlayManager->OpenVoiceSettings([](discord::Result Result)
	{
		// This callback apparently never fires
		if (Result != discord::Result::Ok)
		{
			LOG_DISCORD_ERROR(Result);
		}
	});
}
