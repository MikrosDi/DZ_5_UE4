// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Cannon.h"
#include "DamageTarget.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	RootComponent = Collision;

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);

	AudioHitEffect = CreateDefaultSubobject<UAudioComponent>("AudioEffect");
	AudioHitEffect->SetupAttachment(Mesh);

	HitEffect = CreateDefaultSubobject<UParticleSystemComponent>("ShootEffect");
	HitEffect->SetupAttachment(Mesh);

	
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(Timer, this,&AProjectile::MoveTick, MoveRate, true);
}


void AProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)

{
	if (OtherActor == this)
		return;
	
	GetWorld()->GetTimerManager().ClearTimer(Timer);
	auto Target = Cast<IDamageTarget>(OtherActor);
	if(Target)
	{
		FDamageData DamageData;
		DamageData.DamageValue = Damage;
		DamageData.Instigator = GetInstigator();
		Target->TakeDamage(DamageData);
		
	}
	//ShootEffect->ActivateSystem();
	//AudioEffect->Play();
	Destroy();
	
		HitEffect->ActivateSystem();
		AudioHitEffect->Play();
	
	
	//OtherActor->Destroy(); - уничтожает объект при попадании
}

void AProjectile::MoveTick()
{
	auto Location = GetActorLocation();
	SetActorLocation(Location + GetActorForwardVector() * MoveRate * MoveSpeed, true);
}



