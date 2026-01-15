#include "ZombieAIController.h"
#include "AIPatrolPoint.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "NavigationSystem.h"

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
        PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    }

    if (!PlayerChar || !ControlledPawn) return;

    const float D = DistToPlayer();

    // State switching
    if (State == EZombieState::Patrol)
    {
        if (D <= DetectRange && CanSeePlayer())
            SetState(EZombieState::Chase);
    }
    else if (State == EZombieState::Chase)
    {
        if (D <= AttackRange)
            SetState(EZombieState::Attack);
        else if (D > LoseRange)
            SetState(EZombieState::Patrol);
    }
    else if (State == EZombieState::Attack)
    {
        if (D > AttackRange)
            SetState(EZombieState::Chase);
        else if (D > LoseRange)
            SetState(EZombieState::Patrol);
    }

    // State actions
    if (State == EZombieState::Patrol) DoPatrol();
    else if (State == EZombieState::Chase) DoChase();
}

void AZombieAIController::GatherPatrolPoints()
{
    PatrolPoints.Reset();

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIPatrolPoint::StaticClass(), Found);
    for (AActor* A : Found)
    {
        if (AAIPatrolPoint* P = Cast<AAIPatrolPoint>(A))
            PatrolPoints.Add(P);
    }

    // Randomising so the zmobies don't all walk same route
    PatrolPoints.Sort([](const AAIPatrolPoint& A, const AAIPatrolPoint& B)
        {
            return A.GetName() < B.GetName();
        });

    PatrolIndex = 0;
}

void AZombieAIController::SetState(EZombieState NewState)
{
    if (State == NewState) return;

    // Exit old state
    if (State == EZombieState::Attack)
        StopAttack();

    State = NewState;

    // Entre new state
    if (State == EZombieState::Attack)
        StartAttack();

    if (ACharacter* Z = Cast<ACharacter>(ControlledPawn))
    {
        if (UCharacterMovementComponent* Move = Z->GetCharacterMovement())
        {
            if (NewState == EZombieState::Patrol) Move->MaxWalkSpeed = PatrolSpeed;
            if (NewState == EZombieState::Chase)  Move->MaxWalkSpeed = ChaseSpeed;
            if (NewState == EZombieState::Attack) Move->MaxWalkSpeed = 0.f; // optional
        }
    }

    if (NewState == EZombieState::Patrol)
    {
        bHasPatrolMove = false; // allow a new MoveTo
    }
}

void AZombieAIController::DoPatrol()
{
    if (PatrolPoints.Num() == 0) return;

    if (bHasPatrolMove) return; // to not spam MoveTo every tick

    AAIPatrolPoint* Target = PatrolPoints.IsValidIndex(PatrolIndex) ? PatrolPoints[PatrolIndex] : nullptr;
    if (!Target) return;

    MoveToActor(Target, PatrolAcceptanceRadius);
    bHasPatrolMove = true;
}

void AZombieAIController::DoChase()
{
    MoveToActor(PlayerChar, AttackRange - 20.f);
}

void AZombieAIController::StartAttack()
{
    StopMovement();
    GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AZombieAIController::AttackTick, AttackInterval, true, 0.f);
}

void AZombieAIController::StopAttack()
{
    GetWorld()->GetTimerManager().ClearTimer(AttackTimer);
}

void AZombieAIController::AttackTick()
{
    if (!PlayerChar || !ControlledPawn) return;

    const float D = DistToPlayer();
    if (D > AttackRange) return;

    UGameplayStatics::ApplyDamage(PlayerChar, AttackDamage, this, ControlledPawn, UDamageType::StaticClass());
}

float AZombieAIController::DistToPlayer() const
{
    if (!PlayerChar || !ControlledPawn) return FLT_MAX; //basically NotNullReference
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

    // If we didn't hit anything, or we hit the player, we can "see" them
    return (!bHit) || (Hit.GetActor() == PlayerChar);
}

void AZombieAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    if (State != EZombieState::Patrol) return;

    bHasPatrolMove = false;

    PatrolIndex = (PatrolIndex + 1) % PatrolPoints.Num();
}
