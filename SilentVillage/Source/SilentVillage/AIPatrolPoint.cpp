#include "AIPatrolPoint.h"
#include "Components/BillboardComponent.h"

// Sets default values
AAIPatrolPoint::AAIPatrolPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    Billboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
    SetRootComponent(Billboard);

}

