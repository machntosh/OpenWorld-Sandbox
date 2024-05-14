


#include "BotCharacter.h"


ABotCharacter::ABotCharacter()
{
 	
	PrimaryActorTick.bCanEverTick = true;

}


void ABotCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABotCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABotCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

