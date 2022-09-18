// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "SWorldUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (ExposeOnSpawn=true))	// meta将该变量暴露为一个函数引脚
	AActor* AttachedActor;

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset; // UI显示位置的相对偏移，比如血量条应该在头顶上

protected:
	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;
};
