$ScriptDir = Split-Path $MyInvocation.MyCommand.Path
Push-Location $ScriptDir

If (test-path -PathType container ".\Intermediate\discord-game-sdk")
{
	Remove-Item -Path ".\Intermediate\discord-game-sdk" -Recurse
}

If (!(test-path -PathType container "Intermediate"))
{
	New-Item -ItemType Directory -Path "Intermediate" | Out-Null
}

If (test-path -PathType container ".\Binaries\ThirdParty\DiscordGameSdk")
{
	Remove-Item -Path ".\Binaries\ThirdParty\DiscordGameSdk" -Recurse
	New-Item -ItemType Directory -Path ".\Binaries\ThirdParty\DiscordGameSdk" | Out-Null
}

echo "Downloading Discord Game SDK to .\Intermediate\discord-game-sdk.zip"
Invoke-WebRequest -Uri "https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip" -OutFile ".\Intermediate\discord-game-sdk.zip"

echo "Extracting Discord Game SDK to .\Intermediate\discord-game-sdk"
Expand-Archive -LiteralPath ".\Intermediate\discord-game-sdk.zip" -DestinationPath ".\Intermediate\discord-game-sdk"

$Files = Get-ChildItem ".\Intermediate\discord-game-sdk\lib" -Filter "discord_game_sdk.*" -Recurse

Foreach ($File in $Files) {
	$FilePath = $File.FullName
	$DirName = $File.Directory.BaseName.ToLower()
	If (($DirName -ne "x86_64") -and ($DirName -ne "aarch64")) {
        Continue
    }
	If ($File.Extension -eq ".bundle") {
        Continue
    }
	$DestinationPath = ""
	# macOS
	If ($File.Extension -eq ".dylib")
	{
		If ($DirName -eq "aarch64") {
			$DestinationPath = ".\Binaries\ThirdParty\DiscordGameSdk\Mac\arm64"
		}
		Else {
			$DestinationPath = ".\Binaries\ThirdParty\DiscordGameSdk\Mac\x64"
		}
	}
	# Linux
	If ($File.Extension -eq ".so")
	{
		$DestinationPath = ".\Binaries\ThirdParty\DiscordGameSdk\Linux"
	}
	# Windows
	If (($File.Extension -eq ".dll") -or ($File.Extension -eq ".lib"))
	{
		$DestinationPath = ".\Binaries\ThirdParty\DiscordGameSdk\Win64"
	}
	If (!(test-path -PathType container $DestinationPath))
	{
		New-Item -ItemType Directory -Path $DestinationPath | Out-Null
	}
	echo "Copying $File to $DestinationPath"
	Copy-Item -Path $FilePath -Destination $DestinationPath
}

Remove-Item -Path ".\Intermediate\discord-game-sdk.zip"
Remove-Item -Path ".\Intermediate\discord-game-sdk" -Recurse

Pop-Location
