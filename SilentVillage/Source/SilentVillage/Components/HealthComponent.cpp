#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void UHealthComponent::TakeDamage(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        // Player death will go here later
    }
}
