#include "InventoryComponent.h"
#include "BaseItem.h"

void UInventoryComponent::AddItem(ABaseItem* Item)
{
    if (Item)
    {
        Items.Add(Item);
    }
}
