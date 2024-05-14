

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UGroomComponent;
class UAnimMontage;
class AWeapon;
class USlashOverlay;
class ASoul;
class ATreasure;


UCLASS()
class OPENWORLD_API ASlashCharacter : public ABaseCharacter , public IPickupInterface
{
	GENERATED_BODY()

public:
	
	ASlashCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	FName GetClosestBone(FVector HitBoneLocation, float BoneMaxDistance);

	// virtual void PlayDirectionalHitDeathReactMontage(const FName& SectionName) override;
	virtual void DirectionalHitDeathReact_Implementation(const FVector& ImpactPoint) override;

	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Gold) override;

protected:
	virtual void BeginPlay() override;
   /*
	* Callbacks for input actions
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void HandleGroundMovementInput(const FInputActionValue& Value);
	void HandleClimbMovementInput(const FInputActionValue& Value);
	bool IsUnoccupied();
	virtual void EKeyPressed();
	bool IsOccupied();
	bool HasEnoughStamina();
	void Dodge(const FInputActionValue& Value);
	void OnClimbActionStarted(const FInputActionValue& Value);

	
	/*Combat*/
	void Disarm();
	void Arm();
	void EquipWeapon(AWeapon*Weapon);
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	
	

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	
	UPROPERTY(EditAnywhere,Category="Input")
	UInputMappingContext* SlashContext;
	
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

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ClimbAction;

	

	
private:

	void InitializeSlashOverlay();
	void InitialiseSlashMappingContext();
	void SetHUDHealth();

	/**Character Components*/
	UPROPERTY(VisibleAnywhere,Category="Slash")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Slash")
	UCameraComponent* Camera;
	
	UPROPERTY(VisibleAnywhere, Category="Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	ECharacterState CharacterState= ECharacterState::ECS_Unequipped;
	
	UPROPERTY(BlueprintReadWrite, meta= (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	UCustomMovementComponent* CustomMovementComponent;

public:
	// FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item;}
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const {return ActionState;}
	FORCEINLINE EActionState SetActionState() const {return ActionState;}

	FORCEINLINE UCustomMovementComponent* GetCustomMovementComponent() const {return CustomMovementComponent;}
};

