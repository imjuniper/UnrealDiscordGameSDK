// Copyright Jakob Bouchard. All Rights Reserved.

#pragma once

#include "Interfaces/IHttpRequest.h"


class FDiscordGameSdkDownloader
{
public:
	static FDiscordGameSdkDownloader* StartDownload();
	
private:
	void Start();
	void Destroy(const FString& Reason);
	
	FHttpRequestPtr Request;
	TSharedPtr<SWindow> ProgressWindow;

	uint64 RequestProgress = 0;
	bool bRequestCancelled = false;

	void OnRequestProgress(FHttpRequestPtr HttpRequest, uint64 BytesSent, uint64 BytesReceived);
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString Unzip(const TArray<uint8>& Data, TMap<FString, TArray<uint8>>& OutFiles);
};
