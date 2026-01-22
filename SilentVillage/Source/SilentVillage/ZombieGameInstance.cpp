#include "ZombieGameInstance.h"
#include "Kismet/GameplayStatics.h"


void UZombieGameInstance::AddCollectible(int32 Amount)
{
	CollectedCount += Amount;

	if (HasEnoughCollectibles() && NextLevelName != NAME_None)
	{
		UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
	}
}

void UZombieGameInstance::ResetProgress()
{
	CollectedCount = 0;
}

void UZombieGameInstance::AddZombieKill()
{
	ZombiesKilled++;
}

bool UZombieGameInstance::IsObjectiveComplete() const
{
    switch (CurrentObjective)
    {
    case ELevelObjectiveType::CollectItems:
        return CollectedCount >= RequiredCollectibles;

    case ELevelObjectiveType::KillZombies:
        return ZombiesKilled >= RequiredZombieKills;
    }

    return false;
}

void UZombieGameInstance::SetObjectiveType(ELevelObjectiveType NewObjective)
{
    CurrentObjective = NewObjective;

    // Reset relevant counters
    if (CurrentObjective == ELevelObjectiveType::KillZombies)
    {
        ZombiesKilled = 0;
    }
}

