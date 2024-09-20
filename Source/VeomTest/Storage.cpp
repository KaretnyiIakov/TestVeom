// Fill out your copyright notice in the Description page of Project Settings.


#include "Storage.h"
#include "Kismet/KismetMathLibrary.h"


int32 AStorage::_storageCounter = 0;

// Sets default values
AStorage::AStorage()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
 	
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStorage::BeginPlay()
{
	Super::BeginPlay();
	RenameStorage();
}

int AStorage::GetLimit() const
{
	switch (ResourceLimit)
	{
		case ELimit::Zero: return 0; 
		case ELimit::Fifty: return 50;
		case ELimit::OneHundred: return 100;
		case ELimit::TwoHundredFifty: return 250;
		case ELimit::FiveHundred: return 500;
		default: return 0;
	}
}

void AStorage::SetRandLimit()
{
	int32 RandomValue = UKismetMathLibrary::RandomIntegerInRange(1, static_cast<int32>(ELimit::Enum_MAX)-1);
	ResourceLimit = static_cast<ELimit>(RandomValue);
}

void AStorage::SetRandResourceType()
{
	ResourceType = UKismetMathLibrary::RandomIntegerInRange(1, 50);
}

void AStorage::SetRandResourceCount()
{
	switch (ResourceLimit)
	{
		case ELimit::Zero: ResourceCount = 0;  break;
		case ELimit::Fifty: ResourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 50); break;
		case ELimit::OneHundred: ResourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 100);; break;
		case ELimit::TwoHundredFifty: ResourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 250);; break;
		case ELimit::FiveHundred: ResourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 500);; break;
		default: ResourceCount = 0; break;
	}
}

void AStorage::GiveResourceToLoader(int32 Count)
{
	ResourceCount = ResourceCount - Count;
	ReservedResourceCount = ReservedResourceCount + Count;
}

void AStorage::GetResourceFromLoader(int32 Count)
{
	ResourceCount = ResourceCount + Count;
	ReservedResourceCount = ReservedResourceCount - Count;
}

void AStorage::OnBeTaskAim(bool Add, FString LoaderName, bool WantToTake, int32 TaskResourceCount)
{
	if (Add)
	{
		(WantToTake ? WantToTakeLoaders : WantToPutLoaders ).Add(LoaderName, TaskResourceCount);
	}
	else
	{
		TaskCounterUpdate(false);
		(WantToTake ? WantToTakeLoaders : WantToPutLoaders ).Remove(LoaderName);
	}
	OnLoaderInteraction.Broadcast(Add,LoaderName);
}

void AStorage::TaskCounterUpdate(bool Up)
{
	TaskCounter += (Up ? 1 : -1);
}

void AStorage::RenameStorage()
{
	IncrementCounter();
	this->Rename(*FString::Printf(TEXT("Storage_%d"), _storageCounter));
}

void AStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStorage::IncrementCounter()
{
	_storageCounter++;
	if (_storageCounter > 2000)
	{
		_storageCounter = 0;
	}
}

