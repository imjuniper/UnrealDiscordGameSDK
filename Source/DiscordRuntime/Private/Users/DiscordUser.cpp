// Copyright Juniper Bouchard. All Rights Reserved.

#include "Users/DiscordUser.h"

#include "Discord/types.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DiscordUser)


FDiscordUser::FDiscordUser(discord::User const& User)
{
	ID = User.GetId();
	Username = User.GetUsername();
	DEPRECATED_Discriminator = User.GetDiscriminator();
	Avatar = User.GetAvatar();
	bIsBot = User.GetBot();
}

discord::User FDiscordUser::ToDiscordType() const
{
	discord::User User;
		
	User.SetId(ID);
	User.SetUsername(TCHAR_TO_UTF8(*Username));
	User.SetDiscriminator(TCHAR_TO_UTF8(*DEPRECATED_Discriminator));
	User.SetAvatar(TCHAR_TO_UTF8(*Avatar));
	User.SetBot(bIsBot);
		
	return User;
}
