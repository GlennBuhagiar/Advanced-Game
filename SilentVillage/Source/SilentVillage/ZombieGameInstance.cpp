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
