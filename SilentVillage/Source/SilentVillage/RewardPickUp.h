#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RewardPickUp.generated.h"

class UStaticMeshComponent;
class UPrimitiveComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class ERewardType : uint8
{
	Ammo,
	Health,
	Collectible,
    Ability
};

struct FHitResult;

UCLASS(Blueprintable)
class SILENTVILLAGE_API ARewardPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARewardPickUp();

protected:
    UPROPERTY(VisibleAnywhere)
    USphereComponent* Sphere;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, Category = "Reward")
    ERewardType RewardType = ERewardType::Collectible;

    UPROPERTY(EditAnywhere, Category = "Reward")
    int32 Amount = 5; //Health Amount

    UPROPERTY(EditAnywhere, Category = "Reward|Ability")
    float AbilityDuration = 4.3f;

    UPROPERTY(EditAnywhere, Category = "Reward|Ability")
    float SpeedMultiplier = 1.9f;


    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);
};
