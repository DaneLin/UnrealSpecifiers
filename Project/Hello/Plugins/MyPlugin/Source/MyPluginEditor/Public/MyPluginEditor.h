// Copyright 2018 Jianzhao Fu. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FMyPluginEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};