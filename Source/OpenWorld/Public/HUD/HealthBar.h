

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"


UCLASS()
class OPENWORLD_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

	public:
	UPROPERTY(meta= (BindWidget))
	class UProgressBar* HealthBar;
	
};
