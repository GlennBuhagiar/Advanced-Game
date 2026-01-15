#include "HealthComponentNew.h"
#include "Math/UnrealMathUtility.h"

UHealthComponentNew::UHealthComponentNew()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponentNew::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void UHealthComponentNew::TakeDamageSimple(float DamageAmount)
{
    if (DamageAmount <= 0.f || CurrentHealth <= 0.f)
        return;

    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        OnDeath.Broadcast();
    }
}

void UHealthComponentNew::Heal(float HealAmount)
{
    if (HealAmount <= 0.f || CurrentHealth <= 0.f)
        return;

    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);
}
