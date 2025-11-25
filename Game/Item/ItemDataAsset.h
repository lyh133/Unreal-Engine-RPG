

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Runtime/Engine/Classes/Engine/DataAsset.h"
#include "dazAda/Game/Item/EItemType.h"
#include "ItemDataAsset.generated.h"


/**
 * 
 */
UCLASS()
class DAZADA_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer ItemTags;
	
};
