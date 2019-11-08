// Bradley Olson - GNU General Public License v3.0


#include "IHTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"

// Sets default values
AIHTrackerBot::AIHTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false);
}

// Called when the game starts or when spawned
void AIHTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
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

// Called every frame
void AIHTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}