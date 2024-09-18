// Fill out your copyright notice in the Description page of Project Settings.


#include "Storage.h"

// Sets default values
AStorage::AStorage()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStorage::BeginPlay()
{
	Super::BeginPlay();
	
}

int AStorage::GetLimit() const
{
	switch (_resourceLimit)
	{
		case ELimit::Zero: return 0;  break;
		case ELimit::Fifty: return 50; break;
		case ELimit::OneHundred: return 100; break;
		case ELimit::TwoHundredFifty: return 250; break;
		case ELimit::FiveHundred: return 500; break;
	}
	return 0;
}

void AStorage::SetRandLimit()
{
	int32 RandomValue = UKismetMathLibrary::RandomIntegerInRange(1, static_cast<int32>(ELimit::Enum_MAX)-1);
	_resourceLimit = static_cast<ELimit>(RandomValue);
}

void AStorage::SetRandResourceType()
{
	_resourceType = UKismetMathLibrary::RandomIntegerInRange(1, 50);
}

void AStorage::SetRandResourceCount()
{
	switch (_resourceLimit)
	{
		case ELimit::Zero: _resourceCount = 0;  break;
		case ELimit::Fifty: _resourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 50); break;
		case ELimit::OneHundred: _resourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 100);; break;
		case ELimit::TwoHundredFifty: _resourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 250);; break;
		case ELimit::FiveHundred: _resourceCount =  UKismetMathLibrary::RandomIntegerInRange(0, 500);; break;
	}
	
}

void AStorage::GiveResourceToLoader(int32 Count)
{
	_resourceCount = _resourceCount - Count;
	_reservedResourceCount = _reservedResourceCount + Count;
}

void AStorage::GetResourceFromLoader(int32 Count)
{
	_resourceCount = _resourceCount + Count;
	_reservedResourceCount = _reservedResourceCount - Count;
}

void AStorage::OnBeTaskAim(bool Add, FString LoaderName, bool WantToTake, int32 TaskResourceCount)
{
	_taskCounter = (Add ? 1 : -1) + _taskCounter;
	
	if (Add)
		(WantToTake ? _loadersWhoWantToTake : _loadersWhoWantToPut ).Add(LoaderName, TaskResourceCount);
	else
		(WantToTake ? _loadersWhoWantToTake : _loadersWhoWantToPut ).Remove(LoaderName);

	OnLoaderInteraction.Broadcast(Add,LoaderName);
}

void AStorage::RegisterInManager()
{

}

// Called every frame
void AStorage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

