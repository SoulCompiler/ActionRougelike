// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWorldUserWidget.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROUGELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USInteractionComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	void FindBestInteractable();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrimaryInteract();


	// Reliable：Will always arrive,eventually.Request will be re-sent unless an acknowledgment was received
	// Unreliable：Not guaranteed,packet can get lost and won't retry.
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InFocus	/* In表示输入方 */);

protected:
	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

	UPROPERTY()
	USWorldUserWidget* DefaultWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;

	// 不能使用枚举的原始名称作为成员变量的类型，而是使用TEnumAsByte或具有显式基础类型的c++ 11枚举类。
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
};
