// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActorBase.h"

#include "Net/UnrealNetwork.h"

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
	CreditsCost = 0.0f;

	// @Attention: 必须显式初始化为true！不然默认初始化为false，第一次调用SetPowerupState()时服务端向客户端同步的变量为false，客户端会跳过执行一次OnRep_IsActive()
	bIsActive = true;
	
	// 启动该类对象的同步
	SetReplicates(true);
}

FText ASPowerupActorBase::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return  FText::GetEmpty();
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

void ASPowerupActorBase::SetPowerupState(bool bNewIsActive)
{
	bIsActive = bNewIsActive;
	OnRep_IsActive();
}

void ASPowerupActorBase::OnRep_IsActive()
{
	SetActorEnableCollision(bIsActive);

	// Set Visibility on root and all children
	RootComponent->SetVisibility(bIsActive, true);
}

void ASPowerupActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActorBase, bIsActive);
}
