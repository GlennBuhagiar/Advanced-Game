#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIPatrolPoint.generated.h"

UCLASS()
class SILENTVILLAGE_API AAIPatrolPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAIPatrolPoint();

protected:
	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* Billboard;

};
