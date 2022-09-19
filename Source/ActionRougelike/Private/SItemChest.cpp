// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ASItemChest::ASItemChest()
{
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110.0f;

	// 启动该类对象的同步，仅服务端生成该对象时向客户端同步，与之后发生的事无关
	SetReplicates(true);
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	// LidMesh->SetRelativeRotation(FRotator(TargetPitch, 0, 0)); // 设置相对于root组件的旋转。这个旋转是瞬间的，不是平滑动画

	// 该变量是Replicated的，只要一变化就会根据GetLifetimeReplicatedProps的同步属性向发送客户端该变量的更新
	bLidOpened = !bLidOpened;
	
	// 服务端在需要和客户端同步变量更新后的反应时，手动调用OnRep函数
	// 意思是既是客户端又是服务端时，要手动调用该函数让本地客户端的表现和其他客户端一致
	// 对于客户端，只要收到变量同步且实际更新了本地变量，就会自动调用OnRep函数
	OnRep_LidOpened();
}

void ASItemChest::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 当该变量在服务器中更新时，发给所有客户端
	DOREPLIFETIME(ASItemChest, bLidOpened);
}

// 客户端自动调用
void ASItemChest::OnRep_LidOpened()
{
	float CurrPitch = bLidOpened ? TargetPitch : 0.0f;
	LidMesh->SetRelativeRotation(FRotator(CurrPitch, 0, 0));
}
