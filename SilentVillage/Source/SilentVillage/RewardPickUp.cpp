#include "RewardPickUp.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "HealthComponentNew.h"
#include "PlayerFPSCharacter.h"
#include "Kismet/GameplayStatics.h"


ARewardPickUp::ARewardPickUp()
{
    PrimaryActorTick.bCanEverTick = false;

    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
    SetRootComponent(Sphere);
    Sphere->InitSphereRadius(60.f);

    Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Sphere->SetCollisionObjectType(ECC_WorldDynamic);
    Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    Sphere->SetGenerateOverlapEvents(true);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Sphere);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &ARewardPickUp::OnOverlap);
}

void ARewardPickUp::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

    UE_LOG(LogTemp, Warning, TEXT("RewardPickUp OVERLAP TRIGGERED"));


    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (OtherActor != PlayerPawn) return;

    UHealthComponentNew* HealthComp = PlayerPawn->FindComponentByClass<UHealthComponentNew>();
    APlayerFPSCharacter* Player = Cast<APlayerFPSCharacter>(PlayerPawn);

    switch (RewardType)
    {
    case ERewardType::Health:
        if (HealthComp)
        {
            HealthComp->Heal((float)Amount);
            UE_LOG(LogTemp, Warning, TEXT("Healed player for %d"), Amount);
        }
        break;

    case ERewardType::Ability:
        if (Player)
        {
            switch (AbilityType)
            {
            case EAbilityType::Speed:
                Player->ActivateSpeedBoost(SpeedMultiplier, AbilityDuration);
                UE_LOG(LogTemp, Warning, TEXT("Ability picked up: Speed Boost"));
                break;

            case EAbilityType::DoubleDamage:
                Player->ActivateDoubleDamage(SpeedMultiplier, AbilityDuration);
                UE_LOG(LogTemp, Warning, TEXT("Ability picked up: Double Damage"));
                break;

            case EAbilityType::Invulnerability:
                Player->ActivateInvulnerability(AbilityDuration); 
                UE_LOG(LogTemp, Warning, TEXT("Ability picked up: Invulnerability"));
                break;

            }
        }
        break;


    case ERewardType::Ammo:
        //Maybe add later on
        break;

    case ERewardType::Collectible:
        //Maybe add later on
        break;

    }

    Destroy();
}