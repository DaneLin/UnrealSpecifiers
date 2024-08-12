# EnumValueDisplayNameOverrides

Usage: UPROPERTY
Feature: Blueprint
Type: string="abc"
Description: 改变枚举属性值上的显示名字
Example: meta=(EnumValueDisplayNameOverrides=”First=Cat;Second=Dog”)
Status: Linked
Group: Enum Property
Parent item: ValidEnumValues (ValidEnumValues.md)

给枚举属性上的枚举值进行一些改名，可以改变多个，按照“A=B;C=D”的格式列出即可。收集到的信息是TMap<FName, FText>映射，因此要同时提供原枚举值名称和新的显示名称配对。

原理代码见ValidEnumValues上的代码

测试代码：

```cpp
UENUM(BlueprintType)
enum class EMyPropertyTestEnum : uint8
{
	First,
	Second,
	Third,
	Forth,
	Fifth,
};

UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EnumValueDisplayNameOverrides = "First=Cat;Second=Dog"))
EMyPropertyTestEnum MyEnumWithDisplayNameOverrides;
```

蓝图效果：

可见实际上改变了First、Second的显示名字。

![Untitled](EnumValueDisplayNameOverrides/Untitled.png)