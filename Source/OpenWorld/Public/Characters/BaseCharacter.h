

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "OpenWorld/Public/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;
class UCustomMovementComponent;

UCLASS()
class OPENWORLD_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void PlayDirectionalHitDeathReactMontage(const FName& SectionName);

	UFUNCTION(BlueprintNativeEvent)
	void DirectionalHitDeathReact(const FVector& ImpactPoint);

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

protected:
	
	virtual void BeginPlay() override;
	
	virtual void Attack();
	virtual void HandleDamage (float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	virtual bool CanAttack();
	bool IsAlive();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	/**Montage*/
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();
	
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose ;
	
	
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;

	
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	AActor* CombatTarget;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	double WarpTargetDistance= 75.f;
	
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection( UAnimMontage* Montage, const TArray<FName>& SectionNames);
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage = nullptr;
	UPROPERTY(EditAnywhere,Category="Combat")
	TArray<FName>AttackMontageSections;


private:
	
	/*
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly,Category="Montages")
	UAnimMontage* HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly,Category="Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly,Category="Montages")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere,Category="Combat")
	TArray<FName>DeathMontageSections;
	
	/**
	 * Particle systems
	 */
	UPROPERTY(EditAnywhere,Category="Combat")
	UParticleSystem* HitParticles;
	UPROPERTY(EditAnywhere,Category="Combat")
	USoundBase* HitSound;
public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const {return DeathPose;}
};
