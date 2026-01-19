#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerFPSCharacter.generated.h"

class UCameraComponent;
class UHealthComponentNew;
class UPlayerGameMenuWidget;
class APlayerProjectile;

UCLASS()
class SILENTVILLAGE_API APlayerFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    APlayerFPSCharacter();

    UFUNCTION(BlueprintCallable, Category = "Ability")
    void ActivateSpeedBoost(float Multiplier, float Duration);

    UFUNCTION(BlueprintCallable, Category = "Ability")
    FText GetActiveAbilityText() const;

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHealthComponentNew* Health;

    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<APlayerProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float MuzzleDistance = 100.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireCooldown = 0.2f;

    bool bCanFire = true;
    FTimerHandle FireCooldownHandle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UPlayerGameMenuWidget> MenuWidgetClass;

    UPROPERTY()
    UPlayerGameMenuWidget* MenuWidget = nullptr;

    float BaseWalkSpeed = 0.f;

    bool bSpeedBoostActive = false;
    float SpeedBoostMultiplier = 1.0f;

    FTimerHandle AbilityTimerHandle;

private:

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    void Fire();
    void ResetFire();

    void ToggleMenu();
    void UpdateMenuUI();

    void EndSpeedBoost();
};