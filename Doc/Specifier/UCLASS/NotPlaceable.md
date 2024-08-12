# NotPlaceable

Type: bool
Feature: Behavior
Description: 使继承自基类的Placeable说明符无效。指示不可以在编辑器中将此类的对象放置到关卡、UI场景或蓝图中。
EClassFlagsOperation: |=
EClassFlags: CLASS_NotPlaceable (../../Flags/EClassFlags/CLASS_NotPlaceable.md)
Status: Done

标明该类不可被放置在关卡里，没法拖放到场景里。会在ClassFlags里标记上CLASS_NotPlaceable，这个标记是可以继承的，意味着其所有的子类默认都不可放置。例如AWorldSettings其实就是一个notplaceable的Actor。

但是注意该类依然可以通过SpawnActor动态生成到关卡中。

NotPlaceable的类是不出现在PlaceMode的类选择里去的。

```cpp
UCLASS(Blueprintable,BlueprintType, NotPlaceable)
class INSIDER_API AMyActor_NotPlaceable :public AActor
{
	GENERATED_BODY()
};
```

如果直接是C++类AMyActor_NotPlaceable ，是可以直接从ContentBrowser拖到场景里去的。看源码可知，只有BP继承下来的子类才有受到这个限制。

```cpp
TArray<AActor*> FLevelEditorViewportClient::TryPlacingActorFromObject( ULevel* InLevel, UObject* ObjToUse, bool bSelectActors, EObjectFlags ObjectFlags, UActorFactory* FactoryToUse, const FName Name, const FViewportCursorLocation* Cursor )
{

	bool bPlace = true;
	if (ObjectClass->IsChildOf(UBlueprint::StaticClass()))
	{
		UBlueprint* BlueprintObj = StaticCast<UBlueprint*>(ObjToUse);
		bPlace = BlueprintObj->GeneratedClass != NULL;
		if(bPlace)
		{
			check(BlueprintObj->ParentClass == BlueprintObj->GeneratedClass->GetSuperClass());
			if (BlueprintObj->GeneratedClass->HasAnyClassFlags(CLASS_NotPlaceable | CLASS_Abstract))
			{
				bPlace = false;
			}
		}
	}

	if (bPlace)
	{
		PlacedActor = FActorFactoryAssetProxy::AddActorForAsset( ObjToUse, bSelectActors, ObjectFlags, FactoryToUse, Name );
		if ( PlacedActor != NULL )
		{
			PlacedActors.Add(PlacedActor);
			PlacedActor->PostEditMove(true);
		}
	}
}
```

![Untitled](NotPlaceable/Untitled.png)