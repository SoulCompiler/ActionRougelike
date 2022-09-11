// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"

#include "SCharacter.h"
#include "GameFramework/Character.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionProfileName("Potion");
	RootComponent = MeshComp;

	HealingAmount = 20.0f;
	CoolDownTime = 10.0f;
}

// Called when the game starts or when spawned
void ASHealthPotion::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASHealthPotion::HideAndCoolDown()
{
	MeshComp->SetVisibility(false,true);
	SetActorEnableCollision(false);

	GetWorldTimerManager().SetTimer(TimerHandle_ReSpawn,this,&ASHealthPotion::ReSpawn,CoolDownTime);
}

void ASHealthPotion::ReSpawn()
{
	MeshComp->SetVisibility(true,false);
	SetActorEnableCollision(true);
}

// Called every frame
void ASHealthPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		if (AttributeComp->ApplyHealthChange(HealingAmount))
		{
			HideAndCoolDown();
		}
	}
}