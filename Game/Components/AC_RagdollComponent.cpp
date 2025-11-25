// Fill out your copyright notice in the Description page of Project Settings.

#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "AC_RagdollComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "dazAda/dazAda.h"
#include "dazAda/Game/Characters/AcPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "dazAda/Game/Interface/CharacterDataInterface.h"

// Sets default values for this component's properties
UAC_RagdollComponent::UAC_RagdollComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_RagdollComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedCharacter = Cast<AAcBaseCharacter>(GetOwner());
	if (CachedCharacter)
	{
		CachedASC = CachedCharacter->GetAbilitySystemComponent();
	}

	
}


// Called every frame
void UAC_RagdollComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!CachedCharacter)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.0f, FColor::Red,
			TEXT("RagdollComponent: No valid character owner!")
		);
		return;
	}
	if (CachedCharacter->CurrentMovementState == EACMovementState::Ragdoll)
	{
		UpdateRagdoll();
	}
}

void UAC_RagdollComponent::UpdateRagdoll()
{
	if (!CachedCharacter)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.0f, FColor::Red,
			TEXT("RagdollComponent: No valid character owner!")
		);
		return;
	}

    UCapsuleComponent* Capsule = CachedCharacter->GetCapsuleComponent();
    USkeletalMeshComponent* Mesh = CachedCharacter->GetMesh();

    if (!Capsule || !Mesh)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 2.0f, FColor::Red,
            TEXT("RagdollComponent: Missing mesh or capsule!")
        );
        return;
    }

    // --- VELOCITY ---
    LastRagdollVelocity = Mesh->GetPhysicsLinearVelocity(FName("root"));

    float DriveStrength = FMath::GetMappedRangeValueClamped(
        FVector2D(0.0f, 1000.0f),
        FVector2D(0.0f, 25000.0f),
        LastRagdollVelocity.Size()
    );

    Mesh->SetAllMotorsAngularDriveParams(
        DriveStrength,
        0.0f,
        0.0f
    );

    Mesh->SetEnableGravity(LastRagdollVelocity.Z > -4000);

    // --- HIP TRANSFORM ---
    FVector HipLoc = Mesh->GetSocketLocation(FName("hip"));
    FRotator HipRot = Mesh->GetSocketRotation(FName("hip"));

    bRagdollFaceUp = HipRot.Pitch > 0.0f;

    /*FRotator RagdollRot(0.0f, 0.0f, 0.0f);
    RagdollRot.Yaw = bRagdollFaceUp ? (HipRot.Pitch - 180.0f) : HipRot.Pitch;*/

	FVector Forward = Mesh->GetSocketRotation("hip").Vector();
	float Yaw = Forward.Rotation().Pitch;
	FRotator RagdollRot = FRotator(0, Yaw, 0);

    // --- GROUNDED CHECK ---
    FVector TraceEnd = HipLoc - FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight());

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CachedCharacter);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        HipLoc,
        TraceEnd,
        ECC_Visibility,
        Params
    );

    if (bHit)
    {
        bRagdollOnGround = true;

        float ZOffset =
            Capsule->GetScaledCapsuleHalfHeight() -
            FMath::Abs(Hit.ImpactPoint.Z - Hit.TraceStart.Z);

        FVector NewLoc = FVector(
            HipLoc.X,
            HipLoc.Y,
            HipLoc.Z + ZOffset + 2.0f
        );

        CachedCharacter->SetActorLocationAndRotation(NewLoc, RagdollRot, false, nullptr);
    }
    else
    {
        bRagdollOnGround = false;
        CachedCharacter->SetActorLocationAndRotation(HipLoc, RagdollRot, false, nullptr);
    }
}

void UAC_RagdollComponent::StartRagdoll()
{
	if (!CachedCharacter)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 2.0f, FColor::Red,
			TEXT("RagdollComponent: No valid character owner!")
		);
		return;
	}
	CachedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);


	if (AAcPlayerCharacter* PCChar = Cast<AAcPlayerCharacter>(CachedCharacter))
	{
		APlayerController* PC = Cast<APlayerController>(PCChar->GetController());
		if (PC)
		{
			PC->DisableInput(PC);
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);

			GEngine->AddOnScreenDebugMessage(
				-1, 2.f, FColor::Orange,
				TEXT("Player input disabled due to ragdoll.")
			);
		}
	}
	
	
	if (CachedCharacter->GetClass()->ImplementsInterface(UCharacterDataInterface::StaticClass()))
	{
		ICharacterDataInterface::Execute_SetMovementState(
			CachedCharacter,
			EACMovementState::Ragdoll
		);
	}
	
	CachedASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
	
	CachedCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CachedCharacter->GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	CachedCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CachedCharacter->GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("hip"),true,true);
	CachedCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2);
}

void UAC_RagdollComponent::EndRagdoll()
{
	if (!CachedCharacter) return;

	USkeletalMeshComponent* Mesh = CachedCharacter->GetMesh();
	UAnimInstance* AnimInst = Mesh ? Mesh->GetAnimInstance() : nullptr;

	if (!Mesh || !AnimInst) return;

	// Save the ragdoll pose
	AnimInst->SavePoseSnapshot(FName("RagdollPose"));

	CachedCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	CachedCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECC_Pawn);
	Mesh->SetAllBodiesSimulatePhysics(false);

	// --- PLAY GET-UP ANIMATION ---
	UAnimMontage* MontageToPlay = bRagdollFaceUp ? GetUpFaceUpMontage : GetUpFaceDownMontage;
	float MontageDuration = 0.0f;

	if (MontageToPlay)
	{
		float PlayLength = AnimInst->Montage_Play(MontageToPlay);
		MontageDuration = MontageToPlay->GetPlayLength();
	}

	// --- REMOVE STUN TAG ONLY AFTER MONTAGE FINISHES ---
	if (CachedASC && MontageDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			RagdollGetUpTimer,
			this,
			&UAC_RagdollComponent::ClearStunTag,
			MontageDuration,
			false
		);
	}
}

/*void UAC_RagdollComponent::UpdateRagdoll()
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
	SetActorLocationAndRotation(HipLocation, NewRotation);#1#
}*/

void UAC_RagdollComponent::ClearStunTag()
{
	if (CachedASC)
	{
		CachedASC->RemoveLooseGameplayTag(
			FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun"))
		);
	}
}

void UAC_RagdollComponent::StartRagdollDelayed(float Delay)
{
	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().SetTimer(
		RagdollDelayHandle,
		this,
		&UAC_RagdollComponent::StartRagdoll,
		Delay,
		false
	);
}