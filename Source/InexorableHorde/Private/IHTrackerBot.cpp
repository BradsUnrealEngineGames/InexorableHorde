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

FVector AIHTrackerBot::GetNextPathPoint()
{
	// Shortcut to get player location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

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
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Implement server only functionality
	if (Role == ROLE_Authority)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		// Apply Damage!
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		// Delete Actor after set time
		SetLifeSpan(0.5f);
	}
}

void AIHTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

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

void AIHTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
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