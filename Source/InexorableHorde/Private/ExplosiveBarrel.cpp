// Bradley Olson - GNU General Public License v3.0


#include "ExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AExplosiveBarrel::AExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void AExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

