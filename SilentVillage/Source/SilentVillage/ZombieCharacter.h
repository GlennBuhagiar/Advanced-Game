#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

class UHealthComponentNew;
class UAnimMontage;
class ARewardPickUp;

UCLASS()
class SILENTVILLAGE_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieCharacter();

	void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamage();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRadius = 120.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 180.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = .05f;

	UPROPERTY(EditAnywhere, Category = "Combat|Animation")
	UAnimMontage* AttackMontage = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Zombie", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, Category = "Zombie|Drops")
	TSubclassOf<ARewardPickUp> HealthPickupClass;

	UPROPERTY(EditAnywhere, Category = "Zombie|Drops")
	TSubclassOf<ARewardPickUp> AbilityPickupClass;

	UPROPERTY(EditAnywhere, Category = "Zombie|Drops")
	float AbilityDropChance = 0.35f; // 35% chance

	UPROPERTY(EditAnywhere, Category = "Zombie|Drops")
	TSubclassOf<ARewardPickUp> InvulnerabilityPickupClass;

	UPROPERTY(EditAnywhere, Category = "Zombie|Drops")
	TSubclassOf<ARewardPickUp> DoubleDamagePickupClass;

	UPROPERTY(EditAnywhere, Category = "Zombie|Drops")
	float InvulnerabilityDropChance = 0.25f; // 25%

	UPROPERTY(EditAnywhere, Category = "Zombie|Drops")
	float DoubleDamageDropChance = 0.20f; // 20%

	bool bCanAttack = true;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UHealthComponentNew* Health;

	bool bIsAttacking = false;
	FTimerHandle AttackCooldownHandle;

	void ResetAttack();

	UFUNCTION()
	void HandleDeath();
};
