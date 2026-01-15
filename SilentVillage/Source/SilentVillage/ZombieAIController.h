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
    UPROPERTY(EditAnywhere, Category = "AI|Patrol")
    float PatrolAcceptanceRadius = 1000.f;

    UPROPERTY(EditAnywhere, Category = "AI|Sense")
    float DetectRange = 1200.f;

    UPROPERTY(EditAnywhere, Category = "AI|Sense")
    float LoseRange = 2200.f;

    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float AttackRange = 100.f;

    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float AttackInterval = 1.0f;

    UPROPERTY(EditAnywhere, Category = "AI|Combat")
    float AttackDamage = 20.f;

    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float PatrolSpeed = 90.f;

    UPROPERTY(EditAnywhere, Category = "AI|Movement")
    float ChaseSpeed = 260.f;
    
    bool bHasPatrolMove = false;

    EZombieState State = EZombieState::Patrol;

    UPROPERTY()
    APawn* ControlledPawn = nullptr;

    UPROPERTY()
    ACharacter* PlayerChar = nullptr;

    UPROPERTY()
    TArray<AAIPatrolPoint*> PatrolPoints;

    int32 PatrolIndex = 0;

    FTimerHandle AttackTimer;

    void GatherPatrolPoints();
    void SetState(EZombieState NewState);

    void DoPatrol();
    void DoChase();
    void StartAttack();
    void StopAttack();
    void AttackTick();

    bool CanSeePlayer() const;
    float DistToPlayer() const;
	
};
