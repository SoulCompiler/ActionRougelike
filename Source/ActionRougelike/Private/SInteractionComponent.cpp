// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "SGameplayInterface.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 主动式的交互，作为Pawn的组件主动检测可交互物体并调用其交互函数
void USInteractionComponent::PrimaryInteract()
{
	AActor* MyOwner = GetOwner();		// 返回调用者的指针

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation,EyeRotation);		// 获得Pawn（而非Camera）眼睛的Location和Rotation，这里的Rotation等于Pawn的ControlRotation。

	FVector End = EyeLocation + (EyeRotation.Vector() * 1000);

	FCollisionObjectQueryParams ObjectQueryParams;		// 碰撞查询参数
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);		// 查询Type为WorldDynamic的UObject

	// FHitResult Hit;
	// bool bBlockingHit =  GetWorld()->LineTraceSingleByObjectType(Hit,EyeLocation,End,ObjectQueryParams);		// 从Start坐标发射到End坐标结束的射线追踪，按ObjectQueryParam中的要求进行查询，查询到的UObject放入HitResult中，函数本身返回是否查询到物体。

	TArray<FHitResult> Hits;		// T系列是UE自定义的数据结构
	float Radius = 30.f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits,EyeLocation,End,FQuat::Identity,ObjectQueryParams,Shape);		// 从Start坐标发射到End坐标结束的扫描，检测射线上一个形状范围内的所有物体，然后返回FHitResult的TArray数组
	
	FColor LineColor = bBlockingHit ? FColor::Red : FColor::Green;

	for (auto Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor)
		{
			if (HitActor->Implements<USGameplayInterface>())		// 检测碰撞到Actor是否实现了交互函数
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);		// 类型转换

				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);		// 调用者调用BlueprintNativeEvent函数的方式
			}
		}

		DrawDebugSphere(GetWorld(),Hit.ImpactPoint,Radius,32,LineColor,false,2.0f);		// 可视化debug的球体
	}
	
	DrawDebugLine(GetWorld(),EyeLocation,End,LineColor,false,2.0f,0,2.0f);		// 可视化debug的线条
}

