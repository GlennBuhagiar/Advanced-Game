#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZombieCharacter.generated.h"

class UHealthComponentNew;


UCLASS()
class SILENTVILLAGE_API AZombieCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AZombieCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UHealthComponentNew* Health;

	UPROPERTY(BlueprintReadOnly, Category = "Zombie", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	UFUNCTION()
	void HandleDeath();
};
