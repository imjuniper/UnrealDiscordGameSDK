// Copyright Jakob Bouchard. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"


class FDiscordRuntimeModule final : public IModuleInterface
{
private:
	void* DiscordGameSdkDllHandle = nullptr;
	
public:
	static FDiscordRuntimeModule& Get();
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	bool IsSdkAvailable() const;
};