

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CustomMovementComponent.generated.h"

class UAnimMontage;
class UAnimInstance;

UENUM(BlueprintType)
namespace ECustomMovementMode
{
	enum Type
	{
		Move_Climb UMETA(DisplayName = "Climb Mode")
	};
}
/**
 * 
 */
UCLASS()
class OPENWORLD_API UCustomMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()


protected:
#pragma region OverridenFunctions
	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual FVector ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity, const FVector& CurrentVelocity) const override;
	
#pragma endregion
	
private:
#pragma region ClimbTraces

	TArray<FHitResult> DoCapsuleTraceMultiByObject(const FVector& Start, const FVector&End , bool bShowDebugShape=false, bool bDrawPersistantShapes = false );
	FHitResult DoLineTraceSingleByObject(const FVector& Start, const FVector&End , bool bShowDebugShape=false, bool bDrawPersistantShapes = false);
#pragma endregion
	
#pragma region ClimbCore

	bool TraceClimbableSurfaces();
	FHitResult TraceFromEyeHeight(float TraceDistance,float TraceStartOffset = 0.f );
	bool CanStartClimbing();

	void StartClimbing();
	void StopClimbing();

	void PhysClimb(float deltaTime, int32 Iterations);

	void ProcessClimableSurfaceInfo();

	bool CheckShouldStopClimbing();

	bool CheckHasReachedFloor();

	bool CheckHasReachedLedge();

	bool CanClimbDownLedge();
	
	FQuat GetClimbRotation(float DeltaTime);

	void SnapMovementToClimbaleSurfaces(float DeltaTime);

	void PlayClimbMontage(UAnimMontage* MontageToPlay);

	UFUNCTION()
	void OnClimbMontageEnded(UAnimMontage*Montage, bool bInterrupted);
	
#pragma endregion

#pragma region ClimCoreVariables
	TArray<FHitResult> ClimbableSurfacesTracedResults;

	FVector CurrentClimableSurfaceLocation;
	FVector CurrentClimableSurfaceNormal;

	UPROPERTY()
	UAnimInstance* OwningPlayerAnimInstance;
	
#pragma endregion

	
	
#pragma region ClimbBPVariables

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery> >ClimbableSurfaceTraceTypes;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	float ClimbCapsuleTraceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	float ClimbCapsuleTraceHalfHeight = 72.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	float MaxBreakClimbDeceleration = 400.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	float MaxClimbSpeed = 100.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	float MaxClimbAcceleration = 400.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	float ClimbDownWalkableSurfaceTraceOffset = 50.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	float ClimbDownLedgeTraceOffset = 50.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	UAnimMontage* IdleToClimbMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbToTopMontage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Character Movement: Climbing",meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ClimbDownLedgeMontage;
#pragma endregion

public:
	void ToggleClimbing(bool bEnableClimb);
	bool IsClimbing() const;
	FORCEINLINE FVector GetClimbableSurfaceNormal() const {return CurrentClimableSurfaceNormal;}
	FVector GetUnrotatedClimbVelocity() const;
};
