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

    if (UZombieGameInstance* GI = GetGameInstance<UZombieGameInstance>())
    {
        if (GI->CurrentObjective == ELevelObjectiveType::KillZombies)
        {
            GI->AddZombieKill();
        }
    }


	if (bIsDead) return;
	bIsDead = true;

    if (USkeletalMeshComponent* SkelMesh = GetMesh())
    {
        if (UAnimInstance* AnimInst = SkelMesh->GetAnimInstance())
        {
            AnimInst->StopAllMontages(0.0f); //Stopping all not just attack
        }
    }
    //Spawning Reward w/offset
    const FVector BaseLoc = GetActorLocation() + FVector(0, 0, 40.f);

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Always have a Health drop
    if (HealthPickupClass)
    {
        GetWorld()->SpawnActor<ARewardPickUp>(HealthPickupClass, BaseLoc + FVector(30.f, 0.f, 0.f), FRotator::ZeroRotator, Params);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthPickupClass is null on this zombie!"));
    }

    //Chance of spawning Ability drop
    if (AbilityPickupClass && FMath::FRand() <= AbilityDropChance)
    {
        GetWorld()->SpawnActor<ARewardPickUp>(AbilityPickupClass, BaseLoc + FVector(-30.f, 0.f, 0.f), FRotator::ZeroRotator, Params);
    }
    else if (!AbilityPickupClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilityPickupClass is null on this zombie!"));
    }

    // Chance of spawning Invulnerability ability
    if (InvulnerabilityPickupClass && FMath::FRand() <= InvulnerabilityDropChance)
    {
        GetWorld()->SpawnActor<ARewardPickUp>(
            InvulnerabilityPickupClass,
            BaseLoc + FVector(0.f, 30.f, 0.f),
            FRotator::ZeroRotator,
            Params
        );
    }

    // Chance of spawning Double Damage ability
    if (DoubleDamagePickupClass && FMath::FRand() <= DoubleDamageDropChance)
    {
        GetWorld()->SpawnActor<ARewardPickUp>(
            DoubleDamagePickupClass,
            BaseLoc + FVector(30.f, 0.f, 0.f),
            FRotator::ZeroRotator,
            Params
        );
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

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    APlayerFPSCharacter* Player = Cast<APlayerFPSCharacter>(PlayerPawn);

    


    UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
    if (!AnimInst) return;

    // if montage is already playing, does not restrat it
    if (AnimInst->Montage_IsPlaying(AttackMontage)) return;

    
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
    
       
    UE_LOG(LogTemp, Display, TEXT("Zombie DealDamage() CALLED"));


    if (bIsDead) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!PlayerPawn) return;

    APlayerFPSCharacter* Player = Cast<APlayerFPSCharacter>(PlayerPawn);
    if (Player && Player->IsInvulnerable())
    {
        UE_LOG(LogTemp, Display, TEXT("Zombie ignored attack (player invisible)"));
        return;
    }

    // Check if player is still close enough at the hit frame
    const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
    if (Dist > AttackRange) return;

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


