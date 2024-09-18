// Copyright Epic Games, Inc. All Rights Reserved.

#include "VeomTestPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "VeomTestCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AVeomTestPlayerController::AVeomTestPlayerController()
{
	bShowMouseCursor = true;
}

void AVeomTestPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AVeomTestPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetClickAction, ETriggerEvent::Completed, this, &AVeomTestPlayerController::OnClickReleased);

		//EnhancedInputComponent->BindAxis("MoveForward", ETriggerEvent::Triggered, this, &AVeomTestPlayerController::OnMoveForward);
		//EnhancedInputComponent->BindAction(MoveRight, ETriggerEvent::Triggered, this, &AVeomTestPlayerController::OnMoveRight);

		// Setup key input events
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AVeomTestPlayerController::OnInputStarted);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AVeomTestPlayerController::OnTouchTriggered);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AVeomTestPlayerController::OnTouchReleased);
		//EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AVeomTestPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// Triggered every frame when the input is held down
void AVeomTestPlayerController::OnClickReleased()
{

	FHitResult Hit;
	bool bHitSuccessful = false;

	bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	
	if (bHitSuccessful)
	{
		
	}
}

void OnMoveForward(float value)
{
	//UE_LOG(LogTemplateCharacter, Error, TEXT("value");
}

void OnMoveRight(float value)
{

}