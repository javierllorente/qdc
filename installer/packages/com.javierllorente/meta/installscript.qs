function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "@TargetDir@/qdc.exe", "@StartMenuDir@/qdc.lnk",
            "workingDirectory=@TargetDir@");
        component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/Maintenance tool.lnk",
            "workingDirectory=@TargetDir@");
        component.addElevatedOperation("Execute",  "cmd /C reg add \"HKEY_CURRENT_USER\\Software\\qdc\" /f", "UNDOEXECUTE", "cmd /C reg delete \"HKEY_CURRENT_USER\\Software\\qdc\" /f");
    }
}
