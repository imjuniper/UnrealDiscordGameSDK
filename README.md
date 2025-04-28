# Discord Integration for UE5

- Tested Unreal Engine versions: 5.4 & 5.5

> [!NOTE]
> Discord has now released their new [Social SDK](https://discord.com/developers/docs/discord-social-sdk/overview), with an official Unreal Engine plugin to go along with it. Currently, this new SDK requires manual authentication, since it doesn't communicate with the Discord app directly, which can be annoying in a lot of cases, so this plugin should still be useful to some who require minimal features, such as Rich Presence!

- [Discord Integration for UE5](#discord-integration-for-ue5)
- [Installation](#installation)
	- [Pre-built](#pre-built)
	- [Building from source](#building-from-source)
- [Usage](#usage)
	- [Discord Game SDK Settings (project settings)](#discord-game-sdk-settings-project-settings)
	- [Discord Subsystem (`UDiscordSubsystem`)](#discord-subsystem-udiscordsubsystem)
	- [Discord Activity Manager (`UDiscordActivityManager`)](#discord-activity-manager-udiscordactivitymanager)
		- [Discord Activity Join Request Reply Types (`EDiscordActivityJoinRequestReplyTypes`)](#discord-activity-join-request-reply-types-ediscordactivityjoinrequestreplytypes)
		- [Discord Activity Timestamps (`FDiscordActivityTimestamps`)](#discord-activity-timestamps-fdiscordactivitytimestamps)
		- [Discord Activity Assets (`FDiscordActivityAssets`)](#discord-activity-assets-fdiscordactivityassets)
		- [Discord Activity Party (`FDiscordActivityParty`)](#discord-activity-party-fdiscordactivityparty)
		- [Discord Activity Secrets (`FDiscordActivitySecrets`)](#discord-activity-secrets-fdiscordactivitysecrets)
		- [Discord Activity (`FDiscordActivity`)](#discord-activity-fdiscordactivity)
	- [Discord User Manager (`UDiscordUserManager`)](#discord-user-manager-udiscordusermanager)
		- [Discord User (`FDiscordUser`)](#discord-user-fdiscorduser)
		- [Discord User Flags (`EDiscordUserFlags`)](#discord-user-flags-ediscorduserflags)
		- [Discord Premium Types (`EDiscordPremiumTypes`)](#discord-premium-types-ediscordpremiumtypes)
	- [Discord Overlay Manager (`UDiscordOverlayManager`)](#discord-overlay-manager-udiscordoverlaymanager)

# Installation

## Pre-built

> [!NOTE]
> I only provide prebuilt binaries for Unreal 5.5 at this moment.

You can download the latest pre-built version in the Releases page.

## Building from source

Download this repo, put it in the `Plugins/` folder of your project. Ensure that you have the Discord Game SDK binaries downloaded. If you used the pre-built version before, they will be downloaded already. If not, you can run the `DownloadBinaries` script which will download them, and unzip them in the correct folders.

# Usage

The large majority of the functions match what's available directly from the SDK. See [Discord's documentation](https://discord.com/developers/docs/developer-tools/game-sdk#using-the-sdk) for more information.

## Discord Game SDK Settings (project settings)

**`Require Discord`**  
If checked, the game will force Discord to launch and relaunch itself. Does not work in-editor.

---
**`Client ID`**  
This can be obtained from Discord Developer Portal.

---
**`Timeout Seconds`**  
The time to wait before assuming a callback failed.

## Discord Subsystem (`UDiscordSubsystem`)

The **Discord Subsystem** is used to managed the Discord Client and create the three managers.

---
**`bool IsActive()`**  
Returns whether the subsystem is currently initialized. Will usually return false if the Client ID isn't set in settings, if the Discord SDK binaries are missing or if the Client failed to initialize.

---
**`UDiscordActivityManager* GetDiscordActivityManager()`**  
Returns the current instance of [Discord Activity Manager](#discord-activity-manager).

---
**`UDiscordUserManager* GetDiscordUserManager()`**  
Returns the current instance of [Discord User Manager](#discord-user-manager).

---
**`UDiscordOverlayManager* GetDiscordOverlayManager()`**  
Returns the current instance of [Discord Overlay Manager](#discord-overlay-manager).

## Discord Activity Manager (`UDiscordActivityManager`)

**`bool RegisterCommand(const FString Command)`**  
Returns whether the call was a success. Registers a command by which Discord can launch your game. This might be a custom protocol, like `my-awesome-game://`, or a path to an executable. It also supports any launch parameters that may be needed, like `game.exe --full-screen --no-hax`.

---
**`bool RegisterSteam(const int32 SteamAppID)`**  
Returns whether the call was a success. Used if you are distributing this SDK on Steam. Registers your game's Steam App ID for the protocol `steam://run-game-id/<id>`.

---
> [!NOTE]
> The Blueprint version is latent and doesn't return the error code, it just has pins for **Success** and **Failed**.

**`void UpdateActivity(const FDiscordActivity NewActivity, TFunction<void(discord::Result)> Callback)`**  
Get user information for a given id.

---
> [!WARNING]
> This probably won't work, see [issue 6612](https://github.com/discord/discord-api-docs/issues/6612) in the Discord API Docs.

> [!NOTE]
> The Blueprint version is latent and doesn't return the error code, it just has pins for **Success** and **Failed**.

**`void ClearActivity(TFunction<void(discord::Result)> Callback)`**  
Clear's a user's presence in Discord to make it show nothing.

---
> [!NOTE]
> The Blueprint version is latent and doesn't return the error code, it just has pins for **Success** and **Failed**.

**`void SendRequestReply(const int64 UserID, const EDiscordActivityJoinRequestReplyTypes::Type Reply, TFunction<void(discord::Result)> Callback)`**  
Sends a reply to an Ask to Join request.

---
> [!NOTE]
> The Blueprint version is latent and doesn't return the error code, it just has pins for **Success** and **Failed**.

**`void SendInvite(const int64 UserID, const FString Content, TFunction<void(discord::Result)> Callback)> Callback)`**  
Sends a game invite to a given user. If you do not have a valid activity with all the required fields, this call will error.

---
**`void AcceptInvite(const int64 UserID)`**  
Accepts a game invitation from a given User ID.

### Discord Activity Join Request Reply Types (`EDiscordActivityJoinRequestReplyTypes`)

Possible values:

- No
- Yes
- Ignore (note that this seem to do the same as **No**)

### Discord Activity Timestamps (`FDiscordActivityTimestamps`)

For C++ usage, has a converting constructor for the native Discord type, and can be converted to that type with `ToDiscordType()`.

---
**`int64 Start`**  
Unix timestamp. Send this to have an "elapsed" timer.

---
**`int64 End`**  
Unix timestamp. Send this to have a "remaining" timer.

### Discord Activity Assets (`FDiscordActivityAssets`)

For C++ usage, has a converting constructor for the native Discord type, and can be converted to that type with `ToDiscordType()`.

---
**`FString LargeImageKey`**  
Key for the large image..

---
**`FString LargeImageText`**  
Hover text for the large image.

---
**`FString SmallImageKey`**  
Key for the small image.

---
**`FString SmallImageText`**  
Hover text for the small image.

### Discord Activity Party (`FDiscordActivityParty`)

For C++ usage, has a converting constructor for the native Discord type, and can be converted to that type with `ToDiscordType()`.

---
**`FString ID`**  
A unique identifier for this party.

---
**`int32 CurrentSize`**  
The current size of the party. Setting to 0 means no party.

---
**`int32 MaxSize`**  
The max possible size of the party. Setting to 0 means no party.

### Discord Activity Secrets (`FDiscordActivitySecrets`)

For C++ usage, has a converting constructor for the native Discord type, and can be converted to that type with `ToDiscordType()`.

---
**`FString Match`**  
Unique hash for the given match context.

---
**`FString Join`**  
Unique hash for chat invites and Ask to Join.

### Discord Activity (`FDiscordActivity`)

For C++ usage, has a converting constructor for the native Discord type, and can be converted to that type with `ToDiscordType()`.

---
**`int64 ApplicationID` (read-only)**  
Your application ID.

---
**`FString Name` (read-only)**  
Name of the application.

---
**`FString State`**  
The player's current party status.

---
**`FString Details`**  
What the player is currently doing.

---
**`FDiscordActivityTimestamps Timestamps`**  
Helps create elapsed/remaining timestamps on a player's profile.

---
**`FDiscordActivityAssets Assets`**  
Assets to display on the player's profile.

---
**`FDiscordActivityParty Party`**  
Information about the player's party.

---
**`FDiscordActivitySecrets Secrets`**  
Secret passwords for joining the player's game.

---
**`bool bInstance`**  
Whether this activity is an instanced context, like a match.

## Discord User Manager (`UDiscordUserManager`)

For C++ usage, has a converting constructor for the native Discord type, and can be converted to that type with `ToDiscordType()`.

**`bool GetCurrentUser(FDiscordUser& User)`**  
Fetch information about the currently connected user account. Returns whether the call was a success.

---
**`OnCurrentUserUpdated(FDiscordUser User)` (delegate)**  
Fires when the `User` struct of the currently connected user changes. They may have changed their avatar, username, or something else.

---
> [!NOTE]
> The Blueprint version is latent and doesn't return the error code, it just has pins for **Success** and **Failed**.

**`void GetUser(const int64 UserID, TFunction<void(discord::Result, discord::User const&)> Callback)`**  
Get user information for a given User ID.

---
**`TEnumAsByte<EDiscordPremiumTypes::Type> GetCurrentUserPremiumType()`**  
Returns the [EDiscordPremiumTypes](#discord-premium-types-enum) of the current user.

---
**`bool CurrentUserHasFlag(EDiscordUserFlags Flag)`**  
Returns whether the current user has the flag.

### Discord User (`FDiscordUser`)

For C++ usage, has a converting constructor for the native Discord type, and can be converted to that type with `ToDiscordType()`.

---
**`int64 ID` (read-only)**  
The user's ID.

---
**`FString Username`**  
Their username. This is NOT the display name.

---
**`FString DEPRECATED_Discriminator`**  
The user's unique discriminator. This will only exist on "legacy" usernames, new usernames will have this as `0`. See their [help center article about this change](https://support.discord.com/hc/en-us/articles/12620128861463-New-Usernames-Display-Names).

---
**`FString Avatar`**  
The hash of the user's avatar. Not super useful on its own.

---
**`bool bIsBot`**  
Whether the user is a bot.

### Discord User Flags (`EDiscordUserFlags`)

Possible values:

- Partner (Discord Partner)
- HypeSquadEvents (HypeSquad Events participant)
- HypeSquadHouse1 (House Bravery)
- HypeSquadHouse2 (House Brilliance)
- HypeSquadHouse3 (House Balance)

### Discord Premium Types (`EDiscordPremiumTypes`)

Possible values:

- None (No Nitro)
- Tier 1 (Nitro Classic)
- Tier 2 (Nitro)
- Tier 3 (Nitro Basic)

## Discord Overlay Manager (`UDiscordOverlayManager`)

**`bool IsEnabled()`**  
Return whether the user has the overlay enabled or disabled. If the overlay is disabled, all the functionality in this manager will still work. The calls will instead focus the Discord client and show the modal there instead.

---
**`bool IsLocked()`**  
Return whether the overlay is currently locked or unlocked

---
**`OnToggled(bool bLocked)` (delegate)**  
Fires when the overlay is locked or unlocked (a.k.a. opened or closed).

---
> [!NOTE]
> The Blueprint version is latent and doesn't return the error code, it just has pins for **Success** and **Failed**.

**`void SetLocked(const bool bLocked, TFunction<void(discord::Result)> Callback)`**  
Locks or unlocks input in the overlay. Calling `SetLocked(true)` will also close any modals in the overlay.

---
**`void OpenActivityInvite()`**  
Opens the overlay modal for sending game invitations to users, channels, and servers. If you do not have a valid activity with all the required fields, this call will error.

---
**`void OpenGuildInvite(const FString InviteCode)`**  
Opens the overlay modal for joining a Discord guild, given its invite code.

---
**`void OpenVoiceSettings()`**  
Opens the overlay widget for voice settings for the currently connected application. These settings are unique to each user within the context of your application.
