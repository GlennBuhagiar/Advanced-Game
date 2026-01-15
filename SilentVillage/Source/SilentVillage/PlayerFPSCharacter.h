#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerFPSCharacter.generated.h"

class UCameraComponent;
class UHealthComponentNew;
class APlayerProjectile;

UCLASS()
class SILENTVILLAGE_API APlayerFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    APlayerFPSCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    void Fire();

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UCameraComponent* Camera;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UHealthComponentNew* Health;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<APlayerProjectile> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float MuzzleDistance = 120.f;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float FireCooldown = 0.2f;

    bool bCanFire = true;
    FTimerHandle FireCooldownHandle;

    void ResetFire();
};
