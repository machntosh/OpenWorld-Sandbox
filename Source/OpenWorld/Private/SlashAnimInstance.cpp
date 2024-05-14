// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashAnimInstance.h"
#include "SlashCharacter.h"
#include "Components/CustomMovementComponent.h"


void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCustomMovementComponent();
	}


}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacter)
	{
		GroundSpeed = SlashCharacterMovement->Velocity.Size();
		IsFalling= SlashCharacterMovement->IsFalling();
		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
		DeathPose = SlashCharacter->GetDeathPose();
		GetShouldMove();
		GetAirSpeed();
		GetIsClimbing();
		GetClimbVelocity();
	}

}

void USlashAnimInstance::GetShouldMove()
{
	bShouldMove = SlashCharacterMovement->GetCurrentAcceleration().Size()>0 &&
		GroundSpeed>5.f &&
			!IsFalling;
}

void USlashAnimInstance::GetAirSpeed()
{
	AirSpeed = SlashCharacterMovement->Velocity.Z;
}

void USlashAnimInstance::GetIsClimbing()
{
	bIsClimbing = SlashCharacterMovement->IsClimbing();
}

void USlashAnimInstance::GetClimbVelocity()
{
	ClimbVelocity= SlashCharacterMovement->GetUnrotatedClimbVelocity();
}
