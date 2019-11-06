// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class UStaticMeshComponent;

UCLASS()
class INEXORABLEHORDE_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UStaticMeshComponent* MeshComp =  nullptr;

public:	

};
