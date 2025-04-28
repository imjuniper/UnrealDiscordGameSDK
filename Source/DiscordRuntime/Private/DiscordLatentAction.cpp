// Copyright Juniper Bouchard. All Rights Reserved.

#include "DiscordLatentAction.h"

#include "DiscordLogChannel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DiscordLatentAction)


void FDiscordLatentAction::CreateAndAdd(const UObject* WorldContext, const FLatentActionInfo& LatentInfo,
                                        EDiscordOutputPins& OutputPins, TFunction<void(FDiscordLatentAction*)> DiscordAction)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::Assert);
	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	if (LatentActionManager.FindExistingAction<FDiscordLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID))
	{
		LOG_DISCORD(Warning, "Tried to re-execute ongoing action [{UUID}] in {Function}", LatentInfo.UUID, LatentInfo.ExecutionFunction);
		return;
	}

	FDiscordLatentAction* Action = new FDiscordLatentAction(LatentInfo, OutputPins, DiscordAction);
	LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
}

void FDiscordLatentAction::UpdateOperation(FLatentResponse& Response)
{
	ElapsedTime += Response.ElapsedTime();
	
	if (!bInitialized)
	{
		LOG_DISCORD(Log, "Started Discord latent action [{UUID}] timeout", LatentInfo.UUID);
		bInitialized = true;
		DiscordAction(this);
		Response.TriggerLink(LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
		return;
	}

	if (!bShouldFinish && ElapsedTime > Timeout)
	{
		bShouldFinish = true;
		Output = EDiscordOutputPins::Failed;
		LOG_DISCORD(Warning, "Discord latent action [{UUID}] timeout", LatentInfo.UUID);
	}
	
	Response.FinishAndTriggerIf(bShouldFinish, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
}

void FDiscordLatentAction::FinishOperation(const bool bSuccess)
{
	bShouldFinish = true;
	Output = bSuccess ? EDiscordOutputPins::Success : EDiscordOutputPins::Failed;
}
