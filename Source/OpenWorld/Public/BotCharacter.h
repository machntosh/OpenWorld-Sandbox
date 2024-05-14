

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BotCharacter.generated.h"

UCLASS()
class OPENWORLD_API ABotCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	ABotCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

protected:
	
	virtual void BeginPlay() override;
	

};
