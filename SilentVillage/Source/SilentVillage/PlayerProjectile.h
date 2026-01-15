#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class SILENTVILLAGE_API APlayerProjectile : public AActor
{
	GENERATED_BODY()
	
public:
    APlayerProjectile();

    // Damage dealt on hit
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float Damage = 25.f;

    // Auto-destroy after time
    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float LifeSeconds = 4.f;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere)
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovement;

    UFUNCTION()
    void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
