// Copyright Epic Games, Inc. All Rights Reserved.


#include "SilentVillagePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "SilentVillageCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "SilentVillage.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Kismet/GameplayStatics.h"
#include "ZombieGameInstance.h"

ASilentVillagePlayerController::ASilentVillagePlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ASilentVillageCameraManager::StaticClass();
}

void ASilentVillagePlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogSilentVillage, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void ASilentVillagePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

void ASilentVillagePlayerController::ShowWinScreen()
{
	if (!WinWidgetClass) return;

	UUserWidget* WinWidget = CreateWidget<UUserWidget>(this, WinWidgetClass);
	if (!WinWidget) return;

	WinWidget->AddToViewport(999);

	SetPause(true);

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
}

void ASilentVillagePlayerController::RestartWholeGame()
{
	SetPause(false);

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	bShowMouseCursor = false;

	FInputModeGameOnly GameOnly;
	SetInputMode(GameOnly);

	// Clear progress
	if (UZombieGameInstance* GI = GetGameInstance<UZombieGameInstance>())
	{
		GI->ResetProgress();
		GI->ZombiesKilled = 0;
		GI->CurrentObjective = ELevelObjectiveType::CollectItems;
	}

	UGameplayStatics::OpenLevel(this, FName(TEXT("Lvl_FirstPerson")));
}