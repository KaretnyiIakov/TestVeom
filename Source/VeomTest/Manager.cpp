// Fill out your copyright notice in the Description page of Project Settings.

#include "Manager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AManager::AManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AManager::BeginPlay()
{
	Super::BeginPlay();
	SetEmptyLimitsMap();
	SetEmptyResourcesMap();
	SetEmptyStoragesMap();
}

// Called every frame
void AManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AManager::CreateTask(AStorage* FirstStorage, AStorage* SecondStorage, int32 ResourceType, int32 ResourceCount)
{
	FTask NewTask;
	NewTask._firstStorage = FirstStorage;
	NewTask._secondStorage = SecondStorage;
	NewTask._resourceType = ResourceType;
	NewTask._resourceCount = ResourceCount;

	NewTask._firstStorage->TaskCounterUpdate(true);
	NewTask._secondStorage->TaskCounterUpdate(true);
	
	if (_freeLoaders.IsEmpty())
		_tasks.Add(NewTask);
	else
		GiveTaskForNearestLoader(NewTask);
	
	FirstStorage->_reservedResourceCount = FirstStorage->_reservedResourceCount + -1 * ResourceCount;
	SecondStorage->_reservedResourceCount = SecondStorage->_reservedResourceCount + ResourceCount;
}

void AManager::CreateTasksOfSelectedResource(int32 ResourceType, TMap<AStorage*, int32> StoragesHaveMoreThenNeed,
	TMap<AStorage*, int32> StoragesHaveLessThenNeed)
{
	
	TArray<AStorage*> StoragesWithMoreResources;
	StoragesHaveMoreThenNeed.GetKeys(StoragesWithMoreResources);
	TArray<AStorage*> StoragesWithLessResources;
	StoragesHaveLessThenNeed.GetKeys(StoragesWithLessResources);
	TArray<int32> MoreResourcesCount;
	StoragesHaveMoreThenNeed.GenerateValueArray(MoreResourcesCount);
	TArray<int32> LessResourcesCount;
	StoragesHaveLessThenNeed.GenerateValueArray(LessResourcesCount);
	int32 ResourceMoreCounter = 0;
	int32 ResourceLessCounter = 0;
	AStorage* CurrentMoreStorage = nullptr;
	int32 CurrentMoreResourceCount = 0;
	AStorage* CurrentLessStorage = nullptr;
	int32 CurrentLessResourceCount = 0;

	TArray<AStorage*> StoragesForDistributionOfRemainder = _resource_Storage.Find(ResourceType)->_storages;;
	int32 ResourceCounter = 0;

	while (StoragesHaveMoreThenNeed.Num()-1 >= ResourceMoreCounter && StoragesHaveLessThenNeed.Num()-1 >= ResourceLessCounter)
	{
		CurrentMoreStorage = StoragesWithMoreResources[ResourceMoreCounter];
		CurrentLessStorage = StoragesWithLessResources[ResourceLessCounter];
		CurrentMoreResourceCount = MoreResourcesCount[ResourceMoreCounter];
		CurrentLessResourceCount = LessResourcesCount[ResourceLessCounter];

		if (CurrentMoreResourceCount == CurrentLessResourceCount)
		{
			CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentMoreResourceCount);
			ResourceLessCounter++;
			ResourceMoreCounter++;
		}
		else if (CurrentMoreResourceCount > CurrentLessResourceCount)
		{
			CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentLessResourceCount);
			MoreResourcesCount[ResourceMoreCounter] = CurrentMoreResourceCount-CurrentLessResourceCount;
			ResourceLessCounter++;
		}
		else if (CurrentMoreResourceCount < CurrentLessResourceCount)
		{
			CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentMoreResourceCount);
			LessResourcesCount[ResourceLessCounter] = CurrentLessResourceCount-CurrentMoreResourceCount;
			ResourceMoreCounter++;
		}
		
	}

	if (StoragesHaveMoreThenNeed.Num()-1 >= ResourceMoreCounter)
	{
		while (StoragesHaveMoreThenNeed.Num()-1 >= ResourceMoreCounter)
		{
			//StoragesForDistributionOfRemainder.Remove(StoragesWithMoreResources[ResourceMoreCounter]);
			CurrentMoreStorage = StoragesWithMoreResources[ResourceMoreCounter];
			ResourceCounter = MoreResourcesCount[ResourceMoreCounter];
			
			float ResourceCountForOneStorage_WholePart = UE4::SSE::TruncToInt32((float)MoreResourcesCount[ResourceMoreCounter] / (float)StoragesForDistributionOfRemainder.Num()); //0.0f;
			float ResourceCountForOneStorage_FractionalPart = FMath::Fmod ((float)MoreResourcesCount[ResourceMoreCounter], (float)StoragesForDistributionOfRemainder.Num());// fmod((float)MoreResourcesCount[ResourceMoreCounter]/ (float)StoragesForDistributionOfRemainder.Num(),ResourceCountForOneStorage_WholePart);
			for (AStorage* Storage : StoragesForDistributionOfRemainder)
			{
				if (Storage != CurrentMoreStorage)
				{
					CurrentLessStorage = Storage;
					CurrentLessResourceCount = ((ResourceCountForOneStorage_FractionalPart  * StoragesForDistributionOfRemainder.Num() + 1) > StoragesForDistributionOfRemainder.Find(Storage)) ? (ResourceCountForOneStorage_WholePart + 1) : ResourceCountForOneStorage_WholePart;
					if(CurrentLessResourceCount > 0)
					{
						if ( FMath::Max(ResourceCounter - CurrentLessResourceCount, 0) > 0 )//? (ResourceCounter - CurrentLessResourceCount) : 0) > 0
						{
							ResourceCounter = FMath::Max(ResourceCounter - CurrentLessResourceCount, 0);
							//ResourceCounter = ResourceCounter - CurrentLessResourceCount;
						}
						else
						{
							if (ResourceCounter > 0)
							{
								CurrentLessResourceCount = ResourceCounter;
								ResourceCounter = 0;
							}
							else
								break;
						}
						CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentLessResourceCount);
					}
					else
						break;
				}
			};
			ResourceMoreCounter++;
		}
	}

	if (StoragesHaveLessThenNeed.Num()-1 >= ResourceLessCounter)
	{
		while (StoragesHaveLessThenNeed.Num()-1 >= ResourceLessCounter)
		{
			CurrentLessStorage = StoragesWithLessResources[ResourceLessCounter];
			ResourceCounter = LessResourcesCount[ResourceLessCounter];
			
			float ResourceCountForOneStorage_WholePath = UE4::SSE::TruncToInt32((float)LessResourcesCount[ResourceLessCounter] / (float)StoragesForDistributionOfRemainder.Num());
			float ResourceCountForOneStorage_FractionalPath = FMath::Fmod((float)LessResourcesCount[ResourceLessCounter], (float)StoragesForDistributionOfRemainder.Num());// fmod((float)LessResourcesCount[ResourceLessCounter]/(float)StoragesHaveLessThenNeed.Num(),ResourceCountForOneStorage_WholePath);
			//StoragesForDistributionOfRemainder.Remove(StoragesWithMoreResources[ResourceMoreCounter]);
			for (AStorage* Storage : StoragesForDistributionOfRemainder)
			{
				if (Storage != CurrentLessStorage)
				{
					CurrentMoreStorage = Storage;
					CurrentMoreResourceCount = ((ResourceCountForOneStorage_FractionalPath * StoragesForDistributionOfRemainder.Num() + 1) > StoragesForDistributionOfRemainder.Find(Storage) ) ? (ResourceCountForOneStorage_WholePath + 1) : ResourceCountForOneStorage_WholePath;
					if(CurrentMoreResourceCount > 0)
					{
						if ( FMath::Max(ResourceCounter - CurrentMoreResourceCount, 0) > 0)
						{
							ResourceCounter = FMath::Max(ResourceCounter - CurrentMoreResourceCount, 0);
						}
						else
						{
							if (ResourceCounter > 0)
							{
								CurrentMoreResourceCount = ResourceCounter;
								ResourceCounter = 0;
							}
							else
								break;
						}
						CreateTask(CurrentMoreStorage, CurrentLessStorage, ResourceType, CurrentMoreResourceCount);
					}
					else
						break;
				}
			};
			ResourceLessCounter++;
		}
	}
}

void AManager::GiveTaskForNearestLoader(FTask Task)
{
	if (Task._firstStorage)
	{
		float Distance = 0.0f;
		FindNearestLoader(Task._firstStorage->GetActorLocation(),_freeLoaders,Distance)->GetTask(Task);
	}
	
}
ALoader* AManager::FindNearestLoader(FVector Origin, const TArray<ALoader*>& LoadersToCheck, float& Distance)
{
	ALoader* NearestActor = nullptr;
	float DistanceFromNearestActor = Distance = TNumericLimits<float>::Max();

	for (ALoader* LoaderToCheck : LoadersToCheck)
	{
		if (LoaderToCheck)
		{
			const float DistanceFromActorToCheck = (Origin - LoaderToCheck->GetActorLocation()).SizeSquared();
			if (DistanceFromActorToCheck < DistanceFromNearestActor)
			{
				NearestActor = LoaderToCheck;
				DistanceFromNearestActor = DistanceFromActorToCheck;
			}
		}
	}

	if (NearestActor)
	{
		Distance = FMath::Sqrt(DistanceFromNearestActor);
	}

	return NearestActor;
}

void AManager::RegisterLoader(ALoader* Loader)
{
	//FString NewName = FString::Printf(TEXT("Actor_%d"), _loaderCounter);
	
	Loader->OnBeBusy.AddDynamic(this,&AManager::OnLoaderBeBusy);
	Loader->OnBeFree.AddDynamic(this,&AManager::OnLoaderBeFree);
	OnLoadersSpeedChange.AddDynamic(Loader,&ALoader::OnLoaderSpeedChange);
	_freeLoaders.AddUnique(Loader);
}

void AManager::OnLoaderBeBusy(ALoader* Loader)
{
	_freeLoaders.Remove(Loader);
}

void AManager::OnLoaderBeFree(ALoader* Loader)
{
	if(_tasks.IsEmpty())
	{
		_freeLoaders.AddUnique(Loader);
	}
	else
	{
		Loader->GetTask(_tasks[0]);
		_tasks.RemoveAt(0);
		
	}
}

void AManager::ChangeLoadersSpeed(ESpeed Speed)
{
	OnLoadersSpeedChange.Broadcast(Speed);
}

void AManager::RegisterStorage(AStorage* Storage)
{
	
}

void AManager::UpdateMapsForAllStorages()
{
	TArray<AActor*> Storages;
	UGameplayStatics::GetAllActorsOfClass(this
		, AStorage::StaticClass(), Storages);
	for (AActor* Storage : Storages)
	{
		AStorage* StorageActor = Cast<AStorage>(Storage);
		if (StorageActor->_inSystem)
		{
			UpdateMapsForStorage(StorageActor,true);
		}
	}
}

void AManager::UpdateMapsForStorage(AStorage* CurrentStorage, bool Add)
{
	UpdateStorageMap(CurrentStorage, Add);
	UpdateResourceCountMap(CurrentStorage, Add);
	UpdateResourceLimitMap(CurrentStorage, Add);
}

void AManager::UpdateStorageMap(AStorage* CurrentStorage, bool Add)
{
	if (Add)
		_resource_Storage[CurrentStorage->_resourceType]._storages.AddUnique(CurrentStorage);
	else
		_resource_Storage[CurrentStorage->_resourceType]._storages.Remove(CurrentStorage);
}

void AManager::UpdateResourceCountMap(AStorage* CurrentStorage, bool Add)
{
	int32 Multi = Add ? 1 : -1;
	int32 Count = _resource_Count[CurrentStorage->_resourceType] + (CurrentStorage->_resourceCount * Multi) + CurrentStorage->_reservedResourceCount;
	_resource_Count.Add(CurrentStorage->_resourceType, Count);
}

void AManager::UpdateResourceLimitMap(AStorage* CurrentStorage, bool Add)
{
	int32 Multi = Add ? 1 : -1;
	int32 Count = _resource_Limit[CurrentStorage->_resourceType] + (CurrentStorage->GetLimit() * Multi);
	_resource_Limit.Add(CurrentStorage->_resourceType, Count);
}

void AManager::SetStorageSystemStatus(AStorage* CurrentStorage, bool Add)
{
	if (Add)
	{
		_outSystemStorages.Remove(CurrentStorage);
		UpdateMapsForStorage(CurrentStorage, Add);
		ManageSelectedResource(CurrentStorage->_resourceType,nullptr);
	}
	else
	{
		_outSystemStorages.AddUnique(CurrentStorage);
		ELimit BufferLimit = CurrentStorage->_resourceLimit;
		UpdateResourceLimitMap(CurrentStorage,Add);
		CurrentStorage->_resourceLimit = ELimit::Zero;
		ManageSelectedResource(CurrentStorage->_resourceType,CurrentStorage);
		UpdateStorageMap(CurrentStorage,Add);
		CurrentStorage->_resourceLimit = BufferLimit;
	}
	
}

void AManager::AddOrRemoveResourcesFromStorage(AStorage* CurrentStorage, bool Add, int32 ResourceCount)
{
	if (CurrentStorage->_inSystem)
	{
		UpdateResourceCountMap(CurrentStorage,false);
		CurrentStorage->_resourceCount += ResourceCount * (Add ? 1 : -1);
		UpdateResourceCountMap(CurrentStorage,true);
		ManageSelectedResource(CurrentStorage->_resourceType,nullptr);
	}
	else
		CurrentStorage->_resourceCount += ResourceCount * (Add ? 1 : -1);
	
}

void AManager::ChangeStorageResourceType(AStorage* CurrentStorage, int32 NewType)
{
	CurrentStorage->_resourceType = NewType;
}

float AManager::FindAveragePercentForResource(int32 ResourceType)
{
	return (float)_resource_Count[ResourceType]/(float)_resource_Limit[ResourceType];
}

void AManager::SetEmptyResourcesMap()
{
	for(int i = 1; i <= 50; i++)
	{
		_resource_Count.Add(i,0);
	}
}

void AManager::SetEmptyStoragesMap()
{
	FStorages Empty;
	for(int i = 1; i <= 50; i++)
	{
		_resource_Storage.Add(i,Empty);
	}
}

void AManager::SetEmptyLimitsMap()
{
	for(int i = 1; i <= 50; i++)
	{
		_resource_Limit.Add(i,0);
	}
}

void AManager::ManageSelectedResource(const int32 ResourceType, AStorage* IgnoreStorage)
{
	float const Percent =  FindAveragePercentForResource(ResourceType);
	TMap<AStorage*, int32> StoragesHaveMoreThenNeed;
	TMap<AStorage*, int32> StoragesHaveLessThenNeed;
	
	for (AStorage* Storage : _resource_Storage[ResourceType]._storages)
	{
		if (!FMath::IsNearlyEqual((float)(Storage->_resourceCount + Storage->_reservedResourceCount),Percent * (float)Storage->GetLimit(), 1.0f))
		{
			if ((float)(Storage->_resourceCount + Storage->_reservedResourceCount) > Percent * (float)Storage->GetLimit())
			{
				StoragesHaveMoreThenNeed.Add(Storage, (int32)abs((float)(Storage->_resourceCount + Storage->_reservedResourceCount)-(Percent * (float)Storage->GetLimit())));
			}
			else
			{
				if(!(IgnoreStorage!=nullptr && Storage==IgnoreStorage))
				{
					StoragesHaveLessThenNeed.Add(Storage, (int32)abs((float)(Storage->_resourceCount + Storage->_reservedResourceCount)-(Percent * (float)Storage->GetLimit())));
				}
			}
				
		}
	}
	CreateTasksOfSelectedResource(ResourceType, StoragesHaveMoreThenNeed, StoragesHaveLessThenNeed);
	
}

