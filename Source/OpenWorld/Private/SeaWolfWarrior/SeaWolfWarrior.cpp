


#include "SeaWolfWarrior/SeaWolfWarrior.h"



ASeaWolfWarrior::ASeaWolfWarrior()
{
	
	PrimaryActorTick.bCanEverTick = true;
}


void ASeaWolfWarrior::BeginPlay()
{
	Super::BeginPlay();
	
}


void ASeaWolfWarrior::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ASeaWolfWarrior::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

