// Bradley Olson - GNU General Public License v3.0


#include "IHPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "IHPowerupActor.h"
#include "TimerManager.h"

// Sets default values
AIHPickupActor::AIHPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64, 75, 75);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 10.0f;
}

// Called when the game starts or when spawned
void AIHPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	Respawn();
}

void AIHPickupActor::Respawn()
{
	if (PowerupClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PowerUpClass is nullptr in %s. Please update your Blueprint"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<AIHPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void AIHPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (PowerupInstance)
	{
		PowerupInstance->ActivatePowerup();
		PowerupInstance = nullptr;

		// Set Timer to respawn
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupRespawn, this, &AIHPickupActor::Respawn, CooldownDuration);
	}
}

