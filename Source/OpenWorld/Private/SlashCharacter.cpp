


#include "SlashCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/AttributeComponent.h"
#include "Components/CustomMovementComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "OpenWorld/DebugMacros.h"





ASlashCharacter::ASlashCharacter(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(ABaseCharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	//Set the use of the controller rotation to false
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CustomMovementComponent=Cast<UCustomMovementComponent>(GetCharacterMovement());
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.f,500.f,0.f);

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation=true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	//Initialize hair and eyebrows of SlashCharacter
	Hair=CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName= FString ("head");
	Eyebrows=CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

}

void ASlashCharacter::Tick(float DeltaTime)
{
	if(Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(UEnhancedInputComponent* EnhancedInputComponent= CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction,ETriggerEvent::Triggered,this,&ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered,this,&ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Triggered,this,&ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyPressedAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction,ETriggerEvent::Triggered,this,&ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &ASlashCharacter::OnClimbActionStarted);
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if(SlashOverlay && Attributes) SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint,Hitter);
	
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if(Attributes && Attributes->GetHealthPercent() > 0.f) ActionState = EActionState::EAS_HitReaction;
	
}

FName ASlashCharacter::GetClosestBone(FVector HitBoneLocation, float BoneMaxDistance)
{
	TArray<FName>BoneNames;
	GetMesh()->GetBoneNames(BoneNames);

	FName ClosestBone;
	float MinDistance= 10000;
	for(int i=0; i<BoneNames.Num(); i++)
	{
		FVector BoneLocation = GetMesh()->GetBoneLocation(BoneNames[i], EBoneSpaces::WorldSpace);
		float TempDistance = FVector::Dist(HitBoneLocation,BoneLocation);
		if(MinDistance > TempDistance )
		{
			MinDistance=TempDistance;
			ClosestBone=BoneNames[i];
		}
	}
	if(MinDistance<BoneMaxDistance)
		return ClosestBone;

	return {};
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController =   Cast<APlayerController>(GetController());
	if(PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if(SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if(SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
			}
			
		}
		
	}
}

void ASlashCharacter::InitialiseSlashMappingContext()
{
	if(APlayerController* PlayerController= Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext,0);
		}
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitialiseSlashMappingContext();	
	Tags.Add(FName("EngageableTarget"));
	InitializeSlashOverlay();
	CustomMovementComponent=Cast<UCustomMovementComponent>(GetCharacterMovement());
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if(!CustomMovementComponent) return;
	
	if(CustomMovementComponent->IsClimbing())
	{
		HandleClimbMovementInput(Value);
	}
	else
	{
		HandleGroundMovementInput(Value);
	}
	
	
	
}
void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector= Value.Get<FVector2D>();
	if(Controller !=nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
	    AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASlashCharacter::HandleGroundMovementInput(const FInputActionValue& Value)
{

	if (Controller != nullptr)
	{
		if(ActionState != EActionState::EAS_Unoccupied && ActionState != EActionState::EAS_Attacking)return;
		const FVector2D MovementVector= Value.Get<FVector2D>();
		const FRotator Rotation=Controller->GetControlRotation();
		const FRotator YawRotation(0,Rotation.Yaw,0);
		const FVector ForwardDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection,MovementVector.Y);
		const FVector RightDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection,MovementVector.X);
	}
}

void ASlashCharacter::HandleClimbMovementInput(const FInputActionValue& Value)
{
	if(Controller!=nullptr)
	{
		GetCharacterMovement()->bOrientRotationToMovement=false;
		GetCharacterMovement()->bUseControllerDesiredRotation=false;
		const FVector2D MovementVector= Value.Get<FVector2D>();
	
		const FVector ForwardDirection = FVector::CrossProduct(
		-CustomMovementComponent->GetClimbableSurfaceNormal(),
		GetActorRightVector()
		);
	AddMovementInput(ForwardDirection,MovementVector.Y);
	
	const FVector RightDirection = FVector::CrossProduct(
		-CustomMovementComponent->GetClimbableSurfaceNormal(),
		-GetActorUpVector()
		);
	AddMovementInput(RightDirection,MovementVector.X);
	}

	
};

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied()) Super::Jump();
	
}
void ASlashCharacter::EKeyPressed()
{

	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if(EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon((OverlappingWeapon));
	}
	else
	{
		if (CanDisarm()) Disarm();
		else if (CanArm()) Arm();
	}
}

bool ASlashCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void ASlashCharacter::Dodge(const FInputActionValue& Value)
{
	if(IsOccupied() || !HasEnoughStamina()) return;
	
	PlayDodgeMontage();
	ActionState= EActionState::EAS_Dodging;
	if (Attributes && SlashOverlay)
		{
			Attributes->UseStamina(Attributes->GetDodgeCost());
			SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
		}
}

void ASlashCharacter::OnClimbActionStarted(const FInputActionValue& Value)
{

	if(!CustomMovementComponent) return;
	if(!CustomMovementComponent->IsClimbing())
	{
		CustomMovementComponent->ToggleClimbing(true);
	}
	else
	{
		CustomMovementComponent->ToggleClimbing(false);
	}
	
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState=EActionState::EAS_Equipping;
}
void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState=EActionState::EAS_Equipping;
}
void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{

	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
	
}


void ASlashCharacter::Attack()
{
	Super::Attack();
	if (CanAttack()) 
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}
bool ASlashCharacter::CanAttack()
{	
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;	
}
void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState=EActionState::EAS_Unoccupied;
}

void ASlashCharacter::AttachWeaponToHand()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}
void ASlashCharacter::AttachWeaponToBack()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	
	}
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState== EActionState::EAS_Unoccupied && CharacterState!=ECharacterState::ECS_Unequipped;
}
bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped  && EquippedWeapon;
}

// void ASlashCharacter::PlayDirectionalHitDeathReactMontage(const FName& SectionName)
// {
// 	ActionState= EActionState::EAS_Dead;
// 	Super::PlayDirectionalHitDeathReactMontage(SectionName);
// 	
// }

void ASlashCharacter::DirectionalHitDeathReact_Implementation(const FVector& ImpactPoint)
{
	ActionState= EActionState::EAS_Dead;
	Super::DirectionalHitDeathReact_Implementation(ImpactPoint);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem= Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if(Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Gold)
{
	if(Attributes && SlashOverlay)
	{
		Attributes->AddGold(Gold->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName,EquipMontage);
	}
}
void ASlashCharacter::FinishEquipping()
{
	ActionState= EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState=EActionState::EAS_Unoccupied;
}







