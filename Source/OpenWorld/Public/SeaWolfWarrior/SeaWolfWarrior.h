
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "SeaWolfWarrior.generated.h"



class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UGroomComponent;
class UAnimMontage;
class AWeapon;


UCLASS()
class OPENWORLD_API ASeaWolfWarrior : public ACharacter
{
	GENERATED_BODY()

public:
	
	ASeaWolfWarrior();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category="Input")
	UInputMappingContext* SeaWolfWarriorContext;
	
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* MovementAction;
	
	UPROPERTY(EditAnywhere,Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DodgeAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EKeyPressedAction;

private :
	ECharacterState CharacterState= ECharacterState::ECS_Unequipped;
	
	UPROPERTY(BlueprintReadWrite, meta= (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere,Category="Slash")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Slash")
	UCameraComponent* Camera;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	/*
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;


public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item;}
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
	

	
};
