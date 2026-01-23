#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerGameMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SILENTVILLAGE_API UPlayerGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetHealth(float Current, float Max);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetObjectiveText(const FText& NewText);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetProgressText(int32 Current, int32 Required, const FString& Label);

    

protected:

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* HealthText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ObjectiveText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CollectiblesText;
};
