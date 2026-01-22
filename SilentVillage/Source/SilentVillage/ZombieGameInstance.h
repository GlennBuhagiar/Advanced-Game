#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ZombieGameInstance.generated.h"

UENUM(BlueprintType)
enum class ELevelObjectiveType : uint8
{
	CollectItems,
	KillZombies
};

/**
 * 
 */
UCLASS()
class SILENTVILLAGE_API UZombieGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void SetObjectiveType(ELevelObjectiveType NewObjective);


	UPROPERTY(BlueprintReadWrite, Category = "Objective")
	ELevelObjectiveType CurrentObjective = ELevelObjectiveType::CollectItems;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 RequiredCollectibles = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress")
	int32 CollectedCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	FName NextLevelName = NAME_None;


	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	int32 ZombiesKilled = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Objective")
	int32 RequiredZombieKills = 20;



	UFUNCTION(BlueprintCallable, Category = "Progress")
	void AddCollectible(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Progress")
	int32 GetCollectedCount() const { return CollectedCount; }

	UFUNCTION(BlueprintCallable, Category = "Progress")
	int32 GetRequiredCollectibles() const { return RequiredCollectibles; }

	UFUNCTION(BlueprintCallable, Category = "Progress")
	bool HasEnoughCollectibles() const { return CollectedCount >= RequiredCollectibles; }

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void ResetCollectibles() { CollectedCount = 0; }

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void ResetProgress();

	UFUNCTION(BlueprintCallable)
	void AddZombieKill();

	UFUNCTION(BlueprintCallable)
	bool IsObjectiveComplete() const;



};
