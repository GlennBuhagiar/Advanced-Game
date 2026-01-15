#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "ZombieAIController.generated.h"

class AAIPatrolPoint;

UENUM()
enum class EZombieState : uint8
{
    Patrol,
    Chase,
    Attack
};

UCLASS()
class SILENTVILLAGE_API AZombieAIController : public AAIController
{
    GENERATED_BODY()

public:
    AZombieAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
    // Patrol
    UPROPERTY(EditAnywhere, Category = "AI|Patrol")
    float PatrolAcceptanceRadius = 80.f;

    // Sensing
    UPROPERTY(EditAnywhere, Category = "AI|Sense")
    float DetectRange = 1200.f;

    UPROPERTY(EditAnywhere, Category = "AI|Sense")
    float LoseRange = 2200.f;

    // Combat
    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float AttackEnterRange = 180.f;

    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float AttackExitRange = 260.f;

    // Movement
    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float PatrolSpeed = 90.f;

    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float ChaseSpeed = 260.f;

    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float AttackSpeed = 0.f;

    // Runtime
    bool bHasPatrolMove = false;
    int32 PatrolIndex = 0;

    EZombieState State = EZombieState::Patrol;

    UPROPERTY()
    APawn* ControlledPawn = nullptr;

    UPROPERTY()
    ACharacter* PlayerChar = nullptr;

    UPROPERTY()
    TArray<AAIPatrolPoint*> PatrolPoints;

private:
    void GatherPatrolPoints();
    void SetState(EZombieState NewState);

    void DoPatrol();
    void DoChase();
    void DoAttack();

    bool CanSeePlayer() const;
    float DistToPlayer() const;
};
