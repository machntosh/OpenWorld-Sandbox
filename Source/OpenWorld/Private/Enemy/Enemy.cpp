
#include "Enemy/Enemy.h"
#include "AIController.h"
#include "SlashCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
// #include "GameFramework/CharacterMovementComponent.h"
#include "Components/CustomMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "CharacterTypes.h"
#include "Items/Soul.h"
#include "Items/Weapon.h"


AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ABaseCharacter::CharacterMovementComponentName))
{

	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	
	GetCharacterMovement()->bOrientRotationToMovement=true;
	bUseControllerRotationPitch=false;
	bUseControllerRotationYaw=false;
	bUseControllerRotationRoll= false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius= 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsDead()) return;

	if(EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
	// MoveToTarget(PatrolTarget);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	if(IsInsideAttackRadius()) EnemyState=EEnemyState::EES_Attacking;
	else if(IsOutSideAttackRadius()) StartChasing();
	return DamageAmount;
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	if (!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	StopAttackMontage();

	// if(IsInsideAttackRadius())
	// {
	// 	if(!IsDead())
	// 	{
	// 		StartAttackTimer();
	// 	}
	// }
}


void AEnemy::CheckPunch_Implementation(bool IsLeftHand)
{
	GEngine->AddOnScreenDebugMessage(-1,5.0f,FColor::Red,TEXT("Checking Punch"));
	FVector HandLocation;
	if(IsLeftHand)
	{
		 HandLocation = GetMesh()->GetBoneLocation("LeftHand",EBoneSpaces::WorldSpace);
	}
	else
	{
		HandLocation = GetMesh()->GetBoneLocation("RightHand",EBoneSpaces::WorldSpace);
	}
	ASlashCharacter* PlayerCharacter = Cast<ASlashCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	if(PlayerCharacter)
	{
		FName HitBone = PlayerCharacter->GetClosestBone(HandLocation,50.f);
		if(HitBone != "")
		{
			FVector ImpactPoint = PlayerCharacter->GetMesh()->GetBoneLocation(HitBone);
			UGameplayStatics::ApplyDamage(PlayerCharacter,20.f,GetInstigatorController(),this, UDamageType::StaticClass());
			if(PlayerCharacter->Attributes->IsAlive())
			{
				PlayerCharacter->DirectionalHitReact(ImpactPoint);
				
			}
			else
			{
					PlayerCharacter->DirectionalHitDeathReact(ImpactPoint);
				PlayerCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
		
	}
}


void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if(PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this,&AEnemy::PawnSeen);
	InitializeEnemy();

	
}


void AEnemy::SpawnSoul()
{
	UWorld* World=GetWorld();
	if(World && SoulClass && Attributes)
	{
		const FVector SpawnLocation= GetActorLocation() + FVector(0,0,125);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass,SpawnLocation,GetActorRotation());
		if(SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
			SpawnedSoul->SetOwner(this);
		}
	}
}

void AEnemy::PlayDirectionalHitDeathReactMontage(const FName& SectionName)
{
	EnemyState = EEnemyState::EES_Dead;
	Super::PlayDirectionalHitDeathReactMontage(SectionName);
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnSoul();
}

void AEnemy::DirectionalHitDeathReact_Implementation(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	//Lower the impact point to the Enemy's Z location
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit= (ImpactLowered - GetActorLocation()).GetSafeNormal();
	
	//Forward * ToHit = |Forward| * |ToHit| * cos(Theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(Theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	//Take the inverse cosine ( arc-cosine) of cos(Theta) to get Theta
	double Theta = FMath::Acos(CosTheta);
	//Convert from radians to degrees
	Theta= FMath::RadiansToDegrees(Theta);
	//If CrossProduct points down, then Theta is negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if(CrossProduct.Z < 0.f)
	{
		Theta = -Theta; //or Theta *= -1.f;
	}
	
	FName Section("DeathForward");
	DeathPose = EDeathPose::EDP_DeathForward; 
	if (Theta>= -45.f && Theta < 45.f)
	{
		Section = "DeathBackward";
		DeathPose = EDeathPose::EDP_DeathBackward;
	}
	else if (Theta >= -135.f && Theta <= -45.f)
	{
		Section = "DeathLeft";
		DeathPose = EDeathPose::EDP_DeathLeft;
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = "DeathRight";
		DeathPose = EDeathPose::EDP_DeathRight;
	}
	PlayDirectionalHitDeathReactMontage(Section);
	
}

void AEnemy::Attack()
{
	Super::Attack();
	if(CombatTarget== nullptr) return;
	
	EnemyState=EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack=
		IsInsideAttackRadius() &&
			!IsAttacking() &&
				!IsEngaged()&&
					!IsDead();
	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState=EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	
}

void AEnemy::InitializeEnemy()
{
	EnemyController=Cast<AAIController>(GetController());
	HideHealthBar();
	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapon();
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutSideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if(!IsEngaged()){StartPatrolling();}
	}
	else if(IsOutSideAttackRadius()&& !IsChasing())
	{
		ClearAttackTimer();
		if(!IsEngaged()){StartChasing();}
	}
	else if(CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if(InTargetRange(PatrolTarget,PatrolRadius))
	{
		PatrolTarget=ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin,PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer,this,&AEnemy::PatrolTimerFinished,WaitTime);
		MoveToTarget(PatrolTarget);
	}
}

void AEnemy::HideHealthBar()
{
	if(HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if(HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
	
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState=EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed=PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartChasing()
{
	EnemyState=EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed=ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutSideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutSideAttackRadius()
{
	return !InTargetRange(CombatTarget,AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget,AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState==EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState=EEnemyState::EES_Attacking;
	const float AttackTime= FMath::RandRange(AttackMin,AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer,this,&AEnemy::Attack,AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)

{
	if(Target==nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation()- GetActorLocation()).Size();
	// DRAW_SPHERE_SINGLE_FRAME(GetActorLocation());
	// DRAW_SPHERE_SINGLE_FRAME(Target->GetActorLocation());
	return DistanceToTarget<= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if(EnemyController==nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*>ValidTargets;
	for (AActor* Target :PatrolTargets)
	{
		if(Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	const int32 NumPatrolTargets = ValidTargets.Num();
	if(NumPatrolTargets>0)
	{
		const int32 TargetSelection= FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState< EEnemyState::EES_Chasing &&
			EnemyState != EEnemyState::EES_Chasing &&
			EnemyState != EEnemyState::EES_Dead &&
				SeenPawn->ActorHasTag(FName("EngageableTarget"));

	if (bShouldChaseTarget)
	{
		CombatTarget=SeenPawn;
		ClearPatrolTimer();
		StartChasing();
	}
	
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}