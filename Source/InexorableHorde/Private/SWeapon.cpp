// Bradley Olson - GNU General Public License v3.0


#include "SWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "HAL/IConsoleManager.h"
#include "GameFramework/PlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "InexorableHorde.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines For Weapons"), ECVF_Cheat);

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(FName("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "BeamEnd";
	BaseDamage = 20.f;
	HeadshotMult = 3.0f;
	FireRate = 0.25f;
	LastTimeFired = 0.0f;
}

void ASWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastTimeFired + FireRate - GetWorld()->GetTimeSeconds(), 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASWeapon::Fire, FireRate, true, FirstDelay);
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

// Ray traces to find targets hit by weapon and delivers damage to them
void ASWeapon::Fire() {
	// Trace from center of camera crosshair

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + ShotDirection * 10000;
		FVector TracerEndPoint = TraceEnd;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			// Blocking hit! Process damage
			TracerEndPoint = Hit.ImpactPoint;

			AActor* HitActor = Hit.GetActor();

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			float DamageThisHit = BaseDamage;

			if (SurfaceType == SURFACE_FLESHVULNERABLE) {
				DamageThisHit *= HeadshotMult;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, DamageThisHit, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			// Finding impact effect to play
			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			// Playing impact effect
			if (SelectedEffect) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			LastTimeFired = GetWorld()->GetTimeSeconds();
		}

		if (DebugWeaponDrawing > 0) {
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);
	}
}

void ASWeapon::PlayFireEffects(FVector TracerEndPoint) {

	// Spawning muzzle flash
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	// Spawning tracer effect
	if (TracerEffect) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}

	// Adding camera shake
	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}