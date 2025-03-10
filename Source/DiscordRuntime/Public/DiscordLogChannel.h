// Copyright Jakob Bouchard. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"
#include "Logging/StructuredLog.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDiscord, Log, All);

#define LOG_DISCORD(Verbosity, Format, ...) UE_LOGFMT(LogDiscord, Verbosity, Format __VA_OPT__(,) __VA_ARGS__)

#define LOG_DISCORD_ERROR(Result) \
LOG_DISCORD(Error, "{Function} failed with error code {Code}", __FUNCTION__, static_cast<int32>(Result))
