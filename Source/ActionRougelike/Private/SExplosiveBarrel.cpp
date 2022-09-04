// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	MeshComp->SetSimulatePhysics(true);	
	MeshComp->SetCollisionProfileName("PhysicsActor");		// 在C++中将物理模拟设置为true以后还要设置碰撞规则，否则要在编辑器中手动设置

	ForceComp = CreateDefaultSubobject<URadialForceComponent>("ForceComp");
	ForceComp->SetupAttachment(MeshComp);
	ForceComp->ImpulseStrength = 2000.0f;
	ForceComp->Radius = 750.0f;
	ForceComp->bImpulseVelChange = true;		// 如果为假则物体质量不可忽略，这样就需要把脉冲强度设得很高

	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);		// 设置可响应的碰撞通道
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();		// 继承而来的用于初始化的虚函数一定要首先调用父类的函数。

	MeshComp->OnComponentHit.AddDynamic(this,&ASExplosiveBarrel::OnActorHit);		// 将对象和函数动态绑定到蓝图函数上，作为蓝图函数的多态执行。
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	ForceComp->FireImpulse();		// 执行发射脉冲的函数

	UE_LOG(LogTemp,Log,TEXT("OnActorHit reached (ExplosiveBarrel)"));	// 在编辑器的Log中打印此语句，确认触发这个事件。
}

