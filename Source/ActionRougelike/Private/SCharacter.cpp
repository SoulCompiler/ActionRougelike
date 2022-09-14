// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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

	FiringRange = 5000.0f;

	HandSocketName = "Muzzle_01";
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
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
	PlayerInputComponent->BindAction("SuperAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);
	AttackAnimDelay = 0.2f;
}

void ASCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
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
	if (ensure(AttackVFX))
	{
		UGameplayStatics::SpawnEmitterAttached(AttackVFX, GetMesh(), HandSocketName, FVector::ZeroVector,
		                                       FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
	}

	// 将需要在播放动画延时中执行的函数放在Timer中。
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed,
	                                AttackAnimDelay);


	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	SpawnProjectile(ProjectileClass);
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

void ASCharacter::BlackHoleAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_BlackHoleAttack, this, &ASCharacter::BlackHoleAttack_TimeElapsed,
	                                AttackAnimDelay);
}

void ASCharacter::BlackHoleAttack_TimeElapsed()
{
	SpawnProjectile(BlackHoleProjectileClass);
}

void ASCharacter::Dash()
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::Dash_TimeElaped, AttackAnimDelay);
}

void ASCharacter::Dash_TimeElaped()
{
	SpawnProjectile(DashProjectileClass);
}

// 动画延时后发射子弹
void ASCharacter::SpawnProjectile(TSubclassOf<AActor> ClassToSpawn)
{
	if (ensureAlways(ClassToSpawn))
	{
		FVector HandLocation = GetMesh()->GetSocketLocation(HandSocketName);

		FVector TraceStart = CameraComp->GetComponentLocation();
		FVector TraceEnd = TraceStart + (CameraComp->GetComponentRotation().Vector() * FiringRange);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		FHitResult Hit;

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
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnTM, SpawnParams);
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
                                  float Delta)
{
	if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}

	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
}
