// Bradley Olson - GNU General Public License v3.0


#include "ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "IHHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/Actor.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComp"));
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UIHHealthComponent>(FName("HealthComp"));

	ExplosionRadius = 800;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(FName("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->ImpulseStrength = 10000;
	RadialForceComp->Radius = ExplosionRadius;

	bUnexploded = true;
	ExplosionDamage = 100;
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &AExplosiveBarrel::OnHealthChanged);
	}
	
}

void AExplosiveBarrel::OnHealthChanged(UIHHealthComponent* OwnerHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0 && bUnexploded)
	{
		bUnexploded = false;

		// Change barrel material
		if (MatExploded)
		{
			MeshComp->SetMaterial(0, MatExploded);
		}

		// Spawn particle effect at barrel location
		if (ExplosionSystem)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionSystem, GetActorTransform());
		}

		// Use radial force component at barrel location
		RadialForceComp->FireImpulse();

		// Apply radial damage
		TArray<AActor*> Ignore;
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, ExplosionDamageType, Ignore, this);
	}
}

