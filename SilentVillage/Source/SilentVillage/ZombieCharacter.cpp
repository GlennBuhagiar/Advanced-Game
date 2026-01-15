#include "ZombieCharacter.h"
#include "ZombieAIController.h"
#include "HealthComponentNew.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"

// Sets default values
AZombieCharacter::AZombieCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Health = CreateDefaultSubobject<UHealthComponentNew>(TEXT("HealthComponentNew"));
	Health->MaxHealth = 100.f;

    AIControllerClass = AZombieAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


}

// Called when the game starts or when spawned
void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Bind to delegate
    if (Health)
    {
        Health->OnDeath.AddDynamic(this, &AZombieCharacter::HandleDeath);
    }
	
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

void AZombieCharacter::HandleDeath()
{
	if (bIsDead) return;
	bIsDead = true;

	// Stopping Zombie
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		if (AIC->BrainComponent)
		{
			AIC->BrainComponent->StopLogic(TEXT("Dead"));
		}
		AIC->StopMovement();
	}

	//To stop zombie from turning (removing all movement)
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// If AI is focusing the player, clear it
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
		AIC->SetFocus(nullptr);
	}

	GetCharacterMovement()->DisableMovement();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// AnimBP will handle death animatino
	SetLifeSpan(6.f);
}



