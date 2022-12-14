// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";

	AttackAnimDelay = 0.2f;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim); // 播放蒙太奇动画
		if (ensure(AttackVFX))
		{
			// 应该在所有客户端播放该特效
			UGameplayStatics::SpawnEmitterAttached(AttackVFX, Character->GetMesh(), HandSocketName, FVector::ZeroVector,
			                                       FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		}

		// @cmt22: Server区
		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);
			// GetWorldTimerManager()是AActor的函数
			// 高速重复多次的settimer会产生多个代理，导致stopAction被连续调用，触发ensure。(已解决)
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		// 已经将CameraComp->GetComponentLocation()包装在该函数中，因为CameraComp是protected的
		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();
		// @Todo: 改进射程的硬编码
		FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FHitResult Hit;

		// 射线追踪应该在客户端做，然后将结果发给服务端
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape,
		                                        Params))
		{
			TraceEnd = Hit.ImpactPoint;
			// DrawDebugLine(GetWorld(), SpawnLocation, ImpactLocation, FColor::Blue, false, 2.0f, 0, 2.0f);
		}


		FRotator TargetRotator = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();
		// 原理和UKismetMathLibrary::FindLookAtRotation相同
		FTransform SpawnTM(TargetRotator, HandLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = InstigatorCharacter;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// SpawnActor应该在服务端做
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}
