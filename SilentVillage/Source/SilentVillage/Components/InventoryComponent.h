#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SILENTVILLAGE_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /*
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<ABaseItem*> Items;

    void AddItem(ABaseItem* Item);
    */
    
};
