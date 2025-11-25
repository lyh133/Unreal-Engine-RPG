// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "dazAda/Game/AbilitySystem/AcGameplayAbility.h"

// Sets default values
AWeapon::AWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	weaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	weaponBox->SetupAttachment(rootComponent);

	
	weaponBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	weaponBox->SetCollisionResponseToAllChannels(ECR_Ignore); // ignore everything
	weaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // only hit pawns

	weaponTop = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Top"));
	weaponTop->SetupAttachment(rootComponent);

	weaponBot = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Bot"));
	weaponBot->SetupAttachment(rootComponent);
	
}

void AWeapon::StartMeleeTrace()
{

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,      // Handle to control the timer
		this,             // Object that owns the function
		&AWeapon::SphereTraceEvent, // Function to call
		0.1f,             // Time interval (seconds)
		true              // Loop? true = repeated, false = one-shot
	);	
}

void AWeapon::StopMeleeTrace()
{
	if (GetWorld())
	{
		// Clear the timer using the handle
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

		// Optionally invalidate the handle (not strictly necessary, but good practice)
		TimerHandle.Invalidate();
	}
}

void AWeapon::SphereTraceEvent()
{

	TArray<FHitResult> HitResults;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner()); 
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		weaponTop->GetComponentLocation(),
		weaponBot->GetComponentLocation(),
		FQuat::Identity,
		ECC_Vehicle,
		FCollisionShape::MakeSphere(20.f),
		QueryParams
	);
	
	if (HitResults.Num() > 0)
	{
		OnWeaponTraceFinished.Broadcast(HitResults);
	}

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

