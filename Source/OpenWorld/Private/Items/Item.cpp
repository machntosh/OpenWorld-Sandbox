


#include "Items/Item.h"
#include "Components/SphereComponent.h"
// #include "SlashCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
{
 	
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent=ItemMesh;
	
	Sphere=CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(RootComponent);

	ItemEffect=CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffect"));
	ItemEffect->SetupAttachment(RootComponent);
	
}


void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(TimeConstant * RunningTime);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(TimeConstant * RunningTime);
}

void AItem::SpawnPickupSystem()
{
	if(PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,PickupEffect,GetActorLocation());
	}
}

void AItem::PlayPickupSound()
{
	if(PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,PickupSound,GetActorLocation());
	}
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(this);
	}
	SpawnPickupSystem();
	
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface)
	{
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	//Checking if the item is hovering
	if(ItemState==EItemState::EIS_Hovering)
	{
		//Making the item float through a sinusoidal movement at every frame
		AddActorWorldOffset(FVector(0.f, 0.f,TransformedSin()));
	}
}


