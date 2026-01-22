#include "PlayerFPSCharacter.h"
#include "HealthComponentNew.h"
#include "PlayerProjectile.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"

#include "PlayerGameMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SilentVillageGameMode.h"
#include "ZombieGameInstance.h"
#include "Engine/World.h"


// Sets default values
APlayerFPSCharacter::APlayerFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(GetCapsuleComponent());
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
    Camera->bUsePawnControlRotation = true;

    Health = CreateDefaultSubobject<UHealthComponentNew>(TEXT("HealthComponent"));
    Health->MaxHealth = 100.f;

    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;

}

void APlayerFPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!GetWorld()) return;

    const FString LevelName = GetWorld()->GetName();

    

    if (LevelName.Contains(TEXT("Level2")) && Level2MenuWidgetClass)
    {
        MenuWidgetClass = Level2MenuWidgetClass;
    }
}



// Called to bind functionality to input
void APlayerFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &APlayerFPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &APlayerFPSCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &APlayerFPSCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &APlayerFPSCharacter::LookUp);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerFPSCharacter::Fire);

    PlayerInputComponent->BindAction("ToggleMenu", IE_Pressed, this, &APlayerFPSCharacter::ToggleMenu);

}

void APlayerFPSCharacter::MoveForward(float Value)
{
    if (Value != 0.f)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
}

void APlayerFPSCharacter::MoveRight(float Value)
{
    if (Value != 0.f)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

void APlayerFPSCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void APlayerFPSCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void APlayerFPSCharacter::Fire()
{
    if (!bCanFire || !ProjectileClass) return;
    bCanFire = false;

    const FVector SpawnLoc = Camera->GetComponentLocation() + Camera->GetForwardVector() * MuzzleDistance;
    const FRotator SpawnRot = Camera->GetComponentRotation();

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.Instigator = this;

    GetWorld()->SpawnActor<APlayerProjectile>(ProjectileClass, SpawnLoc, SpawnRot, Params);

    GetWorldTimerManager().SetTimer(FireCooldownHandle, this, &APlayerFPSCharacter::ResetFire, FireCooldown, false);
}

void APlayerFPSCharacter::ResetFire()
{
    bCanFire = true;
}

void APlayerFPSCharacter::ToggleMenu()
{
    // CREATE widget ONCE
    if (!MenuWidget)
    {
        if (MenuWidgetClass)
        {
            MenuWidget = CreateWidget<UPlayerGameMenuWidget>(GetWorld(), MenuWidgetClass);
            UpdateMenuUI();
        }
    }

    if (!MenuWidget) return;

    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    
    // CLOSE MENU
    if (MenuWidget->IsInViewport())
    {
        MenuWidget->RemoveFromParent();
        StopMenuUpdates();

        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;

        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
        return;
    }

    
    // OPEN MENU
    MenuWidget->AddToViewport();
    UpdateMenuUI();
    StartMenuUpdates();

    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(true);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

    PC->SetInputMode(InputMode);
    PC->bShowMouseCursor = false;

    PC->SetIgnoreMoveInput(true);
    PC->SetIgnoreLookInput(true);
}



void APlayerFPSCharacter::UpdateMenuUI()
{
    if (!MenuWidget) return;

    if (Health)
    {
        MenuWidget->SetHealth(Health->CurrentHealth, Health->MaxHealth);
    }

    

    if (UZombieGameInstance* GI = GetGameInstance<UZombieGameInstance>())
    {
        if (GI->CurrentObjective == ELevelObjectiveType::CollectItems)
        {
            
                MenuWidget->SetObjectiveText(
                    FText::FromString(TEXT("Objective: Collect 5 items and escape"))
                );
            
            MenuWidget->SetObjectiveText(
                FText::FromString(
                    FString::Printf(
                        TEXT("Objective: Collect %d / %d items"),
                        GI->GetCollectedCount(),
                        GI->GetRequiredCollectibles()
                    )
                )
            );
        }
        else if (GI->CurrentObjective == ELevelObjectiveType::KillZombies)
        {
            MenuWidget->SetObjectiveText(
                FText::FromString(
                    FString::Printf(
                        TEXT("Objective: Kill %d / %d Zombies"),
                        GI->ZombiesKilled,
                        GI->RequiredZombieKills
                    )
                )
            );
        }
    }
}


float APlayerFPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (bInvulnerableActive)
    {
        UE_LOG(LogTemp, Display, TEXT("Damage blocked (Invulnerability Active)"));
        return 0.f;
    }


    const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    const float UsedDamage = (ActualDamage > 0.f) ? ActualDamage : DamageAmount;

    if (bInvulnerableActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player is Invulnerable - damage ignored"));
        return 0.f;
    }

    if (Health && UsedDamage > 0.f && Health->CurrentHealth > 0.f)
    {
        Health->TakeDamageSimple(UsedDamage);
        if (MenuWidget && MenuWidget->IsInViewport())
        {
            UpdateMenuUI();
        }
        UE_LOG(LogTemp, Warning, TEXT("PLAYER DAMAGED: %.1f | HP now %.1f"),
            UsedDamage, Health->CurrentHealth);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PLAYER TakeDamage fired but Health missing or damage=0"));
    }

    return UsedDamage;
}

void APlayerFPSCharacter::ActivateSpeedBoost(float Multiplier, float Duration)
{
    if (Multiplier <= 0.f) return;

    bSpeedBoostActive = true;
    SpeedBoostMultiplier = Multiplier;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SpeedBoostMultiplier;

    GetWorldTimerManager().ClearTimer(AbilityTimerHandle);
    GetWorldTimerManager().SetTimer(
        AbilityTimerHandle,
        this,
        &APlayerFPSCharacter::EndSpeedBoost,
        Duration,
        false
    );

    UpdateMenuUI();
}

void APlayerFPSCharacter::EndSpeedBoost()
{
    bSpeedBoostActive = false;
    SpeedBoostMultiplier = 1.0f;

    GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

    UpdateMenuUI();
}

FText APlayerFPSCharacter::GetActiveAbilityText() const
{
    if (bSpeedBoostActive)
        return FText::FromString(TEXT("Speed Boost"));

    return FText::FromString(TEXT("None"));
}

void APlayerFPSCharacter::StartMenuUpdates()
{
    UpdateMenuUI();

    GetWorldTimerManager().SetTimer(
        MenuUpdateTimer,
        this,
        &APlayerFPSCharacter::UpdateMenuUI,
        0.1f,
        true
    );
}

void APlayerFPSCharacter::ActivateDoubleDamage(float Multiplier, float Duration)
{
    if (Multiplier <= 1.f) return;

    bDoubleDamageActive = true;
    DamageMultiplier = Multiplier;

    GetWorldTimerManager().ClearTimer(AbilityTimerHandle);
    GetWorldTimerManager().SetTimer(
        AbilityTimerHandle,
        this,
        &APlayerFPSCharacter::EndDoubleDamage,
        Duration,
        false
    );

    UpdateMenuUI();
    UE_LOG(LogTemp, Warning, TEXT("DOUBLE DAMAGE ACTIVATED | Multiplier = %.2f"), DamageMultiplier);

}

void APlayerFPSCharacter::EndDoubleDamage()
{
    bDoubleDamageActive = false;
    DamageMultiplier = 1.0f;
    UpdateMenuUI();
}

void APlayerFPSCharacter::ActivateInvulnerability(float Duration)
{
    
    bInvulnerableActive = true;

    if (Health)
    {
        Health->bDamageBlocked = true;
    }

    GetMesh()->SetVisibility(false, true);

    GetWorldTimerManager().ClearTimer(AbilityTimerHandle);
    GetWorldTimerManager().SetTimer(
        AbilityTimerHandle,
        this,
        &APlayerFPSCharacter::EndInvulnerability,
        Duration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("INVULNERABILITY ACTIVATED"));
}



void APlayerFPSCharacter::EndInvulnerability()
{
    bInvulnerableActive = false;

    if (Health)
    {
        Health->bDamageBlocked = false;
    }

    GetMesh()->SetVisibility(true, true);

    UE_LOG(LogTemp, Warning, TEXT("INVULNERABILITY ENDED"));
}


void APlayerFPSCharacter::StopMenuUpdates()
{
    GetWorldTimerManager().ClearTimer(MenuUpdateTimer);
}


void APlayerFPSCharacter::HandlePlayerDeath()
{
    // Optional: prevent input / spam
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
    }

    if (UZombieGameInstance* GI = Cast<UZombieGameInstance>(GetGameInstance()))
    {
        GI->ResetProgress();
    }

    // Reload current level
    const FName CurrentLevel = FName(*GetWorld()->GetName());
    UGameplayStatics::OpenLevel(this, CurrentLevel);
}