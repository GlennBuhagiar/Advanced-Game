#include "ZombieCharacter.h"
#include "HealthComponentNew.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AZombieCharacter::AZombieCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Health = CreateDefaultSubobject<UHealthComponentNew>(TEXT("HealthComponentNew"));
	Health->MaxHealth = 100.f;

}

// Called when the game starts or when spawned
void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind to delegate
	Health->OnDeath.AddDynamic(this, &AZombieCharacter::OnZombieDied);
	
}

float AZombieCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (Health && DamageAmount > 0.f && Health->CurrentHealth > 0.f)
    {
        Health->TakeDamageSimple(DamageAmount);
    }

    return ActualDamage;
}

void AZombieCharacter::OnZombieDied()
{
    Die();
}

void AZombieCharacter::Die()
{
    // Stopping movement nad disable collisions
    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SetLifeSpan(4.f);
}

