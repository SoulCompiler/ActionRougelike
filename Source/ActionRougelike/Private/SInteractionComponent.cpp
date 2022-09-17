// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "SGameplayInterface.h"

// 控制台变量，控制是否开启DrawDebug
static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false,TEXT("Enable Debug Lines for Interact Component."),
                                                           ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceRadius = 30.0f;
	TraceDistance = 500.0f;
	CollisionChannel = ECC_WorldDynamic;
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void USInteractionComponent::FindBestInteractable()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	AActor* MyOwner = GetOwner(); // 返回调用者的指针

	FVector EyeLocation;
	FRotator EyeRotation;
	// 对于第一人称游戏要使用从眼睛出发的射线，对第三人称应该用从屏幕准星触发的射线
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation); // 获得Pawn（而非Camera）眼睛的Location和Rotation，这里的Rotation等于Pawn的ControlRotation。

	FVector End = EyeLocation + (EyeRotation.Vector() * TraceDistance);

	FCollisionObjectQueryParams ObjectQueryParams; // 碰撞查询参数
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // 查询Type为WorldDynamic的UObject

	// FHitResult Hit;
	// bool bBlockingHit =  GetWorld()->LineTraceSingleByObjectType(Hit,EyeLocation,End,ObjectQueryParams);		// 从Start坐标发射到End坐标结束的射线追踪，按ObjectQueryParam中的要求进行查询，查询到的UObject放入HitResult中，函数本身返回是否查询到物体。

	TArray<FHitResult> Hits; // T系列是UE自定义的数据结构

	FCollisionShape Shape;
	Shape.SetSphere(TraceRadius);
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	// 从Start坐标发射到End坐标结束的扫描，检测射线上一个形状范围内的所有物体，然后返回FHitResult的TArray数组

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	// Clear ref before trying to fill
	FocusedActor = nullptr;

	for (auto Hit : Hits)
	{
		if (bDebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, TraceRadius, 32, LineColor, false, 2.0f); // 可视化debug的球体k
		}

		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<USGameplayInterface>()) // 检测碰撞到Actor是否实现了交互函数
			{
				FocusedActor = HitActor;
				break;
			}
		}
	}

	if (FocusedActor)
	{
		if (DefaultWidgetInstance == nullptr && ensure(DefaultWidgetClass))
		{
			DefaultWidgetInstance = CreateWidget<USWorldUserWidget>(GetWorld(), DefaultWidgetClass);
		}

		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->AttachedActor = FocusedActor;


			if (!DefaultWidgetInstance->IsInViewport())
			{
				DefaultWidgetInstance->AddToViewport();
			}
		}
	}
	else
	{
		// @Todo:如果视线从一个actor移动到另一个actor，那原来的focusActor会移除Widget吗？
		if (DefaultWidgetInstance)
		{
			DefaultWidgetInstance->RemoveFromParent();
		}
	}

	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColor, false, 2.0f, 0, 2.0f); // 可视化debug的线条
	}
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindBestInteractable();
}

// 主动式的交互，作为Pawn的组件主动检测可交互物体并调用其交互函数
void USInteractionComponent::PrimaryInteract()
{
	if (FocusedActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No Focus Actor to interact.");
		return;
	}

	APawn* MyPawn = Cast<APawn>(GetOwner()); // 类型转换

	ISGameplayInterface::Execute_Interact(FocusedActor, MyPawn); // 调用者调用BlueprintNativeEvent函数的方式
}
