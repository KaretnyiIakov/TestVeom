// Fill out your copyright notice in the Description page of Project Settings.


#include "Loader.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
ALoader::ALoader()
{

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// MovementComponent
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALoader::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALoader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALoader::GetTask(FTask Task)
{
	_task = Task;
	_isBusy = true;
	OnBeBusy.Broadcast(this);
	TaskInProgress();

	if (_task._firstStorage != nullptr)
	{
		_task._firstStorage->OnBeTaskAim(true, *this->GetName().RightChop(3), true, _task._resourceCount);
	}
	if (_task._secondStorage != nullptr)
	{
		_task._secondStorage->OnBeTaskAim(true, *this->GetName().RightChop(3), false, _task._resourceCount);
	}
}

void ALoader::EndTask()
{
	_isBusy = false;
	UFunctionLib::ClearTask(_task);
	OnBeFree.Broadcast(this);
}


