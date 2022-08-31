// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);	// 将组件放到根部组件下（派生自actor的默认根部组件是CapsuleComponent）

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);		// 将组件放到弹簧臂的层级下
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(),Value);	// 该函数是基类Pawn的运动组件中自带的虚函数，根据输入的vector和value在向量方向进行运动。
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);		// 将MoveForward函数绑定到项目设置中Input下Axis里名为MoveForward的键

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);		// 将改变偏航角（物体坐标的偏航角）的函数绑定到Turn的键。该函数是基类Pawn中自带的虚函数。
}

