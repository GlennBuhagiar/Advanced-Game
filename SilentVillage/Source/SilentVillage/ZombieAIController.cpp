#include "ZombieAIController.h"
#include "BrainComponent.h"
#include "AIPatrolPoint.h"
#include "ZombieCharacter.h"
#include "PlayerFPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

AZombieAIController::AZombieAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ControlledPawn = InPawn;
    PlayerChar = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    GatherPatrolPoints();
    SetState(EZombieState::Patrol);
}

void AZombieAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!PlayerChar)
    {
        PlayerChar = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    }

    
    if (APlayerFPSCharacter* Player = Cast<APlayerFPSCharacter>(PlayerChar))
    {
        if (Player->IsInvulnerable())
        {
            StopMovement();

            if (BrainComponent)
            {
                BrainComponent->StopLogic(TEXT("Player Invulnerable"));
            }

            return; 
        }
        else
        {
            
            if (BrainComponent && BrainComponent->IsPaused())
            {
                BrainComponent->RestartLogic();
            }
        }
    }

    const float D = DistToPlayer();

    APlayerFPSCharacter* Player = Cast<APlayerFPSCharacter>(PlayerChar);
    if (Player && Player->IsInvulnerable())
    {
        SetState(EZombieState::Patrol);
        StopMovement();
        return;
    }


    if (State == EZombieState::Patrol)
    {
        if (D <= DetectRange && CanSeePlayer())
        {
            SetState(EZombieState::Chase);
        }
    }
    else if (State == EZombieState::Chase)
    {
        if (D <= AttackEnterRange)
        {
            SetState(EZombieState::Attack);
        }
        else if (D > LoseRange)
        {
            SetState(EZombieState::Patrol);
        }
    }
    else if (State == EZombieState::Attack)
    {
        if (D > AttackExitRange)
        {
            SetState(EZombieState::Chase);
        }
        else if (D > LoseRange)
        {
            SetState(EZombieState::Patrol);
        }
    }

    if (State == EZombieState::Patrol) DoPatrol();
    else if (State == EZombieState::Chase) DoChase();
    else if (State == EZombieState::Attack) DoAttack();
}

void AZombieAIController::GatherPatrolPoints()
{
    PatrolPoints.Reset();

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIPatrolPoint::StaticClass(), Found);

    for (AActor* A : Found)
    {
        if (AAIPatrolPoint* P = Cast<AAIPatrolPoint>(A))
        {
            PatrolPoints.Add(P);
        }
    }

    PatrolIndex = (PatrolPoints.Num() > 0) ? FMath::RandRange(0, PatrolPoints.Num() - 1) : 0;
    bHasPatrolMove = false;
}

void AZombieAIController::SetState(EZombieState NewState)
{
    if (State == NewState) return;

    State = NewState;

    // Set spedes
    if (ACharacter* Z = Cast<ACharacter>(ControlledPawn))
    {
        if (UCharacterMovementComponent* Move = Z->GetCharacterMovement())
        {
            if (NewState == EZombieState::Patrol) Move->MaxWalkSpeed = PatrolSpeed;
            if (NewState == EZombieState::Chase)  Move->MaxWalkSpeed = ChaseSpeed;
            if (NewState == EZombieState::Attack) Move->MaxWalkSpeed = AttackSpeed;
        }
    }

    // Reset patrol move gaet when returning to patrol
    if (NewState == EZombieState::Patrol)
    {
        bHasPatrolMove = false;
    }
}

void AZombieAIController::DoPatrol()
{
    if (PatrolPoints.Num() == 0) return;
    if (bHasPatrolMove) return;

    AAIPatrolPoint* Target = PatrolPoints.IsValidIndex(PatrolIndex) ? PatrolPoints[PatrolIndex] : nullptr;
    if (!Target) return;

    MoveToActor(Target, PatrolAcceptanceRadius);
    bHasPatrolMove = true;
}

void AZombieAIController::DoChase()
{
    // Chase close enough so attack tirggers consistently
    MoveToActor(PlayerChar, 30.f);
}

void AZombieAIController::DoAttack()
{
    // Stops sliding while attacking
    StopMovement();

    // Asks zombie to attack; zombie handles cooldown + montgae + notify-damage
    if (AZombieCharacter* Z = Cast<AZombieCharacter>(ControlledPawn))
    {
        Z->StartAttack();
    }
}

float AZombieAIController::DistToPlayer() const
{
    if (!PlayerChar || !ControlledPawn) return FLT_MAX;
    return FVector::Dist(PlayerChar->GetActorLocation(), ControlledPawn->GetActorLocation());
}

bool AZombieAIController::CanSeePlayer() const
{
    if (!PlayerChar || !ControlledPawn) return false;

    FHitResult Hit;
    FVector Start = ControlledPawn->GetActorLocation() + FVector(0, 0, 50);
    FVector End = PlayerChar->GetActorLocation() + FVector(0, 0, 50);

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(ControlledPawn);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

    return (!bHit) || (Hit.GetActor() == PlayerChar);
}

void AZombieAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    if (State != EZombieState::Patrol) return;

    bHasPatrolMove = false;

    if (PatrolPoints.Num() > 0)
    {
        PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
    }
}
