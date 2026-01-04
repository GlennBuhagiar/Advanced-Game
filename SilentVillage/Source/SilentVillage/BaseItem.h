#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

UCLASS()
class SILENTVILLAGE_API ABaseItem : public AActor
{
    GENERATED_BODY()

public:
    ABaseItem();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString ItemName;

    virtual void OnPickup(class ASilentVillageCharacter* Player);
};
