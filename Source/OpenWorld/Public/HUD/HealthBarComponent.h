

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"


UCLASS()
class OPENWORLD_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
	public:
	
	void SetHealthPercent(float HealthPercent);

private:
	UPROPERTY()
	class UHealthBar* HealthBarWidget;
};
