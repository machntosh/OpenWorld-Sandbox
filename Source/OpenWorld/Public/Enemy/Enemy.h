
#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "OpenWorld/Public/CharacterTypes.h"
#include "Enemy.generated.h"



class UPawnSensingComponent;
class AWeapon;
class UCustomMovementComponent;
UCLASS()
class OPENWORLD_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	
	AEnemy(const FObjectInitializer& ObjectInitializer);
	/**<AActor>*/
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/**</AActor>*/
	
	/**<IHitInterface>*/
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void CheckPunch_Implementation(bool IsLeftHand) override;
	/**</IHitInterface>*/
	
	TArray<AActor*> IgnoreActors;

protected:
	/**<AActor>*/
	virtual void BeginPlay() override;
	void SpawnSoul();
	/**</AActor>*/

	/**<ABaseCharacter>*/
	virtual void PlayDirectionalHitDeathReactMontage(const FName& SectionName) override;
	virtual void DirectionalHitDeathReact_Implementation(const FVector& ImpactPoint) override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage (float DamageAmount) override;
	// virtual int32 PlayDeathMontage() override;
	/**</ABaseCharacter>*/
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
	// virtual FName GetClosestBone(FVector HitBoneLocation, float BoneMaxDistance);

	

private:
	
	/*
	 *
	 * AI Behavior
	 */
	void InitializeEnemy();
	void PatrolTimerFinished();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void StartChasing();
	bool IsOutSideCombatRadius();
	bool IsOutSideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	
	AActor*ChoosePatrolTarget();

	
	
	UFUNCTION()
	void PawnSeen(APawn*SeenPawn); //Callback for OnPawnSeen in UPawnSensingComponent
	void SpawnDefaultWeapon();

	UPROPERTY()
	class AAIController* EnemyController;
	
	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AActor* PatrolTarget;
	
	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	TArray<AActor*> PatrolTargets;
	
	FTimerHandle PatrolTimer;
	
	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	float PatrolWaitMin = 5.f;
	
	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	float PatrolWaitMax = 10.f;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	float PatrollingSpeed = 125.f;
	
	FTimerHandle AttackTimer;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackMin = 0.5f;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackMax = 1.f;
	
	UPROPERTY(EditAnywhere,Category="Combat")
	float ChasingSpeed =300.f;
	
	UPROPERTY(EditAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;
	
	UPROPERTY(VisibleAnywhere)
	class UHealthBarComponent* HealthBarWidget;
	
	UPROPERTY(EditAnywhere)
	float DeathLifeSpan = 8.f;
	
	/**Combat*/
	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere)
	double AcceptanceRadius = 30.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASoul> SoulClass;
	
};
