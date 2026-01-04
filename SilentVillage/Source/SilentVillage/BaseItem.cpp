#include "BaseItem.h"
#include "SilentVillageCharacter.h"

ABaseItem::ABaseItem()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABaseItem::OnPickup(ASilentVillageCharacter* Player)
{
    Destroy();
}
