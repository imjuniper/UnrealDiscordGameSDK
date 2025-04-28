// Copyright Juniper Bouchard. All Rights Reserved.

#include "DiscordSubsystem.h"

#include "DiscordLogChannel.h"
#include "DiscordRuntime.h"
#include "DiscordSettings.h"
#include "Activities/DiscordActivityManager.h"
#include "Overlay/DiscordOverlayManager.h"
#include "Users/DiscordUserManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DiscordSubsystem)


void PrintDiscordLog(discord::LogLevel Level, char const* Message)
{
	switch (Level)
	{
	case discord::LogLevel::Error:
		LOG_DISCORD(Error, "[SDK] {0}", Message);
		break;
	case discord::LogLevel::Warn:
		LOG_DISCORD(Warning, "[SDK] {0}", Message);
		break;
	case discord::LogLevel::Info:
		LOG_DISCORD(Display, "[SDK] {0}", Message);
		break;
	case discord::LogLevel::Debug:
		LOG_DISCORD(Log, "[SDK] {0}", Message);
		break;
	default:
		checkNoEntry();
	}
}

void UDiscordSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const auto* DiscordSettings = GetDefault<UDiscordSettings>();
	
	ActivityManager = NewObject<UDiscordActivityManager>(this);
	UserManager = NewObject<UDiscordUserManager>(this);
	OverlayManager = NewObject<UDiscordOverlayManager>(this);

	if (DiscordSettings->ClientID <= 0)
	{
		LOG_DISCORD(Warning, "Client ID missing. UDiscordSubsystem will not be initialized");
		return;
	}

	if (!FDiscordRuntimeModule::Get().IsSdkAvailable())
	{
		LOG_DISCORD(Error, "Discord Game SDK binaries are missing. UDiscordSubsystem will not be initialized");
		return;
	}

	discord::Core* CoreInstance = nullptr;

#if PLATFORM_DESKTOP && !WITH_EDITOR
	const auto CreateResult = discord::Core::Create(DiscordSettings->ClientID, DiscordSettings->bRequireDiscord ? DiscordCreateFlags_Default : DiscordCreateFlags_NoRequireDiscord, &CoreInstance);
#else
#if WITH_EDITOR
	if (DiscordSettings->bRequireDiscord)
	{
		LOG_DISCORD(Warning, "Discord is required, but we can't force-relaunch the editor. Please make sure Discord is open!");
	}
#endif
	const auto CreateResult = discord::Core::Create(DiscordSettings->ClientID, DiscordCreateFlags_NoRequireDiscord, &CoreInstance);
#endif

	if (CreateResult != discord::Result::Ok)
	{
		LOG_DISCORD(Error, "Failed to initialize Core");
		CoreInstance = nullptr;
		return;
	}
	
	Core = TUniquePtr<discord::Core>(CoreInstance);
	Core->SetLogHook(discord::LogLevel::Debug, PrintDiscordLog);
	
	ActivityManager->Initialize(&Core->ActivityManager());
	UserManager->Initialize(&Core->UserManager());
	OverlayManager->Initialize(&Core->OverlayManager());
	
	LOG_DISCORD(Log, "Initialized Core");

#if DISCORD_UE_VERSION >= 505
	SetTickableTickType(ETickableTickType::Always);
#endif
}

void UDiscordSubsystem::Tick(const float DeltaTime)
{
	if (Core.IsValid()) Core->RunCallbacks();
}

ETickableTickType UDiscordSubsystem::GetTickableTickType() const
{
#if DISCORD_UE_VERSION >= 505
	return ETickableTickType::Never;
#else
	// By default (if the child class doesn't override GetTickableTickType), don't let CDOs ever tick: 
	return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType(); 
#endif
}

#if DISCORD_UE_VERSION < 505
bool UDiscordSubsystem::IsAllowedToTick() const
{
	// No matter what IsTickable says, don't let CDOs or uninitialized world subsystems tick :
	// Note: even if GetTickableTickType was overridden by the child class and returns something else than ETickableTickType::Never for CDOs, 
	//  it's probably a mistake, so by default, don't allow ticking. If the child class really intends its CDO to tick, it can always override IsAllowedToTick...
	return !IsTemplate() && Core.IsValid();
}
#endif

bool UDiscordSubsystem::IsActive() const
{
	return Core.IsValid();
}

void UDiscordSubsystem::Deinitialize()
{
#if DISCORD_UE_VERSION >= 505
	SetTickableTickType(ETickableTickType::Never);
#endif

	if (Core.IsValid()) Core.Reset();
}
