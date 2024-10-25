﻿# AutoCreateRefTerm

- **功能描述：** 指定函数的多个输入引用参数在没有连接的时候自动为其创建默认值
- **使用位置：** UFUNCTION
- **引擎模块：** Blueprint
- **元数据类型：** strings="a，b，c"
- **常用程度：** ★★★★★

指定函数的多个输入引用参数在没有连接的时候自动为其创建默认值。

要注意“输入”+“引用”，这两个前提项。

当有些情况你想把函数的参数采用引用来传递，但是又不想每次都得必须连接一个变量，想在不连接的时候提供一个内联编辑的默认值，因此蓝图系统就提供了这么一个便利功能。

## 测试代码：

```cpp
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Location,Value"))
	static bool MyFunc_HasAutoCreateRefTerm(const FVector& Location, const int32& Value) { return false; }

	UFUNCTION(BlueprintCallable)
	static bool MyFunc_NoAutoCreateRefTerm(const FVector& Location, const int32& Value) { return false; }

	UFUNCTION(BlueprintCallable)
	static bool MyFunc_NoRef(FVector Location, int32 Value) { return false; }
```

## 蓝图效果：

可以见到MyFunc_NoAutoCreateRefTerm的函数会产生编译的报错，因为是引用参数但是却没有连接，导致引用缺少实参。

![Untitled](Untitled.png)

## 原理代码：

```cpp
void UEdGraphSchema_K2::GetAutoEmitTermParameters(const UFunction* Function, TArray<FString>& AutoEmitParameterNames)
{
	AutoEmitParameterNames.Reset();

	const FString& MetaData = Function->GetMetaData(FBlueprintMetadata::MD_AutoCreateRefTerm);
	if (!MetaData.IsEmpty())
	{
		MetaData.ParseIntoArray(AutoEmitParameterNames, TEXT(","), true);

		for (int32 NameIndex = 0; NameIndex < AutoEmitParameterNames.Num();)
		{
			FString& ParameterName = AutoEmitParameterNames[NameIndex];
			ParameterName.TrimStartAndEndInline();
			if (ParameterName.IsEmpty())
			{
				AutoEmitParameterNames.RemoveAtSwap(NameIndex);
			}
			else
			{
				++NameIndex;
			}
		}
	}

	// Allow any params that are blueprint defined to be autocreated:
	if (!FBlueprintEditorUtils::IsNativeSignature(Function))
	{
		for (	TFieldIterator<FProperty> ParamIter(Function, EFieldIterationFlags::Default);
				ParamIter && (ParamIter->PropertyFlags & CPF_Parm);
				++ParamIter)
		{
			FProperty* Param = *ParamIter;
			if(Param->HasAnyPropertyFlags(CPF_ReferenceParm))
			{
				AutoEmitParameterNames.Add(Param->GetName());
			}
		}
	}
}

还有在
void UK2Node_CallFunction::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
if ( Function )
{
	TArray<FString> AutoCreateRefTermPinNames;
	CompilerContext.GetSchema()->GetAutoEmitTermParameters(Function, AutoCreateRefTermPinNames);
	const bool bHasAutoCreateRefTerms = AutoCreateRefTermPinNames.Num() != 0;

	for (UEdGraphPin* Pin : Pins)
	{
		const bool bIsRefInputParam = Pin && Pin->PinType.bIsReference && (Pin->Direction == EGPD_Input) && !CompilerContext.GetSchema()->IsMetaPin(*Pin);
		if (!bIsRefInputParam)
		{
			continue;
		}

		const bool bHasConnections = Pin->LinkedTo.Num() > 0;
		const bool bCreateDefaultValRefTerm = bHasAutoCreateRefTerms &&
			!bHasConnections && AutoCreateRefTermPinNames.Contains(Pin->PinName.ToString());

		if (bCreateDefaultValRefTerm)
		{
			const bool bHasDefaultValue = !Pin->DefaultValue.IsEmpty() || Pin->DefaultObject || !Pin->DefaultTextValue.IsEmpty();

			// copy defaults as default values can be reset when the pin is connected
			const FString DefaultValue = Pin->DefaultValue;
			UObject* DefaultObject = Pin->DefaultObject;
			const FText DefaultTextValue = Pin->DefaultTextValue;
			bool bMatchesDefaults = Pin->DoesDefaultValueMatchAutogenerated();

			UEdGraphPin* ValuePin = InnerHandleAutoCreateRef(this, Pin, CompilerContext, SourceGraph, bHasDefaultValue);
			if ( ValuePin )
			{
				if (bMatchesDefaults)
				{
					// Use the latest code to set default value
					Schema->SetPinAutogeneratedDefaultValueBasedOnType(ValuePin);
				}
				else
				{
					ValuePin->DefaultValue = DefaultValue;
					ValuePin->DefaultObject = DefaultObject;
					ValuePin->DefaultTextValue = DefaultTextValue;
				}
			}
		}
		// since EX_Self does not produce an addressable (referenceable) FProperty, we need to shim
		// in a "auto-ref" term in its place (this emulates how UHT generates a local value for
		// native functions; hence the IsNative() check)
		else if (bHasConnections && Pin->LinkedTo[0]->PinType.PinSubCategory == UEdGraphSchema_K2::PSC_Self && Pin->PinType.bIsConst && !Function->IsNative())
		{
			InnerHandleAutoCreateRef(this, Pin, CompilerContext, SourceGraph, /*bForceAssignment =*/true);
		}
	}
}
}
```