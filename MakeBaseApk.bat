﻿"D:\UE4\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project=“C:\ProjectSR\ProjectSR.uproject" -clientconfig=Development -platform=Android_ETC1 -targetplatform=Android -cookflavor=ETC1 -cook -map=StartupMap -stage -package -cmdline=StartupMap -compressed -pak -build

"D:\UE4\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project=“C:\ProjectSR\ProjectSR.uproject" -clientconfig=Development -platform=Android_ETC1 -targetplatform=Android -cookflavor=ETC1 -cook -map=StartupMap -cmdline=StartupMap -build -unversionedcookedcontent -createreleaseversion=ReleaseContent

"D:\UE4\UnrealEngine\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun -project=“C:\ProjectSR\ProjectSR.uproject" -clientconfig=Development -platform=Android_ETC1 -targetplatform=Android -cookflavor=ETC1 -build –cook -MAPINISECTION=DownloadContent -unversionedcookedcontent -BasedOnReleaseVersion=ReleaseContent -DLCName=DownloadContent -stage -compressed -pak -cmdline=Main