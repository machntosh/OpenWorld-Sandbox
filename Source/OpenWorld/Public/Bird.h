

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Bird.generated.h"


class UInputAction;
class UCapsuleComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
UCLASS()
class OPENWORLD_API ABird : public APawn
{
	GENERATED_BODY()

public:
	
	ABird();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Input")
	const UInputMappingContext* BirdMappingContext;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Input")
	UInputAction* LookAction;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
private:
	UPROPERTY(VisibleAnywhere, Category ="Bird")
	UCapsuleComponent* Capsule;
	
	UPROPERTY(VisibleAnywhere,Category="Bird")
	USkeletalMeshComponent* BirdSkeletalMesh;
	
	 UPROPERTY(VisibleAnywhere,Category="Bird")
	 USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere,Category="Bird")
	UCameraComponent* Camera;
	

};
