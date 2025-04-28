// Copyright Juniper Bouchard. All Rights Reserved.

#pragma once

#include "DiscordSettings.h"
#include "DiscordLatentAction.generated.h"


UENUM()
enum class EDiscordOutputPins : uint8
{
	Then,
	Success,
	Failed
};

class FDiscordLatentAction final : public FPendingLatentAction
{
	const FLatentActionInfo LatentInfo;

	EDiscordOutputPins& Output;

	TFunction<void(FDiscordLatentAction*)> DiscordAction;

	float ElapsedTime = 0;
	
	float Timeout;

	bool bInitialized = false;
	
	bool bShouldFinish = false;

public:
	FDiscordLatentAction(const FLatentActionInfo& InLatentInfo, EDiscordOutputPins& InOutputPins, TFunction<void(FDiscordLatentAction*)> InDiscordAction)
		: LatentInfo(InLatentInfo), Output(InOutputPins), DiscordAction(InDiscordAction)
	{
		Timeout = GetDefault<UDiscordSettings>()->TimeoutSeconds;
		Output = EDiscordOutputPins::Then;
	}

	static void CreateAndAdd(const UObject* WorldContext, const FLatentActionInfo& LatentInfo, EDiscordOutputPins& OutputPins, TFunction<void(FDiscordLatentAction*)> DiscordAction);

	virtual void UpdateOperation(FLatentResponse& Response) override;

	void FinishOperation(const bool bSuccess);

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return FString::Printf(TEXT("Time spent: %.1fs | Timeout: %.1f"), ElapsedTime, Timeout);
	}
#endif
};