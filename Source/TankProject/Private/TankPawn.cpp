// Fill out your copyright notice in the Description page of Project Settings.

#include "TankPawn.h"

#include "TankPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Cannon.h"
#include "TurretCannon.h"
#include "DefoltCannon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TankProject/TankProject.h"
#include "HealthComponent.h"




// Sets default values
ATankPawn::ATankPawn():AParentStarterClass()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ArmComponent = CreateDefaultSubobject<USpringArmComponent>("ArmComponent");
	ArmComponent->SetupAttachment(RootComponent);
	ArmComponent->SetRelativeRotation(FRotator(-90, 0, 0));
	
	CameraComponent =CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(ArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
	
	HealthComponent->OnHealthChanged.AddUObject(this, &ATankPawn::OnHealthChanged);
	HealthComponent->OnDeath.AddUObject(this, &ATankPawn::OnDeath);
}


void ATankPawn::MoveForward(float Scale)
{
	MoveScaleTarget = Scale;
}

void ATankPawn::RotateRight(float Scale)
{
	RotationScaleTarget = Scale;
}

void ATankPawn::Shoot()
{
	if(SwapWep ==0)
	
	{
		if(Cannon && AmmoBB > 0)
		{
			Cannon->Shoot(AmmoBB);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(333, 1.5,FColor::Red,FString(TEXT("NO BULLET")));
		}
	}
	else
	{
		FireStart();
	}
}


void ATankPawn::SetupCannon(const TSubclassOf<ACannon>& CannonClass)
{
	CannonType = CannonClass;
	if(CannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		Cannon = GetWorld()->SpawnActor<ACannon>(CannonType, Transform);
		Cannon->AttachToComponent(CannonPosition,FAttachmentTransformRules::SnapToTargetIncludingScale);
		Cannon->SetInstigator(this);
	}
}

void ATankPawn::TurretShoot()
{
	
		if( TurretCannon != nullptr && AmmoII > 0)
		{
			TurretCannon->TurretShoot(AmmoII);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(3331, 1.5,FColor::Red,FString(TEXT("NO BULLET")));
		}
	
}


void ATankPawn::TurretSetupCannon(const TSubclassOf<ATurretCannon>& CannonClass)
{
	TurretCannonType = CannonClass;
	if(TurretCannonType)
	{
		auto Transform = CannonPosition->GetComponentTransform();
		TurretCannon = GetWorld()->SpawnActor<ATurretCannon>(TurretCannonType, Transform);
		TurretCannon->AttachToComponent(CannonPosition,FAttachmentTransformRules::SnapToTargetIncludingScale);
		TurretCannon->SetInstigator(this);
	}
}

void ATankPawn::TakeDamage(FDamageData Damage)
{
	if(HealthComponent)
	HealthComponent->TakeDamage(Damage);
}


void ATankPawn::FireStart()
{
	if(DefoltCannon != nullptr)
	{
			DefoltCannon->FireStart();
	}
	
}

void ATankPawn::FireStop()
{

	if(DefoltCannon != nullptr)
	{
		DefoltCannon->FireStop();
	}
}

void ATankPawn::SetupDefoltCannon(const TSubclassOf<ADefoltCannon>& CannonClass)
{
	{
		DefoltCannonType = CannonClass;
		if (DefoltCannonType)
		{
			auto Transform = CannonPosition->GetComponentTransform();
			DefoltCannon = GetWorld()->SpawnActor<ADefoltCannon>(DefoltCannonType, Transform);
			DefoltCannon->AttachToComponent(CannonPosition, FAttachmentTransformRules::SnapToTargetIncludingScale);
			DefoltCannon->SetInstigator(this);
		}
	}
}

void ATankPawn::Swap()
{
	if(SwapWep == 0 )
	{
		SwapWep = 1;
	}
	else
	{
		SwapWep = 0;
	}
}

void ATankPawn::UploadAmmo(const int ShellCount)
{
	{
		DefoltCannon->UploadAmmo(ShellCount);
	}
	
	AmmoBB += ShellCount;
	GEngine->AddOnScreenDebugMessage(-1234, 1, FColor::Blue, FString::Printf(TEXT("AmmoBB %i"), AmmoBB));
	
}

void ATankPawn::OnPointsReceived(int InPoints)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red,
								 FString::Printf(TEXT("Score: %d"), Points));
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green,
	                                 FString::Printf(TEXT("%s + %d Points"),*PlayerName, InPoints));
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();

	TankController = Cast<ATankPlayerController>(GetController());

	SetupCannon(CannonType);
	SetupDefoltCannon(DefoltCannonType);

	
}

void ATankPawn::Destroyed()
{
	Super::Destroyed();

	if(Cannon)
	{
		Cannon->Destroy();
	}
	
	if(DefoltCannon)
	{
		DefoltCannon->Destroy();
	}

	if(TurretCannon)
	{
		TurretCannon->Destroy();
	}
	
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveTank(DeltaTime);
	RotateTank(DeltaTime);
	RotateCannon(DeltaTime);
}

	// Called to bind functionality to input
	void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
	{
		Super::SetupPlayerInputComponent(PlayerInputComponent);
	}

void ATankPawn::MoveTank(float DeltaTime)
{
	MoveScaleCurrent = FMath::Lerp(MoveScaleCurrent, MoveScaleTarget,MovementAcceleration);
	const auto Location = GetActorLocation();
	SetActorLocation(Location + GetActorForwardVector() * MoveScaleCurrent * MovementSpeed * DeltaTime, false);
	GEngine->AddOnScreenDebugMessage(-666, -5, FColor::Turquoise, FString::Printf(TEXT("Speed - %f"), MoveScaleCurrent));
}

void ATankPawn::RotateTank(float DeltaTime)
{
	RotationScaleCurrent = FMath::Lerp(RotationScaleCurrent, RotationScaleTarget, RotationAcceleration);
	auto Rotation = GetActorRotation();
	Rotation.Yaw += RotationScaleCurrent * RotationSpeed * DeltaTime;
	SetActorRotation(Rotation);
	UE_LOG(LogTankProject, Warning, TEXT("%s"), *Rotation.ToString())
}

void ATankPawn::RotateCannon(float DeltaTime)
{
	if(!TankController)
		return;
	auto BodyMeshRotation = BodyMesh->GetComponentRotation();
	auto OldRotation = TurretMesh->GetComponentRotation();
	FRotator TurretRotation = UKismetMathLibrary::FindLookAtRotation(TurretMesh->GetComponentLocation(),TankController->GetShootTarget());
	OldRotation.Yaw = TurretRotation.Yaw;
	OldRotation.Pitch = BodyMeshRotation.Pitch;
	OldRotation.Roll = BodyMeshRotation.Roll;
	TurretMesh->SetWorldRotation(FMath::Lerp(TurretMesh->GetComponentRotation(),OldRotation,RotationAcceleration));
}

void ATankPawn::OnHealthChanged(float CurrentHealth)
{
	
	GEngine->AddOnScreenDebugMessage(987665, 10, FColor::White,FString::Printf(TEXT("Health: %f"), CurrentHealth));
}

void ATankPawn::OnDeath()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),GetWorld()->GetFirstPlayerController(),EQuitPreference::Quit,true);
}

