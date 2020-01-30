// Bradley Olson - GNU General Public License v3.0


#include "IHTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "IHHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"


// Sets default values
AIHTrackerBot::AIHTrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	HealthComp = CreateDefaultSubobject<UIHHealthComponent>(TEXT("HealthComp"));

	OverlapSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphereComp"));
	OverlapSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapSphereComp->SetupAttachment(RootComponent);
	OverlapSphereComp->SetSphereRadius(200);

	MovementForce = 1000;
	bUseVelocityChange = false;
	bExploded = false;
	bStartedSelfDestruction = false;
	RequiredDistanceToTarget = 100;
	ExplosionRadius = 200;
	ExplosionDamage = 100;
}

// Called when the game starts or when spawned
void AIHTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &AIHTrackerBot::HandleTakeDamage);

	if (Role == ROLE_Authority)
	{
		// Find initial move to
		NextPathPoint = GetNextPathPoint();
	}

	OverlapSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AIHTrackerBot::SmallSphereOverlap);

	// Every second we update our power level based on nearby bots
	FTimerHandle TimerHandle_CheckPowerLevel;
	GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &AIHTrackerBot::OnCheckNearbyBots, 1.0f, true);
}

void AIHTrackerBot::HandleTakeDamage(UIHHealthComponent* OwningHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on hitpoints == 0
	if (Health <= 0.0f)
	{
		SelfDestruct();
	}

	// Assign material pointer if it has not been assigned
	if (!MatInst)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
}

void AIHTrackerBot::SmallSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);
	if (!bStartedSelfDestruction && !bExploded)
	{
		if (PlayerPawn)
		{
			// We overlapped with a player!
			if (Role == ROLE_Authority)
			{
				// Start self destruction sequence
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &AIHTrackerBot::DamageSelf, 0.5f, true, 0.0f);
			}

			bStartedSelfDestruction = true;

			if (SelfDestructSound)
			{
				UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
			}

		}
	}
}

FVector AIHTrackerBot::GetNextPathPoint()
{
	// Shortcut to get player location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!PlayerPawn) { return; }

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (!NavPath) { return; }

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	// Failed to find path
	return GetActorLocation();
}

void AIHTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;

	// Play explosion visuals
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}

	// Spawn debug sphere
	//DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Yellow, false, 4.0f, 0, 1.0f);

	// Play explosion sound
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Implement server only functionality
	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		// Apply Damage!
		float FinalDamage = ExplosionDamage + (ExplosionDamage * 0.2 * PowerLevel);

		UGameplayStatics::ApplyRadialDamage(this, FinalDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		// Delete Actor after set time
		SetLifeSpan(0.5f);
	}
}

void AIHTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void AIHTrackerBot::OnCheckNearbyBots()
{
	// distance to check for nearby bots
	const float Radius = 600;

	// Create temporary collision shape for overlaps
	FCollisionShape CollShape;
	CollShape.SetSphere(Radius);

	// Only find Pawns (eg. players and AI bots)
	FCollisionObjectQueryParams QueryParams;
	// Our tracker bot's mesh component is set to Physics Body in Blueprint (default profile of physics simulated actors)
	QueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);

	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity, QueryParams, CollShape);

	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 12, FColor::White, false, 1.0f);

	int32 NrOfBots = 0;
	// loop over the results using a "range based for loop"
	for (FOverlapResult Result : Overlaps)
	{
		// Check if we overlapped with another tracker bot (ignoring players and other bot types)
		AIHTrackerBot* Bot = Cast<AIHTrackerBot>(Result.GetActor());
		// Ignore this trackerbot instance
		if (Bot && Bot != this)
		{
			NrOfBots++;
		}
	}

	const int32 MaxPowerLevel = 4;

	// Clamp between min=0 and max = 4
	PowerLevel = FMath::Clamp(NrOfBots, 0, MaxPowerLevel);

	// Update material color
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		float Alpha = PowerLevel / (float)MaxPowerLevel;

		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}

	DrawDebugString(GetWorld(), FVector(0), FString::FromInt(PowerLevel), this, FColor::White, 1.0f, true);
}

// Called every frame
void AIHTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Cancel if on client. Only server should be doing these calculations.
	if (Role != ROLE_Authority || bExploded) { return; }

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		// Keep moving toward next target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.GetSafeNormal();

		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	//DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f);
}