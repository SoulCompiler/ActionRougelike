// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "DrawDebugHelpers.h"
#include "SActionComponent.h"
#include "SGameplayFunctionLibrary.h"
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

		// 为什么不用硬编码？每次查询Tag都要经过GameplayTag系统查询，性能消耗；就算static驻留在内存中也会导致代码框架不雅观。
		// static FGameplayTag ParryTag = FGameplayTag::RequestGameplayTag("Status.Parrying");

		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			// @Warning: 如果两个人都有Parry Tag，子弹就会无限反弹，防止这个现象
			MoveComp->Velocity = -MoveComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));
			return;
		}

		// 2.0版本：静态函数
		// @cmt22: 应用1.0版本的客户端限制后，return false导致该分支无法进入
		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, DamageAmount, SweepResult))
		{
			Explode();

			// @Doubt: Explode()中的Destroy不是已经被调用了吗？为什么还能执行以下函数？搞清楚Destroy函数的实现。
			if (ActionComp && HasAuthority())
			{
				ActionComp->AddAction(GetInstigator(), BurningActionClass);
			}
		}
	}
}


void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	// Super::OnActorHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 100, 12, FColor::Red, false, 1.0);

	Explode();
}
