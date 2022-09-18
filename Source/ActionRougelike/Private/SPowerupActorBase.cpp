// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActorBase.h"

// Sets default values
ASPowerupActorBase::ASPowerupActorBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Potion");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	CoolDownTime = 10.f;

	// 启动该类对象的同步，仅服务端生成该对象时向客户端同步，与之后发生的事无关
	// SetReplicates(true);
}

void ASPowerupActorBase::Interact_Implementation(APawn* InstigatorPawn)
{
}

void ASPowerupActorBase::ReSpawnPowerup()
{
	SetPowerupState(true);
}

void ASPowerupActorBase::HideAndCoolDownPowerup()
{
	SetPowerupState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_ReSpawn, this, &ASPowerupActorBase::ReSpawnPowerup, CoolDownTime);
}

void ASPowerupActorBase::SetPowerupState(bool bIsActive)
{
	SetActorEnableCollision(bIsActive);

	// Set Visibility on root and all children
	RootComponent->SetVisibility(bIsActive, true);
}
