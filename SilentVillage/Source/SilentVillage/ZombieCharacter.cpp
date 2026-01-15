#include "ZombieCharacter.h"
#include "ZombieAIController.h"
#include "HealthComponentNew.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "RewardPickUp.h"



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

    if (Health && ActualDamage > 0.f && Health->CurrentHealth > 0.f)
    {
        Health->TakeDamageSimple(ActualDamage);
    }

    return ActualDamage;
}

void AZombieCharacter::HandleDeath()
{
	if (bIsDead) return;
	bIsDead = true;
    
    //Spawning Reward
    if (RewardClass)
    {
        const FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 40.f);

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ARewardPickUp* Spawned = GetWorld()->SpawnActor<ARewardPickUp>(RewardClass, SpawnLoc, FRotator::ZeroRotator, Params);

        UE_LOG(LogTemp, Warning, TEXT("Reward spawn attempt: %s"),
            Spawned ? TEXT("SUCCESS") : TEXT("FAILED"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("RewardClass is NULL on this zombie!"));
    }


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


void AZombieCharacter::StartAttack()
{
    if (bIsDead || !bCanAttack || !AttackMontage) return;

    UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInst) return;

    // if montage is already playing, does not restrat it
    if (AnimInst->Montage_IsPlaying(AttackMontage)) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
    if (Dist > AttackRange) return;

    bCanAttack = false;

    const float Duration = PlayAnimMontage(AttackMontage);
    if (Duration <= 0.f)
    {
        bCanAttack = true;
        return;
    }

    // Resetting when montage ends, then start colodown
    AnimInst->OnMontageEnded.RemoveDynamic(this, &AZombieCharacter::OnAttackMontageEnded);
    AnimInst->OnMontageEnded.AddDynamic(this, &AZombieCharacter::OnAttackMontageEnded);
}



void AZombieCharacter::ResetAttack()
{
    bIsAttacking = false;
}

void AZombieCharacter::DealDamage()
{
    if (bIsDead) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    // Check if player is still close enough at the hit frame
    const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
    if (Dist > AttackRange) return;

    // Optional: angle check so it only hits in front
    const FVector ToPlayer = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    const float FacingDot = FVector::DotProduct(GetActorForwardVector(), ToPlayer);
    if (FacingDot < 0.3f) return;

    // Apply damage to player
    UGameplayStatics::ApplyDamage(PlayerPawn, AttackDamage, GetController(), this, nullptr);

    //DrawDebugSphere(GetWorld(), GetActorLocation() + GetActorForwardVector()*80.f, AttackRadius, 16, FColor::Red, false, 0.2f);
}

void AZombieCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != AttackMontage) return;

    // Cooldown after animation ends (prevents the restart glitch)
    const float Cooldown = FMath::Max(AttackCooldown, 0.1f);

    GetWorldTimerManager().ClearTimer(AttackCooldownHandle);
    GetWorldTimerManager().SetTimer( AttackCooldownHandle,[this](){bCanAttack = true;}, Cooldown, false);
}


