#include "AcBaseCharacter.h"
#include "dazAda/Game/AbilitySystem/AcAbilitySystemComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "AcCharacterMovementComponent.h"
#include "dazAda/Game/AbilitySystem/Attributes/AcCombatAttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "dazAda/Game/Components/AC_CombatComponent.h"
#include "dazAda/Game/Components/AC_RagdollComponent.h"
#include "dazAda/Game/Components/AC_StatusEffectComponent.h"

class AActor;
class UInputComponent;
AAcBaseCharacter::AAcBaseCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UAcCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//setup char body
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	//check(CapsuleComp);
	UpperBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UpperBody"));
	LowerBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LowerBody"));
	FootLeft = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FootLeft"));
	FootRight = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FootRight"));
	UpperBody->SetupAttachment(GetMesh());
	LowerBody->SetupAttachment(GetMesh());
	FootLeft->SetupAttachment(GetMesh());
	FootRight->SetupAttachment(GetMesh());

	
	UAcCharacterMovementComponent* ArcaneMoveComp = CastChecked<UAcCharacterMovementComponent>(GetCharacterMovement());
	GetCharacterMovement()->bOrientRotationToMovement = true;
	CombatAttributeSet = CreateDefaultSubobject<UAcCombatAttributeSet>("CombatAttributeSet");

	CombatCoreComponent = CreateDefaultSubobject<UAC_CombatComponent>(TEXT("Combat Core Component"));
	StatusEffectComponent = CreateDefaultSubobject<UAC_StatusEffectComponent>(TEXT("Hit Reaction Component"));
	RagdollComponent = CreateDefaultSubobject<UAC_RagdollComponent>(TEXT("Ragdoll Component"));
	
}

/*void AAcBaseCharacter::Tick(float DeltaTime)
{
	if (bIsRagdoll) UpdateRagdoll();
}*/



UAcAbilitySystemComponent* AAcBaseCharacter::GetAcAbilitySystemComponent() const
{
	return Cast<UAcAbilitySystemComponent>(GetAbilitySystemComponent());
}

//todo
UAbilitySystemComponent* AAcBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}



void AAcBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}


void AAcBaseCharacter::SetIsAlive() 
{
	const bool bWasAlive = bIsAlive;
	bIsAlive = (GetHealth() > 0.f);
	
	// The moment health reaches 0 for the first time
	if (bWasAlive && !bIsAlive)
	{
		OnDeath();
	}
}
void AAcBaseCharacter::OnDeath()
{
	AbilitySystemComponent->CancelAbilities(nullptr);
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->DisableMovement();
		MoveComp->StopMovementImmediately();
	}

		AbilitySystemComponent->AddLooseGameplayTag(
			FGameplayTag::RequestGameplayTag("State.Dead")
		);

	GEngine->AddOnScreenDebugMessage(
	-1, 3.f, FColor::Red,
	FString::Printf(TEXT("%s is now DEAD (State.Dead tag added)."), *GetName()));

	if (RagdollComponent)
	{
		RagdollComponent->StartRagdoll();
	}


}

void AAcBaseCharacter::OnAbilitySystemInitialized()
{
	UAcAbilitySystemComponent* ArcaneASC = GetAcAbilitySystemComponent();
	check(ArcaneASC);
	
}

void AAcBaseCharacter::OnAbilitySystemUninitialized()
{
}

void AAcBaseCharacter::OnMovementAccelerationChanged(const FOnAttributeChangeData& Data) const
{
	GetCharacterMovement()->MaxAcceleration = Data.NewValue;
}

void AAcBaseCharacter::OnMovementSpeedChanged(const FOnAttributeChangeData& Data) const
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}




//todo
void AAcBaseCharacter::InitializeGameplayTags()
{

	
}

void AAcBaseCharacter::InitStartupEffects()
{
	if (!AbilitySystemComponent || AbilitySystemComponent->bStartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1.f,EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent.Get());
		}
	}

	AbilitySystemComponent->bStartupEffectsApplied = true;
}



void AAcBaseCharacter::UpdateMovementState()
{
	FVector Velocity = GetVelocity();
	FVector Acceleration = GetCharacterMovement()->GetCurrentAcceleration();
	FVector NormalizedVel = Velocity.GetSafeNormal();
	FVector NormalizedAcc = Acceleration.GetSafeNormal();
	const float AccelMag = Acceleration.Size();
	const float MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	
	// If dot product < 0, movement and acceleration are opposite → likely slowing down / idle
	if (FVector::DotProduct(NormalizedVel, NormalizedAcc) < 0.f)
	{
		CurrentLocomotionState = EACLocomotionState::Idle;
	}else if (MaxWalkSpeed > 350.f && AccelMag > 200.f)
	{
		CurrentLocomotionState = EACLocomotionState::Sprint;
	}else if (MaxWalkSpeed > 240.f && AccelMag > 160.f)
	{
		CurrentLocomotionState = EACLocomotionState::Jog;

	}else if (MaxWalkSpeed > 1.f && AccelMag > .1f)
	{
		CurrentLocomotionState = EACLocomotionState::Walk;

	}else
	{
		CurrentLocomotionState = EACLocomotionState::Idle;
	}
}


UAcCombatAttributeSet* AAcBaseCharacter::GetCombatAttributeSet() const
{
	return CombatAttributeSet;
}

void AAcBaseCharacter::GiveDefaultAbilities()
{
	check(AbilitySystemComponent);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbilities)
	{
		const FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
	/*GEngine->AddOnScreenDebugMessage(
-1,                     // Key (-1 = new message every time)
2.0f,                   // Duration (seconds)
FColor::Red,            // Color
TEXT("given!")             // Message
);*/
	
}

void AAcBaseCharacter::InitDefaultAbilities() const
{
	if (!AbilitySystemComponent || !DefaultGameplayEffect ){return; }
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultGameplayEffect, 1.f, EffectContext);
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAcCombatAttributeSet::GetEffectiveMovementAccelerationAttribute())
	.AddUObject(this, &AAcBaseCharacter::OnMovementAccelerationChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAcCombatAttributeSet::GetEffectiveMovementSpeedAttribute())
	.AddUObject(this, &AAcBaseCharacter::OnMovementSpeedChanged);
}

float AAcBaseCharacter::GetHealth() const
{
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetHealth();
	}

	return 0.0f;
}

float AAcBaseCharacter::GetStance() const
{
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetStance();
	}

	return 0.0f;
}

float AAcBaseCharacter::GetStancePercentage() const
{
	if (!CombatAttributeSet) 
		return 0.f;

	const float Stance = CombatAttributeSet->GetStance();
	const float MaxStance = CombatAttributeSet->GetMaxStance();

	// Avoid division by zero
	if (MaxStance <= 0.f)
		return 0.f;

	return FMath::Clamp(Stance / MaxStance, 0.f, 1.f);
}

float AAcBaseCharacter::GetHealthPercentage() const
{
	if (!CombatAttributeSet) 
		return 0.f;

	const float health = CombatAttributeSet->GetHealth();
	const float MaxHealth = CombatAttributeSet->GetMaxHealth();

	// Avoid division by zero
	if (MaxHealth <= 0.f)
		return 0.f;

	return FMath::Clamp(health / MaxHealth, 0.f, 1.f);
}

float AAcBaseCharacter::GetBaseAD() const
{
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetBaseAD();
	}

	return 0.0f;
}

float AAcBaseCharacter::GetBaseForce() const
{
	if (CombatAttributeSet)
	{
		return CombatAttributeSet->GetBaseForce();
	}

	return 0.0f;
}



void AAcBaseCharacter::EquipWeapon(AWeapon* NewWeapon)
{
	/*if (!NewWeapon || !AbilitySystemComponent)
		return;
	Weapon = NewWeapon;
	// Grant abilities from weapon
	
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbilities)
	{
		if (*AbilityClass)
		{
			FGameplayAbilitySpecHandle Handle = 
				AbilitySystemComponent->GiveAbility(
					FGameplayAbilitySpec(AbilityClass, 1, 0));

			NewWeapon->GrantedAbilityHandles.Add(Handle);
		}
	}*/	
	
}

void AAcBaseCharacter::UnequipWeapon()
{
	/*if (Weapon && AbilitySystemComponent)
	{
		for (const FGameplayAbilitySpecHandle& Handle : Weapon->GrantedAbilityHandles)
		{
			AbilitySystemComponent->ClearAbility(Handle);
		}
		Weapon->GrantedAbilityHandles.Empty();
	}

	Weapon = nullptr;*/
}



void AAcBaseCharacter::UpdateRagdoll()
{
	if (GetCapsuleComponent() == nullptr)
	{
				GEngine->AddOnScreenDebugMessage(
		-1,                     // Key (-1 = new message every time)
		2.0f,                   // Duration (seconds)
		FColor::Red,            // Color
		TEXT("error update ragdoll no capsule comp")             // Message

		);
		return; 
	}
	LastRagdollVelocity = GetMesh()->GetPhysicsLinearVelocity(FName("root"));
	
	float DriveStrength = FMath::GetMappedRangeValueClamped(
	FVector2D(0.0f, 1000.0f),   // Input range (velocity)
	FVector2D(0.0f, 25000.0f),  // Output range (angular drive strength)
	LastRagdollVelocity.Size());

	GetMesh()->SetAllMotorsAngularDriveParams(
	DriveStrength,  // spring (strength)
	0.0f,  // damping
	0.0f            // force limit (0 = no limit)
	);
	GetMesh()->SetEnableGravity(LastRagdollVelocity.Z > -4000);

	FVector TargetRagdollLocation = GetMesh()->GetSocketLocation(FName("hip"));
	FRotator HipRotation = GetMesh()->GetSocketRotation(FName("hip"));
	bRagdollFaceUp = HipRotation.Pitch > 0.0f;
	FRotator newRot = FRotator(0.0f);
	if (bRagdollFaceUp)
	{
		newRot.Yaw = HipRotation.Pitch - 180.0f;
	}else
	{
		newRot.Yaw = HipRotation.Pitch;
	}
	FRotator TargetRagdollRotation = newRot;
	FVector TraceEND = FVector(TargetRagdollLocation.X, TargetRagdollLocation.Y, TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());



	FHitResult HitResult;

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this); // ignore self
	
	// Perform the trace
	GetWorld()->LineTraceSingleByChannel(
		HitResult,       // out hit result
		TargetRagdollLocation,           // start location
		TraceEND,             // end location
		ECC_Visibility,  // trace channel (can be ECC_Pawn, ECC_Camera, etc.)
		TraceParams
	);

	if (HitResult.bBlockingHit)
	{
		bRagdollOnGround = true;
		
		FVector NewCharLoc = FVector(TargetRagdollLocation.X,TargetRagdollLocation.Y,
			TargetRagdollLocation.Z
			+ (GetCapsuleComponent()->GetScaledCapsuleHalfHeight()-abs(HitResult.ImpactPoint.Z-HitResult.TraceStart.Z))
			+2.0f
			);
		

		SetActorLocationAndRotation(NewCharLoc,TargetRagdollRotation,false,nullptr);
	}else
	{
		bRagdollOnGround = false;
		SetActorLocationAndRotation(TargetRagdollLocation,TargetRagdollRotation,false,nullptr);
	}
	
	
	
	/*FTransform HipTransform = GetMesh()->GetSocketTransform(FName("hip"));
	FVector HipLocation = HipTransform.GetLocation();
	FRotator HipRotation = HipTransform.GetRotation().Rotator();
	HipLocation.Z += 30.0f;
	FRotator NewRotation(0.0f, HipRotation.Yaw, 0.0f);
	SetActorLocationAndRotation(HipLocation, NewRotation);*/
}