#include "CollectableItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SilentVillageGameMode.h"
#include "GameFramework/Pawn.h"


// Sets default values
ACollectableItem::ACollectableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);
	Sphere->InitSphereRadius(60.f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Sphere);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACollectableItem::OnOverlap);
}

void ACollectableItem::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor != PlayerPawn) return;

	if (ASilentVillageGameMode* GM = Cast<ASilentVillageGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->AddCollectible(Value);

		UE_LOG(LogTemp, Warning, TEXT("Collectible picked: %d/%d"),
			GM->GetCollectedCount(), GM->GetRequiredCollectibles());
	}

	Destroy();
}