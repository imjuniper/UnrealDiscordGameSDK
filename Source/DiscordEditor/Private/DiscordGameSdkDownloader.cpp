// Copyright Jakob Bouchard. All Rights Reserved.

#include "DiscordGameSdkDownloader.h"

#include "Logging/StructuredLog.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "miniz.h"


bool GDiscordGameSdkDownloaderRestartPending = false;

DEFINE_LOG_CATEGORY_STATIC(LogDiscordEditor, Log, All);
#define LOG_DISCORD_ED(Verbosity, Format, ...) UE_LOGFMT(LogDiscordEditor, Verbosity, Format __VA_OPT__(,) __VA_ARGS__)

#define LOCTEXT_NAMESPACE "DiscordEditor"

FDiscordGameSdkDownloader* FDiscordGameSdkDownloader::StartDownload()
{
	if (GDiscordGameSdkDownloaderRestartPending)
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString("Can't download: the Discord Game SDK is pending install, please restart the engine"));
		return nullptr;
	}

	const auto Downloader = new FDiscordGameSdkDownloader();
	Downloader->Start();
	return Downloader;
}

void FDiscordGameSdkDownloader::Start()
{
	Request = FHttpModule::Get().CreateRequest();
	Request->SetURL("https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip");
	Request->SetVerb(TEXT("GET"));

	ProgressWindow =
		SNew(SWindow)
		.Title(LOCTEXT("SdkDownloadWindow_Title", "Downloading Discord Game SDK"))
		.ClientSize(FVector2D(400, 100))
		.IsTopmostWindow(true);

	ProgressWindow->SetContent(
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("NoBorder"))
		.Padding(2)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(2)
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text_Lambda([&]
					{
						return FText::FormatOrdered(LOCTEXT("SdkDownloadWindow_Text", "{amount} MB received"), RequestProgress / float(1 << 20));
					})
				]
			]
		]
	);

	ProgressWindow->SetOnWindowClosed(FOnWindowClosed::CreateLambda([&](const TSharedRef<SWindow>&)
	{
		if (!Request)
		{
			return;
		}

		Request->CancelRequest();

		ensure(!bRequestCancelled);
		bRequestCancelled = true;

		LOG_DISCORD_ED(Log, "Discord Game SDK download cancelled");
	}));

	FSlateApplication::Get().AddWindow(ProgressWindow.ToSharedRef());

	Request->OnRequestProgress64().BindRaw(this, &FDiscordGameSdkDownloader::OnRequestProgress);
	Request->OnProcessRequestComplete().BindRaw(this, &FDiscordGameSdkDownloader::OnRequestComplete);
	Request->ProcessRequest();

	LOG_DISCORD_ED(Log, "Downloading Discord Game SDK from {0}", Request->GetURL());
}

void FDiscordGameSdkDownloader::Destroy(const FString& Reason)
{
	if (!Reason.IsEmpty())
	{
		FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Reason));
	}

	// Delay the deletion to be safe
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([&](float)
	{
		delete this;
		return false;
	}), 0.1f);
}

void FDiscordGameSdkDownloader::OnRequestProgress(FHttpRequestPtr HttpRequest, uint64 BytesSent, uint64 BytesReceived)
{
	ensure(HttpRequest == Request);
	RequestProgress = BytesReceived;
}

void FDiscordGameSdkDownloader::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	ensure(HttpRequest == Request);

	// Make sure OnWindowClosed exits early
	Request.Reset();

	check(ProgressWindow);
	// Make sure the dialog is on top
	ProgressWindow->Minimize();
	ProgressWindow->RequestDestroyWindow();
	ProgressWindow.Reset();

	if (bRequestCancelled)
	{
		return Destroy("Download cancelled");
	}

	if (!bSucceeded || !ensure(HttpResponse))
	{
		return Destroy("Query failed");
	}

	if (HttpResponse->GetResponseCode() == EHttpResponseCodes::NotFound)
	{
		return Destroy("The Discord Game SDK URL was not found.\nDiscord might have moved or deleted the SDK, please contact me at jakob@bchrd.ca");
	}
	
	if (HttpResponse->GetResponseCode() != EHttpResponseCodes::Ok)
	{
		return Destroy("Request failed: " + FString::FromInt(HttpResponse->GetResponseCode()) + "\n" + HttpResponse->GetContentAsString());
	}

	LOG_DISCORD_ED(Log, "Downloaded Discord Game SDK from {0}", HttpResponse->GetURL());

	TMap<FString, TArray<uint8>> Files;
	const FString ZipError = Unzip(HttpResponse->GetContent(), Files);
	if (!ZipError.IsEmpty())
	{
		return Destroy("Failed to unzip: " + ZipError);
	}

	// Filter files
	for (auto It = Files.CreateIterator(); It; ++It)
	{
		if (!It.Key().Contains("lib/x86_64") && !It.Key().Contains("lib/aarch64"))
		{
			LOG_DISCORD_ED(Log, "Skipping {0}: not in the lib/x86_64 or lib/aarch64 folder", It.Key());
			It.RemoveCurrent();
		}
		else if (It.Key().Contains(".bundle"))
		{
			LOG_DISCORD_ED(Log, "Skipping {0}: Only .dylib file is necessary", It.Key());
			It.RemoveCurrent();
		}
	}

	const FString InstallDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("Discord/Binaries/ThirdParty/DiscordGameSdk"));
	
	if (FPaths::DirectoryExists(InstallDir) && !IFileManager::Get().DeleteDirectory(*InstallDir, false, true))
	{
		return Destroy("Can't install Discord Game SDK into " + InstallDir + ": folder already exists and couldn't be deleted. Please delete it manually (you might need to close the editor).");
	}

	for (const auto& It : Files)
	{
		FString File;

		// macOS
		if (It.Key.EndsWith(".dylib"))
		{
			File = It.Key.Contains("aarch64")
				? It.Key.Replace(TEXT("lib/aarch64"), TEXT("Mac/arm64"))
				: It.Key.Replace(TEXT("lib/x86_64"), TEXT("Mac/x64"));
		}
		// Linux
		if (It.Key.EndsWith(".so"))
		{
			File = It.Key.Replace(TEXT("lib/x86_64"), TEXT("Linux"));
		}
		// Windows
		if (It.Key.EndsWith(".dll") || It.Key.EndsWith(".dll.lib"))
		{
			File = It.Key.Replace(TEXT("lib/x86_64"), TEXT("Win64"));
		}
		
		const FString TargetPath = InstallDir / File;
		if (!FFileHelper::SaveArrayToFile(It.Value, *TargetPath))
		{
			return Destroy("Failed to write " + TargetPath);
		}
	}
	
	ensure(!GDiscordGameSdkDownloaderRestartPending);
	GDiscordGameSdkDownloaderRestartPending = true;

	const TSharedPtr<SWindow> ActiveWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (ActiveWindow.IsValid())
	{
		ActiveWindow->HACK_ForceToFront();
	}

	if (FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString("Download successful. Do you want to restart to reload the SDK?")) == EAppReturnType::Yes)
	{
		FUnrealEdMisc::Get().RestartEditor(/*bWarn =*/false);
	}

	Destroy("");
}

FString FDiscordGameSdkDownloader::Unzip(const TArray<uint8>& Data, TMap<FString, TArray<uint8>>& OutFiles)
{
#define CheckZip(...) \
if ((__VA_ARGS__) != MZ_TRUE) \
{ \
return FString(mz_zip_get_error_string(mz_zip_peek_last_error(&Zip))); \
} \
{ \
const mz_zip_error Error = mz_zip_peek_last_error(&Zip); \
if (Error != MZ_ZIP_NO_ERROR) \
{ \
return FString(mz_zip_get_error_string(Error)); \
} \
}

#define CheckZipError() CheckZip(MZ_TRUE)

	mz_zip_archive Zip;
	mz_zip_zero_struct(&Zip);
	ON_SCOPE_EXIT
	{
		mz_zip_end(&Zip);
	};

	CheckZip(mz_zip_reader_init_mem(&Zip, Data.GetData(), Data.Num(), 0));

	const int32 NumFiles = mz_zip_reader_get_num_files(&Zip);

	for (int32 FileIndex = 0; FileIndex < NumFiles; FileIndex++)
	{
		const int32 FilenameSize = mz_zip_reader_get_filename(&Zip, FileIndex, nullptr, 0);
		CheckZipError();

		TArray<char> FilenameBuffer;
		FilenameBuffer.SetNumUninitialized(FilenameSize);
		mz_zip_reader_get_filename(&Zip, FileIndex, FilenameBuffer.GetData(), FilenameBuffer.Num());
		CheckZipError();

		// To be extra safe
		FilenameBuffer.Add(0);

		const FString Filename = FString(FilenameBuffer.GetData());
		if (Filename.EndsWith("/"))
		{
			continue;
		}

		mz_zip_archive_file_stat FileStat;
		CheckZip(mz_zip_reader_file_stat(&Zip, FileIndex, &FileStat));

		TArray<uint8> Buffer;
		Buffer.SetNumUninitialized(FileStat.m_uncomp_size);

		CheckZip(mz_zip_reader_extract_file_to_mem(&Zip, FilenameBuffer.GetData(), Buffer.GetData(), Buffer.Num(), 0));

		ensure(!OutFiles.Contains(Filename));
		OutFiles.Add(Filename, MoveTemp(Buffer));
	}

	return {};

#undef CheckZipError
#undef CheckZip
}

#undef LOCTEXT_NAMESPACE
