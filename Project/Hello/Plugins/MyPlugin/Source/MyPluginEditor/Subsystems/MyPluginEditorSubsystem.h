// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"

#include "EditorSubsystem.h"
#include "MyPluginEditorSubsystem.generated.h"

UCLASS()
class MYPLUGINEDITOR_API UMyPluginEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()
public:
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return true; }
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;

public:
	UFUNCTION(BlueprintCallable)
	void AddScore(float delta);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Score;

};
