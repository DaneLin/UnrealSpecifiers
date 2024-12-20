# ScriptNoExport

- **Function Description:** Prevents the export of this function or property to the script.
- **Usage Locations:** UFUNCTION, UPROPERTY
- **Engine Module:** Script
- **Metadata Type:** bool
- **Commonality:** ★★★

Do not export this function or property to the script.

## Test Code:

```cpp
UCLASS(Blueprintable, BlueprintType, meta = (ScriptName = "MyPythonLib"))
class INSIDER_API UMyPythonTestLibary :public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void MyScriptFunc_None();

	UFUNCTION(BlueprintCallable, meta = (ScriptNoExport))
	static void MyScriptFunc_NoExport();
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MyFloat = 123.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ScriptNoExport))
	float MyFloat_NoExport = 123.f;
};
```

## 测试效果 Py 代码:

It is evident that default functions and properties are exported to the script, whereas MyScriptFunc_NoExport and MyFloat_NoExport are not present in the script.

```cpp
class MyPythonLib(BlueprintFunctionLibrary):
    r"""
    My Python Test Libary

    **C++ Source:**

    - **Module**: Insider
    - **File**: MyPythonTest.h

    """
     @property
    def my_float(self) -> float:
        r"""
        (float):  [Read-Write]
        """
        ...
    @my_float.setter
    def my_float(self, value: float) -> None:
        ...
    @classmethod
    def my_script_func_none(cls) -> None:
        r"""
        X.my_script_func_none() -> None
        My Script Func None
        """
        ...
```

## Principle:

The export status of a property or function is determined by the presence or absence of ScriptNoExport.

```cpp
bool IsScriptExposedProperty(const FProperty* InProp)
{
	return !InProp->HasMetaData(ScriptNoExportMetaDataKey)
		&& InProp->HasAnyPropertyFlags(CPF_BlueprintVisible | CPF_BlueprintAssignable);
}

bool IsScriptExposedFunction(const UFunction* InFunc)
{
	return !InFunc->HasMetaData(ScriptNoExportMetaDataKey)
		&& InFunc->HasAnyFunctionFlags(FUNC_BlueprintCallable | FUNC_BlueprintEvent)
		&& !InFunc->HasMetaData(BlueprintGetterMetaDataKey)
		&& !InFunc->HasMetaData(BlueprintSetterMetaDataKey)
		&& !InFunc->HasMetaData(BlueprintInternalUseOnlyMetaDataKey)
		&& !InFunc->HasMetaData(CustomThunkMetaDataKey)
		&& !InFunc->HasMetaData(NativeBreakFuncMetaDataKey)
		&& !InFunc->HasMetaData(NativeMakeFuncMetaDataKey);
}
```