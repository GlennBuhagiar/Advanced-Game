#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SilentVillageGameMode.generated.h"

UCLASS()
class SILENTVILLAGE_API ASilentVillageGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASilentVillageGameMode();

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	int32 RequiredCollectibles = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	FName NextLevelName = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Progress")
	int32 CollectedCount = 0;

	UFUNCTION(BlueprintCallable, Category = "Progress")
	void AddCollectible(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Progress")
	bool HasEnoughCollectibles() const;

	UFUNCTION(BlueprintCallable, Category = "Progress")
	int32 GetCollectedCount() const { return CollectedCount; }

	UFUNCTION(BlueprintCallable, Category = "Progress")
	int32 GetRequiredCollectibles() const { return RequiredCollectibles; }

private:
	void CheckWin();
};


