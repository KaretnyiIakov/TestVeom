// Fill out your copyright notice in the Description page of Project Settings.

#include "Loader.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

int32 ALoader::_loaderCounter = 0;

// Sets default values
ALoader::ALoader()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// MovementComponent
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALoader::BeginPlay()
{
	Super::BeginPlay();
	// Name at creation
	RenameLoader();
}

// Called every frame
void ALoader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Loader Tasks functions
void ALoader::StartTask(FTask const Task)
{
	// Assign a task and call delegates
	LoaderTask = Task;
	bIsBusy = true;
	OnBeBusy.Broadcast(this);
	TaskInProgress();
	
	// Warn storages that they should expect a loader to work and send work type
	if (LoaderTask._firstStorage != nullptr)
	{
		LoaderTask._firstStorage->OnBeTaskAim(true, this->GetName(), true, LoaderTask._resourceCount);
	}
	if (LoaderTask._secondStorage != nullptr)
	{
		LoaderTask._secondStorage->OnBeTaskAim(true, this->GetName(), false, LoaderTask._resourceCount);
	}
}

void ALoader::EndTask()
{
	bIsBusy = false;
	UFunctionLib::ClearTask(LoaderTask);
	bFirstStorageIsVisited = false;
	bSecondStorageIsVisited = false;
	OnBeFree.Broadcast(this);
}

// Functions used for naming loaders (only for cosmetic)
void ALoader::IncrementCounter()
{
	_loaderCounter++;
	if (_loaderCounter > 2000)
	{
		_loaderCounter = 0;
	}
}

void ALoader::RenameLoader()
{
	IncrementCounter();
	this->Rename(*FString::Printf(TEXT("Loader_%d"), _loaderCounter));
}


