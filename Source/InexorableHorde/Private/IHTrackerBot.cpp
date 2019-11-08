// Bradley Olson - GNU General Public License v3.0


#include "IHTrackerBot.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AIHTrackerBot::AIHTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void AIHTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIHTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}