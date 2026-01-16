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

// Called when the game starts or when spawned
void APlayerFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
    if (!MenuWidget && MenuWidgetClass)
    {
        MenuWidget = CreateWidget<UPlayerGameMenuWidget>(GetWorld(), MenuWidgetClass);
    }

    if (!MenuWidget) return;

    bMenuOpen = !bMenuOpen;

    if (bMenuOpen)
    {
        MenuWidget->AddToViewport();
        UpdateMenuUI();

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            PC->SetShowMouseCursor(true);
            PC->SetInputMode(FInputModeUIOnly());
        }
    }
    else
    {
        MenuWidget->RemoveFromParent();

        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            PC->SetShowMouseCursor(false);
            PC->SetInputMode(FInputModeGameOnly());
        }
    }
}

void APlayerFPSCharacter::UpdateMenuUI()
{
    if (!MenuWidget) return;

    // assuming your player has a HealthComponentNew called Health
    if (Health)
    {
        MenuWidget->SetHealth(Health->CurrentHealth, Health->MaxHealth);
    }

    MenuWidget->SetObjectiveText(FText::FromString(TEXT("Objective: Collect 5 items and escape")));
}