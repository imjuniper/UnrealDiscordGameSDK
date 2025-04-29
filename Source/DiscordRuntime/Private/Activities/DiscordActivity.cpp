// Copyright Juniper Bouchard. All Rights Reserved.

#include "Activities/DiscordActivity.h"

#include "Discord/types.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DiscordActivity)


FDiscordActivityTimestamps::FDiscordActivityTimestamps(discord::ActivityTimestamps const& Timestamps)
{
	Start = Timestamps.GetStart();
	End = Timestamps.GetEnd();
}

discord::ActivityTimestamps FDiscordActivityTimestamps::ToDiscordType() const
{
	discord::ActivityTimestamps Timestamps;
		
	Timestamps.SetStart(Start);
	Timestamps.SetEnd(End);
		
	return Timestamps;
}

FDiscordActivityAssets::FDiscordActivityAssets(discord::ActivityAssets const& Assets)
{
	LargeImageKey = Assets.GetLargeImage();
	LargeImageText = Assets.GetLargeText();
	SmallImageKey = Assets.GetSmallImage();
	SmallImageText = Assets.GetSmallText();
}

discord::ActivityAssets FDiscordActivityAssets::ToDiscordType() const
{
	discord::ActivityAssets Assets;
		
	Assets.SetLargeImage(TCHAR_TO_UTF8(*LargeImageKey));
	Assets.SetLargeText(TCHAR_TO_UTF8(*LargeImageText));
	Assets.SetSmallImage(TCHAR_TO_UTF8(*SmallImageKey));
	Assets.SetSmallText(TCHAR_TO_UTF8(*SmallImageText));
		
	return Assets;
}

FDiscordActivityParty::FDiscordActivityParty(discord::ActivityParty const& Party)
{
	ID = Party.GetId();
	CurrentSize = Party.GetSize().GetCurrentSize();
	MaxSize = Party.GetSize().GetMaxSize();
}

discord::ActivityParty FDiscordActivityParty::ToDiscordType() const
{
	discord::ActivityParty Party;
		
	Party.SetId(TCHAR_TO_UTF8(*ID));
	Party.GetSize().SetCurrentSize(CurrentSize);
	Party.GetSize().SetMaxSize(MaxSize);
		
	return Party;
}

FDiscordActivitySecrets::FDiscordActivitySecrets(discord::ActivitySecrets const& Secrets)
{
	Match = Secrets.GetMatch();
	Join = Secrets.GetJoin();
}

discord::ActivitySecrets FDiscordActivitySecrets::ToDiscordType() const
{
	discord::ActivitySecrets Secrets;
		
	Secrets.SetMatch(TCHAR_TO_UTF8(*Match));
	Secrets.SetJoin(TCHAR_TO_UTF8(*Join));
		
	return Secrets;
}

FDiscordActivity::FDiscordActivity(discord::Activity const& Activity)
{
	ApplicationID = Activity.GetApplicationId();
	Name = Activity.GetName();
	State = Activity.GetState();
	Details = Activity.GetDetails();
	bInstance = Activity.GetInstance();
	Timestamps = FDiscordActivityTimestamps(Activity.GetTimestamps());
	Assets = FDiscordActivityAssets(Activity.GetAssets());
	Party = FDiscordActivityParty(Activity.GetParty());
	Secrets = FDiscordActivitySecrets(Activity.GetSecrets());
}

discord::Activity FDiscordActivity::ToDiscordType() const
{
	discord::Activity Activity{};

	if (ApplicationID != -1)
	{
		Activity.SetApplicationId(ApplicationID);
		Activity.SetName(TCHAR_TO_UTF8(*Name));
	}
	Activity.SetState(TCHAR_TO_UTF8(*State));
	Activity.SetDetails(TCHAR_TO_UTF8(*Details));
	Activity.SetInstance(bInstance);

	// Activity Timestamps
	Activity.GetTimestamps().SetStart(Timestamps.Start);
	Activity.GetTimestamps().SetEnd(Timestamps.End);
	
	// Activity Assets
	Activity.GetAssets().SetLargeImage(TCHAR_TO_UTF8(*Assets.LargeImageKey));
	Activity.GetAssets().SetLargeText(TCHAR_TO_UTF8(*Assets.LargeImageText));
	Activity.GetAssets().SetSmallImage(TCHAR_TO_UTF8(*Assets.SmallImageKey));
	Activity.GetAssets().SetSmallText(TCHAR_TO_UTF8(*Assets.SmallImageText));
	
	// Activity Party
	Activity.GetParty().SetId(TCHAR_TO_UTF8(*Party.ID));
	Activity.GetParty().GetSize().SetCurrentSize(Party.CurrentSize);
	Activity.GetParty().GetSize().SetMaxSize(Party.MaxSize);
	
	// Activity Secrets
	Activity.GetSecrets().SetMatch(TCHAR_TO_UTF8(*Secrets.Match));
	Activity.GetSecrets().SetJoin(TCHAR_TO_UTF8(*Secrets.Join));
		
	return Activity;
}
