#include "PlayerProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerProjectile::APlayerProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Collision->InitSphereRadius(8.f);
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    SetRootComponent(Collision);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Collision);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 2500.f;
    ProjectileMovement->MaxSpeed = 2500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    Collision->OnComponentHit.AddDynamic(this, &APlayerProjectile::OnProjectileHit);

}

// Called when the game starts or when spawned
void APlayerProjectile::BeginPlay()
{
    Super::BeginPlay();
    SetLifeSpan(LifeSeconds);
}

void APlayerProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    AActor* MyOwner = GetOwner();
    if (!OtherActor || OtherActor == this || OtherActor == MyOwner)
    {
        Destroy();
        return;
    }

    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        MyOwner ? MyOwner->GetInstigatorController() : nullptr,
        this,
        UDamageType::StaticClass()
    );

    Destroy();
}

