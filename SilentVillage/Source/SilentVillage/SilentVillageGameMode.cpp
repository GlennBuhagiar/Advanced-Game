#include "SilentVillageGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "ZombieGameInstance.h"

ASilentVillageGameMode::ASilentVillageGameMode()
{
	// stub
}

void ASilentVillageGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UZombieGameInstance* GI = GetGameInstance<UZombieGameInstance>())
	{
		const FString LevelName = GetWorld()->GetName();

		if (LevelName.Contains(TEXT("First")))
		{
			GI->CurrentObjective = ELevelObjectiveType::CollectItems;
		}
		else if (LevelName.Contains(TEXT("Lvl_Level2")))
		{
			GI->CurrentObjective = ELevelObjectiveType::KillZombies;
			GI->RequiredZombieKills = 20;
			GI->ZombiesKilled = 0;
		}
	}
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

