

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UTextRenderComponent;
class USphereComponent;
class USoundBase;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped,
};

UCLASS()
class OPENWORLD_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	

	AItem();
	virtual void Tick(float DeltaTime) override;


protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Properties")
	float Amplitude=0.25f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Properties")
	float TimeConstant=5.f;
	
	UFUNCTION(BlueprintPure, Category = "Sine Properties")
	float TransformedSin();
	
	UFUNCTION(BlueprintPure, Category = "Cosine Properties")
	float TransformedCos();
	
	virtual void SpawnPickupSystem();
	virtual void PlayPickupSound();
	

	template<typename T>
	static T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					                     int32 OtherBodyIndex);
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;
	
	UPROPERTY(VisibleAnywhere)
    	USphereComponent* Sphere;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* ItemEffect;
	
	
private:
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	float RunningTime;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;
	
};

template <typename T>
T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}
