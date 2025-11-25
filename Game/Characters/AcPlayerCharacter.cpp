// Fill out your copyright notice in the Description page of Project Settings.


#include "AcPlayerCharacter.h"
#include "dazAda/Game/Player/AcPlayerController.h"
#include "dazAda/Game/Player/AcPlayerState.h"
#include "dazAda/Game/Camera/AcCameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "dazAda/Game/AbilitySystem/Attributes/AcCombatAttributeSet.h"
#include "dazAda/Game/Characters/AcCharacterStatusComponent.h"



/*GEngine->AddOnScreenDebugMessage(
-1,           // Key: -1 for a unique message, or a specific key to update an existing message
5.0f,         // TimeToDisplay: How long the message stays on screen (in seconds)
FColor::Yellow, // Display color of the message
TEXT("start from C++!"));*/


// Sets default values
AAcPlayerCharacter::AAcPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	//setup cameras 
	/*CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;*/
	/*ThirdPersonCameraComponent = CreateDefaultSubobject<UAcCameraComponent>(TEXT("ThirdPersonCameraComponent"));*/
	/*ThirdPersonCameraComponent->SetupAttachment(CameraBoom);*/

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = false;
	
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UAcAbilitySystemComponent>("AbilitySystemComponent");
	CharacterStatusComponent = CreateDefaultSubobject<UAcCharacterStatusComponent>("CharacterStatusComponent");
	SetPlayerLockCamera(true);
}

// Called when the game starts or when spawned
void AAcPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}	
	}                                                               
	GetCharacterMovement()->MaxWalkSpeed = CombatAttributeSet->GetEffectiveMovementSpeed();
	GetCharacterMovement()->MaxAcceleration = CombatAttributeSet->GetEffectiveMovementAcceleration();
}

// Called every frame
void AAcPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsAlive && lockCamera) RotateToCamera();

}

//todo
void AAcPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	/*PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AAcPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AAcPlayerCharacter::MoveRight);*/
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AAcPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AAcPlayerCharacter::Turn);



	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AAcPlayerCharacter::HandleMove);
		EIC->BindAction(IA_Jog, ETriggerEvent::Started, this, &AAcPlayerCharacter::HandleJogStart);
		EIC->BindAction(IA_Jog, ETriggerEvent::Completed, this, &AAcPlayerCharacter::HandleJogEnd);
		EIC->BindAction(IA_Sprint, ETriggerEvent::Started, this, &AAcPlayerCharacter::HandleSprintStart);
		EIC->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AAcPlayerCharacter::HandleSprintEnd);
		EIC->BindAction(IA_LockCamera, ETriggerEvent::Triggered, this, &AAcPlayerCharacter::HandleLockCamera);
		EIC->BindAction(IA_Ability1, ETriggerEvent::Triggered, this, &AAcPlayerCharacter::HandleAbility1);
		EIC->BindAction(IA_Ability2, ETriggerEvent::Triggered, this, &AAcPlayerCharacter::HandleAbility2);
		EIC->BindAction(IA_Ability3, ETriggerEvent::Triggered, this, &AAcPlayerCharacter::HandleAbility3);
		EIC->BindAction(IA_Ability4, ETriggerEvent::Triggered, this, &AAcPlayerCharacter::HandleAbility4);
	}

}

AAcPlayerController* AAcPlayerCharacter::GetAcPlayerController() const
{
	return CastChecked<AAcPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AAcPlayerState* AAcPlayerCharacter::GetAcPlayerState() const
{
	return CastChecked<AAcPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}


void AAcPlayerCharacter::MoveForward(float Value)
{
	if (Value == 0) return;
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AAcPlayerCharacter::MoveRight(float Value)
{
	if( Value == 0) return;
	const FRotator ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AAcPlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AAcPlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}


void AAcPlayerCharacter::HandleMove(const FInputActionInstance& Instance)
{
	FVector2D MovementVector = Instance.GetValue().Get<FVector2D>();
	const FRotator ControlRotation = GetControlRotation();
	const float CurrentTime = GetWorld()->GetTimeSeconds();

	float ForwardValue = MovementVector.Y; // W/S
	float RightValue   = MovementVector.X; // A/D

	// --- Forward/backward filter ---
	if (ForwardValue != 0.f)
	{
		int32 NewDir = (ForwardValue > 0.f) ? +1 : -1;

		if (LastForwardDir != 0 && NewDir != LastForwardDir)
		{
			float Delta = CurrentTime - LastForwardKeyTime;
			if (Delta < OppositeInputLockTime)
			{
				// Block quick reverse
				ForwardValue = 0.f;
			}
		}

		if (ForwardValue != 0.f)
		{
			LastForwardDir = NewDir;
			LastForwardKeyTime = CurrentTime;
		}
	}

	// --- Left/right filter ---
	if (RightValue != 0.f)
	{
		int32 NewDir = (RightValue > 0.f) ? +1 : -1;

		if (LastRightDir != 0 && NewDir != LastRightDir)
		{
			float Delta = CurrentTime - LastRightKeyTime;
			if (Delta < OppositeInputLockTime)
			{
				// Block quick side flip
				RightValue = 0.f;
				
			}
		}

		if (RightValue != 0.f)
		{
			LastRightDir = NewDir;
			LastRightKeyTime = CurrentTime;
		}
	}

	// --- Apply filtered movement ---
	if (Controller)
	{
		const FRotator YawRot(0.0f, ControlRotation.Yaw, 0.0f);

		if (ForwardValue != 0.f)
		{
			const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
			AddMovementInput(ForwardDir, ForwardValue);
		}

		if (RightValue != 0.f)
		{
			const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
			AddMovementInput(RightDir, RightValue);
		}
	}
}



void AAcPlayerCharacter::HandleLockCamera(const FInputActionInstance& Instance)
{
	this->lockCamera = !this->lockCamera;
	GetCharacterMovement()->bOrientRotationToMovement = !this->lockCamera;
}

void AAcPlayerCharacter::HandleAbility1(const FInputActionInstance& Instance)
{
	// The message to display (use TEXT() macro for FString literals)

	if (AbilitySystemComponent)
	{

		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.Skill.Ability1")));
	}
}

void AAcPlayerCharacter::HandleAbility2(const FInputActionInstance& Instance)
{
	if (AbilitySystemComponent)
	{

		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.Skill.Ability2")));
	}
}

void AAcPlayerCharacter::HandleAbility3(const FInputActionInstance& Instance)
{
	if (AbilitySystemComponent)
	{

		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.Skill.Ability3")));
	}
}
void AAcPlayerCharacter::HandleAbility4(const FInputActionInstance& Instance)
{
	if (AbilitySystemComponent)
	{

		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Ability.Skill.Ability4")));
	}
}

void AAcPlayerCharacter::HandleJogStart(const FInputActionInstance& Instance)
{
 // The message to display (use TEXT() macro for FString literals)

	if (AbilitySystemComponent)
	{

		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Locomotion.MovementMode.Jog")));
	}
}

void AAcPlayerCharacter::HandleJogEnd(const FInputActionInstance& Instance)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer TagsToCancel;
		TagsToCancel.AddTag(FGameplayTag::RequestGameplayTag("Locomotion.MovementMode.Jog"));
		AbilitySystemComponent->CancelAbilities(&TagsToCancel);
		
	}
}

void AAcPlayerCharacter::HandleSprintStart(const FInputActionInstance& Instance)
{
	// The message to display (use TEXT() macro for FString literals)

	if (AbilitySystemComponent)
	{

		AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Locomotion.MovementMode.Sprint")));
	}
}

void AAcPlayerCharacter::HandleSprintEnd(const FInputActionInstance& Instance)
{
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer TagsToCancel;
		TagsToCancel.AddTag(FGameplayTag::RequestGameplayTag("Locomotion.MovementMode.Sprint"));
		AbilitySystemComponent->CancelAbilities(&TagsToCancel);
		
	}
}


void AAcPlayerCharacter::InitAbilitySystemComponent()
{
	AAcPlayerState* AcPlayerState = GetAcPlayerState();
	AbilitySystemComponent = CastChecked<UAcAbilitySystemComponent>(AcPlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(AcPlayerState, this);
	CombatAttributeSet = AcPlayerState->GetCombatAttributeSet();
	
}

void AAcPlayerCharacter::LockCameraToHead(bool bLockCameraToHead)
{
	lockCameraToHead = bLockCameraToHead;
}

void AAcPlayerCharacter::SetPlayerLockCamera(bool value)
{
	this->lockCamera = value;
	GetCharacterMovement()->bOrientRotationToMovement = !value;
}

void AAcPlayerCharacter::RotateToCamera()
{
	FRotator current = GetActorRotation();
	FRotator target = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	float worldDeltaSecond = GetWorld()->GetDeltaSeconds();
	FRotator res = UKismetMathLibrary::RInterpTo(current, target, worldDeltaSecond, 10);
	SetActorRotation(res);
}

void AAcPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilitySystemComponent();
	GiveDefaultAbilities();
	InitDefaultAbilities();
	InitStartupEffects();
}

void AAcPlayerCharacter::UnPossessed()
{
	Super::UnPossessed();
}

/*void AAcPlayerCharacter::BindASCInput()
{
	
	if (!ASCInputBound && IsValid(AbilitySystemComponent) && IsValid(InputComponent))
	{
		FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(FName("/Script/dazAda"), FName("EACAbilityInputID"));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
	FString("CancelTarget"), AbilityEnumAssetPath, static_cast<int32>(EACAbilityInputID::Confirm), static_cast<int32>(EACAbilityInputID::Cancel)));
		ASCInputBound = true;
		GEngine->AddOnScreenDebugMessage(
-1,                     // Key (-1 = new message every time)
2.0f,                   // Duration (seconds)
FColor::Red,            // Color
TEXT("bound")             // Message

);
	}
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			FString AbilityName = Spec.Ability->GetName();
			int32 InputID = Spec.InputID;

			GEngine->AddOnScreenDebugMessage(
				-1, 5.0f, FColor::Green,
				FString::Printf(TEXT("Ability %s bound to InputID %d"), *AbilityName, InputID)
			);
		}
	}
}*/
void AAcPlayerCharacter::BindASCInput()
{
	if (UEnum* EnumPtr = StaticEnum<EACAbilityInputID>())
	{
		// Convert FString → FName
		FTopLevelAssetPath AbilityEnumAssetPath(FName(*EnumPtr->GetPathName()));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(
			InputComponent,
			FGameplayAbilityInputBinds(
				FString("ConfirmTarget"),
				FString("CancelTarget"),
				AbilityEnumAssetPath,
				static_cast<int32>(EACAbilityInputID::Confirm),
				static_cast<int32>(EACAbilityInputID::Cancel)
			)
		);

		ASCInputBound = true;

		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("ASC Input bound to enum: %s"), *EnumPtr->GetPathName())
		);
	}
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.Ability)
		{
			FString AbilityName = Spec.Ability->GetName();
			int32 InputID = Spec.InputID;

			GEngine->AddOnScreenDebugMessage(
				-1, 5.0f, FColor::Green,
				FString::Printf(TEXT("Ability %s bound to InputID %d"), *AbilityName, InputID)
			);
		}
	}
}

void AAcPlayerCharacter::OnDeath()
{
	Super::OnDeath();
	DisablePlayerInput();
}

void AAcPlayerCharacter::DisablePlayerInput()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return; // NPC or AI, ignore

	// Only disable once
	bInputWasEnabled = !PC->IsInputKeyDown(EKeys::AnyKey);

	PC->DisableInput(PC);
	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);
}

void AAcPlayerCharacter::EnablePlayerInput()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return; // NPC or AI, ignore

	// Only re-enable if previously disabled
	if (bInputWasEnabled)
	{
		PC->EnableInput(PC);
		PC->SetIgnoreLookInput(false);
		PC->SetIgnoreMoveInput(false);
	}
	
}