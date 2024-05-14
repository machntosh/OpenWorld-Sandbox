


#include "Components/CustomMovementComponent.h"

#include "SlashCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OpenWorld/DebugMacros.h"


void UCustomMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayerAnimInstance =  CharacterOwner->GetMesh()->GetAnimInstance();

	if(OwningPlayerAnimInstance)
	{
		OwningPlayerAnimInstance->OnMontageEnded.AddDynamic(this,&UCustomMovementComponent::OnClimbMontageEnded);
		OwningPlayerAnimInstance->OnMontageBlendingOut.AddDynamic(this,&UCustomMovementComponent::OnClimbMontageEnded);
	}
	
}

void UCustomMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CanClimbDownLedge();
}


void UCustomMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if(IsClimbing())
	{
		bOrientRotationToMovement = false;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(48.f);
	}

	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::Move_Climb)
	{
		bOrientRotationToMovement = true;
		CharacterOwner->GetCapsuleComponent()->SetCapsuleHalfHeight(88.f);
		const FRotator DirtyRotation = UpdatedComponent->GetComponentRotation();
		const FRotator CleanStandRotation = FRotator(0.f,DirtyRotation.Yaw,0.f);
		UpdatedComponent->SetRelativeRotation(CleanStandRotation);
		
		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}


void UCustomMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if(IsClimbing())
	{
		PhysClimb(deltaTime,Iterations);
	}
	
	Super::PhysCustom(deltaTime, Iterations);
}

float UCustomMovementComponent::GetMaxSpeed() const
{
	if(IsClimbing())
	{
		return MaxClimbSpeed;
	}
	else
	{
		
		return Super::GetMaxSpeed();
	}
}

float UCustomMovementComponent::GetMaxAcceleration() const
{
	if(IsClimbing())
	{
		return MaxClimbAcceleration;
	}
	else
	{
		return Super::GetMaxAcceleration();
	}
}

FVector UCustomMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity,
	const FVector& CurrentVelocity) const
{
	const bool bIsPlayingRMMontage = IsFalling() &&OwningPlayerAnimInstance && OwningPlayerAnimInstance->IsAnyMontagePlaying();
	if(bIsPlayingRMMontage)
	{
		return RootMotionVelocity;
	}
	else
	{
		return Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
	}
}

#pragma region ClimbTraces
TArray<FHitResult> UCustomMovementComponent::DoCapsuleTraceMultiByObject(const FVector& Start, const FVector& End,
                                                                         bool bShowDebugShape, bool bDrawPersistantShapes)
{
	TArray<FHitResult> OutCapsuleTraceHitResults;
	EDrawDebugTrace::Type DebugTraceType=EDrawDebugTrace::None;
	if(bShowDebugShape)
	{
		DebugTraceType=EDrawDebugTrace::ForOneFrame;
		if(bDrawPersistantShapes)
		{
			DebugTraceType=EDrawDebugTrace::Persistent;
		}
	}
	UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		this,
		Start,
		End,
		ClimbCapsuleTraceRadius,
		ClimbCapsuleTraceHalfHeight,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutCapsuleTraceHitResults,
		false);

	return OutCapsuleTraceHitResults;
}
FHitResult UCustomMovementComponent::DoLineTraceSingleByObject(const FVector& Start, const FVector& End,
	bool bShowDebugShape, bool bDrawPersistantShapes)
{
	FHitResult OutHit;
	
	EDrawDebugTrace::Type DebugTraceType=EDrawDebugTrace::None;
	
	if(bShowDebugShape)
	{
		DebugTraceType=EDrawDebugTrace::ForOneFrame;
		
		if(bDrawPersistantShapes)
		{
			DebugTraceType=EDrawDebugTrace::Persistent;
		}
	}
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ClimbableSurfaceTraceTypes,
		false,
		TArray<AActor*>(),
		DebugTraceType,
		OutHit,
		false
		);
	return OutHit;
}
#pragma endregion

#pragma region Climbcore
void UCustomMovementComponent::ToggleClimbing(bool bEnableClimb)
{
	if(bEnableClimb)
	{
		if(CanStartClimbing())
		{
			PlayClimbMontage(IdleToClimbMontage);
		}
		else if(CanClimbDownLedge())
		{
			PlayClimbMontage(ClimbDownLedgeMontage);
		}
	}
	else
	{
		//Stop Climbing
		StopClimbing();
	}
}

bool UCustomMovementComponent::CanStartClimbing()
{
	if(IsFalling()) return false;
	if(!TraceClimbableSurfaces()) return false;
	if(!TraceFromEyeHeight(100.f).bBlockingHit) return false;

	return true;
	
}

bool UCustomMovementComponent::CanClimbDownLedge()
{
	if(IsFalling()) return false;

	const FVector ComponentLocation = UpdatedComponent -> GetComponentLocation();
	const FVector ComponenForward = UpdatedComponent -> GetForwardVector();
	const FVector DownVector= -UpdatedComponent->GetUpVector();

	const FVector WalkableSurfaceTraceStart = ComponentLocation + ComponenForward *	ClimbDownWalkableSurfaceTraceOffset ;
	const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;

	FHitResult WalkableSurfaceHit=  DoLineTraceSingleByObject(WalkableSurfaceTraceStart,WalkableSurfaceTraceEnd,true);

	const FVector LedgeTraceStart = WalkableSurfaceHit.TraceStart + ComponenForward * ClimbDownLedgeTraceOffset;
	const FVector LedgeTraceEnd = LedgeTraceStart + DownVector * 300.f;

	 FHitResult LedgeTraceHit =   DoLineTraceSingleByObject(LedgeTraceStart,LedgeTraceEnd,true);

	if(WalkableSurfaceHit.bBlockingHit && !LedgeTraceHit.bBlockingHit)
	{
		return true;
	}
		return false;
}


void UCustomMovementComponent::StartClimbing()
{
	SetMovementMode(MOVE_Custom,ECustomMovementMode::Move_Climb);
	
}

void UCustomMovementComponent::StopClimbing()
{
	SetMovementMode(MOVE_Falling);
}

void UCustomMovementComponent::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	/*Process all the Climbable surfaces info*/
	TraceClimbableSurfaces();
	ProcessClimableSurfaceInfo();

	
	/*Check if we should stop climbing*/
	if(CheckShouldStopClimbing() || CheckHasReachedFloor() )
	{
		StopClimbing();
	}
	
	RestorePreAdditiveRootMotionVelocity();

	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		//Define the max climb speed and acceleratiom
		CalcVelocity(deltaTime, 0.f, true, MaxBreakClimbDeceleration);
	}

	ApplyRootMotionToVelocity(deltaTime);

	
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);

	//Handle climb rotation
	SafeMoveUpdatedComponent(Adjusted, GetClimbRotation((deltaTime)), true, Hit);

	if (Hit.Time < 1.f)
	{
		//adjust and try again
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
		
	
	if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() )
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	/*Snap movement to climbable surface*/
	SnapMovementToClimbaleSurfaces(deltaTime);

	if(CheckHasReachedLedge())
	{
		PlayClimbMontage(ClimbToTopMontage);
	}
}

void UCustomMovementComponent::ProcessClimableSurfaceInfo()
{
	CurrentClimableSurfaceLocation = FVector::ZeroVector;
	CurrentClimableSurfaceNormal = FVector::ZeroVector;

	if(ClimbableSurfacesTracedResults.IsEmpty()) return;

	for(const FHitResult& TracedHitResult:ClimbableSurfacesTracedResults)
	{
		CurrentClimableSurfaceLocation += TracedHitResult.ImpactPoint;
		CurrentClimableSurfaceNormal += TracedHitResult.ImpactNormal;
	}
	CurrentClimableSurfaceLocation /= ClimbableSurfacesTracedResults.Num();
	CurrentClimableSurfaceNormal = CurrentClimableSurfaceNormal.GetSafeNormal();


}

bool UCustomMovementComponent::CheckShouldStopClimbing()
{
	if(ClimbableSurfacesTracedResults.IsEmpty()) return true;
	const float DotResult= FVector::DotProduct(CurrentClimableSurfaceNormal, FVector::UpVector);
	const float DegreeDiff = FMath::RadiansToDegrees(FMath::Acos(DotResult));

	if(DegreeDiff<= 60.f)
	{
		return true;
	}
	

	return false;
}

bool UCustomMovementComponent::CheckHasReachedFloor()
{
	const FVector DownVector = -UpdatedComponent->GetUpVector();
	const FVector StartOffset = DownVector * 50.f;

	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + DownVector;

	TArray<FHitResult> PossibleFloorHits = DoCapsuleTraceMultiByObject(Start,End);

	if(PossibleFloorHits.IsEmpty()) return false;

	for(const FHitResult& PossibleFloorHit:PossibleFloorHits)
	{	
		const bool bFloorReached =
		FVector::Parallel(-PossibleFloorHit.ImpactNormal,FVector::UpVector) &&
		GetUnrotatedClimbVelocity().Z<-10.f;

		if(bFloorReached)
		{
			return true;
		}
	}

	return false;
}

bool UCustomMovementComponent::CheckHasReachedLedge()
{
	FHitResult LedgetHitResult = TraceFromEyeHeight(100.f,50.f);

	if(!LedgetHitResult.bBlockingHit)
	{
		const FVector WalkableSurfaceTraceStart = LedgetHitResult.TraceEnd;

		const FVector DownVector = -UpdatedComponent->GetUpVector();
		const FVector WalkableSurfaceTraceEnd = WalkableSurfaceTraceStart + DownVector * 100.f;

		FHitResult WalkableSurfaceHitResult =
		DoLineTraceSingleByObject(WalkableSurfaceTraceStart,WalkableSurfaceTraceEnd);

		if(WalkableSurfaceHitResult.bBlockingHit && GetUnrotatedClimbVelocity().Z > 10.f)
		{
			return true;
		}
	}

	return false;
}




FQuat UCustomMovementComponent::GetClimbRotation(float DeltaTime)
{
	FQuat CurrentQuat = UpdatedComponent->GetComponentQuat();
	if(HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return CurrentQuat;
	}
	const FQuat TargetQuat = FRotationMatrix::MakeFromX(-CurrentClimableSurfaceNormal).ToQuat();

	return FMath::QInterpTo(CurrentQuat,TargetQuat, DeltaTime,5.f);
	
}

void UCustomMovementComponent::SnapMovementToClimbaleSurfaces(float DeltaTime)
{
	const FVector ComponentForward= UpdatedComponent->GetForwardVector();
	const FVector ComponentLocation=UpdatedComponent->GetComponentLocation();

	const FVector ProjectedCharacterToSurface =
		(CurrentClimableSurfaceLocation - ComponentLocation ).ProjectOnTo(ComponentForward);

	const FVector SnapVector = -  CurrentClimableSurfaceNormal * ProjectedCharacterToSurface.Length();

	UpdatedComponent->MoveComponent(
		SnapVector*DeltaTime*MaxClimbSpeed,
		UpdatedComponent->GetComponentQuat(),
		true);
}

void UCustomMovementComponent::PlayClimbMontage(UAnimMontage* MontageToPlay)
{
	if(!MontageToPlay)return;
	if(!OwningPlayerAnimInstance) return;
	if(OwningPlayerAnimInstance->IsAnyMontagePlaying()) return;

	OwningPlayerAnimInstance->Montage_Play(MontageToPlay);
}

void UCustomMovementComponent::OnClimbMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == IdleToClimbMontage || Montage== ClimbDownLedgeMontage )
	{
		StartClimbing();
		StopMovementImmediately();
	}
	if(Montage == ClimbToTopMontage)
	{
		SetMovementMode(MOVE_Walking);
	}
}

bool UCustomMovementComponent::IsClimbing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == ECustomMovementMode::Move_Climb;
	
}

FVector UCustomMovementComponent::GetUnrotatedClimbVelocity() const
{
	return UKismetMathLibrary::Quat_UnrotateVector(UpdatedComponent->GetComponentQuat(),Velocity);
}

// Trace for climbable surfaces.return true of there are indeed valid surfaces, false otherwise
bool UCustomMovementComponent::TraceClimbableSurfaces()
{

	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 30.f;
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();
	
	ClimbableSurfacesTracedResults=DoCapsuleTraceMultiByObject(Start,End);

	return !ClimbableSurfacesTracedResults.IsEmpty();
}

FHitResult UCustomMovementComponent::TraceFromEyeHeight(float TraceDistance, float TraceStartOffset)
{
	const FVector ComponentLocation = UpdatedComponent->GetComponentLocation();
	const FVector EyeHeightOffset = UpdatedComponent->GetUpVector() * (CharacterOwner->BaseEyeHeight + TraceStartOffset);
	const FVector Start = ComponentLocation + EyeHeightOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector() * TraceDistance;
	
	return DoLineTraceSingleByObject(Start,End);
}



#pragma endregion
