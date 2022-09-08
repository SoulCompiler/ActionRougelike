// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASDashProjectile::ASDashProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");

	// 以下是可以在代码中设置的碰撞规则
	// SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	// SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// SphereComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);

	SphereComp->SetCollisionProfileName("Projectile"); // 使用预设的配置文件作为该对象的碰撞规则，就不用在代码中一一设置了。
	RootComponent = SphereComp; // 一般都把碰撞体作为根组件。
	
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASDashProjectile::Projectile_Explode, 0.5);
}

// Called every frame
void ASDashProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASDashProjectile::Projectile_Explode()
{
	MovementComp->StopMovementImmediately();
	
	UGameplayStatics::SpawnEmitterAtLocation(this, EmitterTemplate, GetActorLocation(), GetActorRotation(), false);

	GetWorldTimerManager().ClearTimer(TimerHandle_Dash);
	
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASDashProjectile::Dash_Effect, 0.5f);
}

void ASDashProjectile::Dash_Effect()
{
	APawn* ProjectileInstigator = GetInstigator();

	// FTransform DashTarget(Owner->GetActorRotation(),this->GetActorLocation());

	ProjectileInstigator->SetActorLocation(this->GetActorLocation());

	Destroy();
}
