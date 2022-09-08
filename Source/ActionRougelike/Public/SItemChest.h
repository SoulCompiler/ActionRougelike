// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ASItemChest();

protected:
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh; // 箱子底座

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UStaticMeshComponent* LidMesh; // 箱子盖子


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;	// 因为UFUNCTION是Native的，所以实现和声明的函数名不一样

public:
	UPROPERTY(EditAnywhere)
	float TargetPitch;
};
