#include "SilentVillageGameMode.h"
#include "Kismet/GameplayStatics.h"

ASilentVillageGameMode::ASilentVillageGameMode()
{
	// stub
}

void ASilentVillageGameMode::AddCollectible(int32 Amount)
{
	CollectedCount += Amount;
	CheckWin();
}

bool ASilentVillageGameMode::HasEnoughCollectibles() const
{
	return CollectedCount >= RequiredCollectibles;
}

void ASilentVillageGameMode::CheckWin()
{
	if (!HasEnoughCollectibles())
		return;

	if (NextLevelName != NAME_None)
	{
		UGameplayStatics::OpenLevel(this, NextLevelName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NextLevelName is NONE. Set it in GameMode defaults."));
	}
}