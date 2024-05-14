// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "SlashAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLD_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "Slash Anim Instance")
	class ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	class UCustomMovementComponent* SlashCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove;
	void GetShouldMove();

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float AirSpeed;
	void GetAirSpeed();

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
    	bool bIsClimbing;
	void GetIsClimbing();
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector ClimbVelocity;
	void GetClimbVelocity();

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EActionState ActionState;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	TEnumAsByte<EDeathPose> DeathPose;
};
