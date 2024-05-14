


#include "Characters/BaseCharacter.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CustomMovementComponent.h"
#include "Items/Weapon.h"
#include "Kismet/GameplayStatics.h"



ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ABaseCharacter::CharacterMovementComponentName)), EquippedWeapon(nullptr), HitReactMontage(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
	
}

void ABaseCharacter::PlayDirectionalHitDeathReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		AnimInstance->Montage_JumpToSection(SectionName,DeathMontage);
	}
	
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,HitSound,ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if(HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),HitParticles,ImpactPoint);
	}
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName,HitReactMontage);
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection=  PlayRandomMontageSection(DeathMontage,DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if(Pose < EDeathPose::EDP_MAX)
	{
		DeathPose =Pose;
	}
	return Selection; 
	
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage,FName("Default"));
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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
	
	FName Section("FromBack");
	if (Theta>= -45.f && Theta < 45.f)
	{
		Section = "FromFront";
	}
	else if (Theta >= -135.f && Theta <= -45.f)
	{
		Section = "FromLeft";
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = "FromRight";
	}
	PlayHitReactMontage(Section);
	
	
}

void ABaseCharacter::DirectionalHitDeathReact_Implementation(const FVector& ImpactPoint)
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
	DeathPose = EDeathPose::EDP_DeathForward; //this is an enum defined in CharacterTypes. IT enables to switch between different death poses.
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
	Tags.Add(FName("Dead"));
	PlayDirectionalHitDeathReactMontage(Section);
	
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) AnimInstance->Montage_Stop(0.25f,AttackMontage);
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if(CombatTarget == nullptr)return {};
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe*=WarpTargetDistance;
	// DRAW_SPHERE(CombatTargetLocation+ TargetToMe); <-Activate to debug 
	
	return CombatTargetLocation + TargetToMe;
	
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if(CombatTarget)
	{
		return CombatTarget->GetActorLocation();
		
	}
	return FVector();
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if(IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else DirectionalHitDeathReact(ImpactPoint);
	
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance&& Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName,Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if(SectionNames.Num() <=0) {return -1;}
	const int32 MaxSectionIndex = SectionNames.Num() -1;
	const int32 Selection = FMath::RandRange(0,MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}


bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}


void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox() )
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}
  
