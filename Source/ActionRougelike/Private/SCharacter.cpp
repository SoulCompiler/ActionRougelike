// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true; // 弹簧臂使用输入给Pawn的旋转控制。
	SpringArmComp->SetupAttachment(RootComponent); // 将组件放到根部组件下（派生自actor的默认根部组件是CapsuleComponent）

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp); // 将组件放到弹簧臂的层级下

	GetCharacterMovement()->bOrientRotationToMovement = true; // 当角色在某个方向上移动时，角色朝向旋转到那个方向。

	bUseControllerRotationYaw = false; // 禁用绕对象坐标Yaw轴的旋转（UE默认禁止Pitch和Roll轴的旋转），确保视角移动时角色的朝向不受影响。
	// 这项是视游戏需求而定的，一些游戏（ARPG类）就需要视角移动时身体朝向随之改变。

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	
	FiringRange = 1000.0f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
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

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	// 将MoveForward函数绑定到项目设置中Input下Axis里名为MoveForward的键
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	// 将改变Controller偏航角（物体坐标的偏航角）的函数绑定到Turn的键。该函数是基类Pawn中自带的虚函数。
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
	PlayerInputComponent->BindAction("SuperAttack",IE_Pressed,this,&ASCharacter::SuperAttack);
	PlayerInputComponent->BindAction("Dash",IE_Pressed,this,&ASCharacter::Dash);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation(); // 将Controller的Rotation赋值给Rotator。
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	AddMovementInput(ControlRot.Vector(), Value); // 该函数是基类Pawn的运动组件中自带的虚函数，根据输入的vector和value在向量方向进行运动。
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation(); // 将Controller的Rotation赋值给Rotator。
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	// 不知道RightVector怎么获取时，去蓝图中搜索，然后根据蓝图显示的信息再在VS中使用VA的Find Symbol功能搜索该变量相关的名字RightVector，找到和蓝图信息相符的函数，就可以获取正确的变量。

	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	PlayAnimMontage(AttackAnim); // 播放蒙太奇动画

	// 将需要在播放动画延时中执行的函数放在Timer中。
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f);


	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}


FRotator ASCharacter::GetImpactRotation(const FVector& SpawnLocation)
{
	FVector CameraLocation = CameraComp->GetComponentLocation();
	FVector TraceEnd = CameraLocation + (CameraComp->GetComponentRotation().Vector() * FiringRange);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->LineTraceMultiByObjectType(Hits, CameraLocation, TraceEnd, ObjectQueryParams);

	FRotator ImpactRotation;
	if (bBlockingHit)
	{
		FVector ImpactLocation = Hits[0].ImpactPoint;
		ImpactRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, ImpactLocation);
		// DrawDebugLine(GetWorld(), SpawnLocation, ImpactLocation, FColor::Blue, false, 2.0f, 0, 2.0f);
	}
	else
	{
		ImpactRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TraceEnd);
		// DrawDebugLine(GetWorld(), SpawnLocation, TraceEnd, FColor::Red, false, 2.0f, 0, 2.0f);
	}
	
	return ImpactRotation;
}

// 动画延时后发射子弹
void ASCharacter::PrimaryAttack_TimeElapsed()
{
	if (ensure(ProjectileClass))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01"); // 获得手掌骨骼的坐标
        
        	FRotator TargetRotator = GetImpactRotation(HandLocation);
        
        	FTransform SpawnTM = FTransform(TargetRotator, HandLocation); // 第二个参数是投射物的出生点的位置
        
        	FActorSpawnParameters SpawnParams;
        	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        	// 当Spawn出生点产生碰撞时总是SpawnActor
        	SpawnParams.Instigator = this; // 让发射物Actor携带发射者Instigator的信息。
        
        	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
	
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::SuperAttack()
{
	PlayAnimMontage(AttackAnim);

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	FRotator TargetRotator = GetImpactRotation(HandLocation);
	FTransform SpawnTM(TargetRotator,HandLocation);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	GetWorld()->SpawnActor<AActor>(BlackholeClass,SpawnTM,SpawnParams);
}

void ASCharacter::Dash()
{
	PlayAnimMontage(AttackAnim);

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	FRotator TargetRotator = GetImpactRotation(HandLocation);
	FTransform SpawnTM(TargetRotator,HandLocation);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	GetWorld()->SpawnActor<AActor>(DashProjectileClass,SpawnTM,SpawnParams);
}