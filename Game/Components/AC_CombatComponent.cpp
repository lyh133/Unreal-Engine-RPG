#include "dazAda/Game/Components/AC_CombatComponent.h"
#include "Components/SphereComponent.h"
#include "dazAda/Game/Characters/AcBaseCharacter.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UAC_CombatComponent::UAC_CombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // ✅ Create subobjects in the constructor (safe place)
    FistLeftHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("FistLeftHitbox"));
    FistRightHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("FistRightHitbox"));
    FootRightHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("FootRightHitbox"));
    FootLeftHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("FootLeftHitbox"));

    // Optional: Set defaults
    const float DefaultRadius = 10.0f;
    FistLeftHitbox->SetSphereRadius(DefaultRadius);
    FistRightHitbox->SetSphereRadius(DefaultRadius);
    FootRightHitbox->SetSphereRadius(DefaultRadius);
    FootLeftHitbox->SetSphereRadius(DefaultRadius);

    // Disable collision by default
    FistLeftHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FistRightHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FootRightHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FootLeftHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FistLeftHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
    FistRightHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
    FootRightHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
    FootLeftHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);

    FistLeftHitbox->SetVisibility(false);
    FistRightHitbox->SetVisibility(false);
    FootRightHitbox->SetVisibility(false);
    FootLeftHitbox->SetVisibility(false);
}

// Called when the game starts
void UAC_CombatComponent::BeginPlay()
{
    Super::BeginPlay();

    CachedCharacter = Cast<AAcBaseCharacter>(GetOwner());
    if (!CachedCharacter)
        return;

    USkeletalMeshComponent* CharacterMesh = CachedCharacter->GetMesh();
    if (!CharacterMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Character mesh not found for %s!"), *CachedCharacter->GetName());
        return;
    }

    CachedASC = CachedCharacter->GetAbilitySystemComponent();

    // ✅ Attach hitboxes to sockets at runtime
    FistLeftHitbox->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("l_handSocket"));
    FistRightHitbox->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("r_handSocket"));
    FootLeftHitbox->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("l_footSocket"));
    FootRightHitbox->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("r_footSocket"));

    // Example: spawn equipped weapon (currently commented out)
    if (EquippedWeaponClass)
    {
        // FActorSpawnParameters Params;
        // Params.Owner = GetOwner();
        // Params.Instigator = Cast<APawn>(GetOwner());

        // EquippedWeapon = GetWorld()->SpawnActor<APGAS_WeaponBase>(
        //     EquippedWeaponClass,
        //     GetOwner()->GetActorTransform(),
        //     Params
        // );

        // if (ACharacter* CharOwner = Cast<ACharacter>(GetOwner()))
        // {
        //     EquippedWeapon->AttachToComponent(CharOwner->GetMesh(),
        //         FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        //         FName("RightHandSocket"));
        // }
    }
}

void UAC_CombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CachedASC)
    {
        // cleanup if needed
    }

    Super::EndPlay(EndPlayReason);
}

// ==========================
// Getter Functions
// ==========================
USphereComponent* UAC_CombatComponent::GetFistLeftHitbox() const { return FistLeftHitbox; }
USphereComponent* UAC_CombatComponent::GetFootRightHitbox() const { return FootRightHitbox; }
USphereComponent* UAC_CombatComponent::GetFistRightHitbox() const { return FistRightHitbox; }
USphereComponent* UAC_CombatComponent::GetFootLeftHitbox() const { return FootLeftHitbox; }

// ==========================
// Trace Events
// ==========================
void UAC_CombatComponent::LeftFistTraceEvent()
{
    PerformTrace(FistLeftHitbox, OnFistTraceFinished);
}

void UAC_CombatComponent::RightFistTraceEvent()
{
    PerformTrace(FistRightHitbox, OnFistTraceFinished);
}

void UAC_CombatComponent::LeftFootTraceEvent()
{
    PerformTrace(FootLeftHitbox, OnFootTraceFinished);
}

void UAC_CombatComponent::RightFootTraceEvent()
{
    PerformTrace(FootRightHitbox, OnFootTraceFinished);
}

void UAC_CombatComponent::PerformTrace(USphereComponent* Hitbox, FOnFistTraceFinished& Delegate)
{
    if (!Hitbox || !CachedCharacter) return;

    TArray<FHitResult> HitResults;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CachedCharacter);

    FVector Start = Hitbox->GetComponentLocation() - GetOwner()->GetActorForwardVector() * 25.f;
    FVector End = Hitbox->GetComponentLocation() + GetOwner()->GetActorForwardVector() * 25.f;

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(5.f),
        Params
    );

    if (bHit)
    {
        Delegate.Broadcast(HitResults);
    }
}

void UAC_CombatComponent::PerformTrace(USphereComponent* Hitbox, FOnFootTraceFinished& Delegate)
{
    if (!Hitbox || !CachedCharacter) return;

    TArray<FHitResult> HitResults;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(CachedCharacter);

    FVector Start = Hitbox->GetComponentLocation() - GetOwner()->GetActorForwardVector() * 25.f;
    FVector End = Hitbox->GetComponentLocation() + GetOwner()->GetActorForwardVector() * 25.f;

    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Start,
        End,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(5.f),
        Params
    );

    if (bHit)
    {
        Delegate.Broadcast(HitResults);
    }
}

// ==========================
// Timer-based Trace Starters
// ==========================
void UAC_CombatComponent::StartRightFootTrace()
{
    StartTraceTimer(&UAC_CombatComponent::RightFootTraceEvent);
}

void UAC_CombatComponent::StopRightFootTrace()
{
    StopTraceTimer();
}

void UAC_CombatComponent::StartLeftFootTrace()
{
    StartTraceTimer(&UAC_CombatComponent::LeftFootTraceEvent);
}

void UAC_CombatComponent::StopLeftFootTrace()
{
    StopTraceTimer();
}

void UAC_CombatComponent::StartRightFistTrace()
{
    StartTraceTimer(&UAC_CombatComponent::RightFistTraceEvent);
}

void UAC_CombatComponent::StopRightFistTrace()
{
    StopTraceTimer();
}

void UAC_CombatComponent::StartLeftFistTrace()
{
    StartTraceTimer(&UAC_CombatComponent::LeftFistTraceEvent);
}

void UAC_CombatComponent::StopLeftFistTrace()
{
    StopTraceTimer();
}

// ==========================
// Timer Helpers
// ==========================
void UAC_CombatComponent::StartTraceTimer(void (UAC_CombatComponent::*TraceFunc)())
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            TraceFunc,
            0.1f,
            true
        );
    }
}

void UAC_CombatComponent::StopTraceTimer()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
        TimerHandle.Invalidate();
    }
}