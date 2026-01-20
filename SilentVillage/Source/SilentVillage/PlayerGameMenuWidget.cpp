#include "PlayerGameMenuWidget.h"
#include "Components/TextBlock.h"


void UPlayerGameMenuWidget::SetHealth(float Current, float Max)
{
    if (!HealthText) return;

    const FString Str = FString::Printf(TEXT("Health: %.0f / %.0f"), Current, Max);
    HealthText->SetText(FText::FromString(Str));
}

void UPlayerGameMenuWidget::SetObjectiveText(const FText& NewText)
{
    if (!ObjectiveText) return;
    ObjectiveText->SetText(NewText);
}

void UPlayerGameMenuWidget::SetCollectiblesText(int32 Current, int32 Required)
{
    if (!CollectiblesText)
    {
        UE_LOG(LogTemp, Warning, TEXT("CollectiblesText is NULL (BindWidget failed)"));
        return;
    }

    const FString Str = FString::Printf(TEXT("Collected %d / %d"), Current, Required);
    CollectiblesText->SetText(FText::FromString(Str));
}


