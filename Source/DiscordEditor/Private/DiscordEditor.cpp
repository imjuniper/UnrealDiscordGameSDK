// Copyright Juniper Bouchard. All Rights Reserved.

#include "Modules/ModuleInterface.h"

#include "DiscordGameSdkDownloader.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Notifications/SNotificationList.h"


#define LOCTEXT_NAMESPACE "DiscordEditor"

class FDiscordEditorModule final : public IModuleInterface
{
private:
	TWeakPtr<SNotificationItem> WeakNotification;

	FDiscordGameSdkDownloader* Downloader = nullptr;
	
public:
	void OnMainFrameLoaded(TSharedPtr<SWindow> Window, bool bArg)
	{
		FNotificationInfo Info(LOCTEXT("SdkMissingNotification_Title", "Discord Game SDK is missing"));
		Info.SubText = LOCTEXT("SdkMissingNotification_SubText", "Until the Discord Game SDK is downloaded, the integrations will not be functional.");
		Info.ButtonDetails.Add(FNotificationButtonInfo(
			LOCTEXT("SdkMissingNotification_Button", "Download"),
			LOCTEXT("SdkMissingNotification_ButtonTooltip", "Download the SDK from Discord"),
			FSimpleDelegate::CreateLambda([&]
			{
				TSharedPtr<SNotificationItem> Notification = WeakNotification.Pin();
				if (Notification.IsValid())
				{
					Notification->SetExpireDuration(0.0f);
					Notification->SetFadeOutDuration(0.5f);
					Notification->ExpireAndFadeout();
				}
				
				if (Downloader)
				{
					FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Can't download: the Discord Game SDK is already downloading"));
					return;
				}
				
				Downloader = FDiscordGameSdkDownloader::StartDownload();
			}),
			SNotificationItem::CS_Fail));
		Info.ButtonDetails.Add(FNotificationButtonInfo(
			LOCTEXT("SdkMissingNotification_Button", "Ignore for now"),
			LOCTEXT("SdkMissingNotification_ButtonTooltip", "Will remind you on next editor startup"),
			FSimpleDelegate::CreateLambda([&]
			{
				TSharedPtr<SNotificationItem> Notification = WeakNotification.Pin();
				if (Notification.IsValid())
				{
					Notification->SetExpireDuration(0.0f);
					Notification->SetFadeOutDuration(0.5f);
					Notification->ExpireAndFadeout();
				}
			}),
			SNotificationItem::CS_Fail));
		Info.bUseSuccessFailIcons = true;
		Info.WidthOverride = 300.0f;
		Info.bFireAndForget = false;

		const TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
		if (!Notification)
		{
			return;
		}

		Notification->SetCompletionState(SNotificationItem::CS_Fail);
		WeakNotification = Notification;
	}

	virtual void StartupModule() override
	{
		FString PathToDll = FPaths::ProjectPluginsDir() / TEXT("Discord/Binaries/ThirdParty/DiscordGameSdk") / FPlatformProcess::GetBinariesSubdirectory();
		const FString DllName = FString::Printf(TEXT("discord_game_sdk.%s"), FPlatformProcess::GetModuleExtension());

#if PLATFORM_MAC
#if PLATFORM_CPU_ARM_FAMILY
		PathToDll = FPaths::Combine(*PathToDll, TEXT("arm64"));
#else
		PathToDll = FPaths::Combine(*PathToDll, TEXT("x64"));
#endif
#endif

		if (!FPaths::FileExists(FPaths::Combine(*PathToDll, *DllName)))
		{
			IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
			MainFrameModule.OnMainFrameCreationFinished().AddRaw(this, &FDiscordEditorModule::OnMainFrameLoaded);
		}
	}
};

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDiscordEditorModule, DiscordEditor)