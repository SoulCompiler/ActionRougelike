// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	// // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;
	//
	// SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	//
	// // 以下是可以在代码中设置的碰撞规则
	// // SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	// // SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	// // SphereComp->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	//
	// SphereComp->SetCollisionProfileName("Projectile"); // 使用预设的配置文件作为该对象的碰撞规则，就不用在代码中一一设置了。
	// SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	// RootComponent = SphereComp; // 一般都把碰撞体作为根组件。
	//
	// EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	// EffectComp->SetupAttachment(SphereComp);
	//
	// MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	// MovementComp->InitialSpeed = 1000.0f;
	// MovementComp->bRotationFollowsVelocity = true;
	// MovementComp->bInitialVelocityInLocalSpace = true;

	SphereComp->OnComponentBeginOverlap.RemoveAll(this);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

	DamageAmount = 20.0f;
}

void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();


	SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);

	if (ensure(AudioComp->Sound))
	{
		AudioComp->Play();
	}
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		// 1.0版本：直接写逻辑
		// // USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		// // 由静态函数替代
		// USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(OtherActor);
		//
		// if (AttributeComp)
		// {
		// 	if (ensure(ImpactAFX))
		// 	{
		// 		UGameplayStatics::PlaySoundAtLocation(this, ImpactAFX, GetActorLocation(), GetActorRotation());
		// 	}
		// 	AttributeComp->ApplyHealthChange(GetInstigator(), -DamageAmount);
		// 	Explode();
		// }

		// 2.0版本：静态函数
		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();
		}
	}
}


void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Super::OnActorHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 100, 12, FColor::Red, false, 1.0);

	if (ensure(ImpactVFX) && ensure(ImpactAFX) && ensure(CameraShake))
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlaySoundAtLocation(this, ImpactAFX, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlayWorldCameraShake(this, CameraShake, GetActorLocation(), 0, 5000);
	}

	Destroy();
}
