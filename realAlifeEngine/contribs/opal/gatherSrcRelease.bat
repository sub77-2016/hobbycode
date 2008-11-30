mkdir opal-0.3.1-src
cd opal-0.3.1-src
copy ..\readme.txt .
copy ..\changelog.txt .
copy ..\license-LGPL.txt .
copy ..\license-BSD.txt .
copy ..\SConstruct .

mkdir src
copy ..\src\SConscript src
copy ..\src\*.h src
copy ..\src\*.cpp src
mkdir src\ODE
copy ..\src\ODE\SConscript src\ODE
copy ..\src\ODE\*.h src\ODE
copy ..\src\ODE\*.cpp src\ODE
mkdir src\external
mkdir src\external\tinyxml
copy ..\src\external\tinyxml\SConscript src\external\tinyxml
copy ..\src\external\tinyxml\*.h src\external\tinyxml
copy ..\src\external\tinyxml\*.cpp src\external\tinyxml

mkdir vc71
copy ..\vc71\opal.sln vc71
copy ..\vc71\opal-ode.vcproj vc71

mkdir vc8
copy ..\vc8\opal.sln vc8
copy ..\vc8\opal-ode.vcproj vc8

mkdir tools
mkdir tools\3dsmax
copy ..\tools\3dsmax\*.ms tools\3dsmax
copy ..\tools\3dsmax\*.txt tools\3dsmax
mkdir tools\blender
copy ..\tools\blender\*.py tools\blender
copy ..\tools\blender\*.txt tools\blender

mkdir samples
copy ..\samples\readme.txt samples
mkdir samples\data
mkdir samples\data\blueprints
copy ..\samples\data\blueprints\*.xml samples\data\blueprints
mkdir samples\data\materials
copy ..\samples\data\materials\*.material samples\data\materials
mkdir samples\data\models
copy ..\samples\data\models\*.mesh samples\data\models
mkdir samples\data\overlays
copy ..\samples\data\overlays\*.overlay samples\data\overlays
mkdir samples\data\packs
copy ..\samples\data\packs\*.zip samples\data\packs
mkdir samples\data\textures
copy ..\samples\data\textures\*.* samples\data\textures
mkdir samples\src
copy ..\samples\src\*.h samples\src
copy ..\samples\src\*.cpp samples\src
mkdir samples\bin
copy ..\samples\bin\readme.txt samples\bin
copy ..\samples\bin\*.cfg samples\bin
mkdir samples\playpen
copy ..\samples\playpen\SConstruct samples\playpen
copy ..\samples\playpen\main.cpp samples\playpen
mkdir samples\playpen\vc71
copy ..\samples\playpen\vc71\playpen.sln samples\playpen\vc71
copy ..\samples\playpen\vc71\playpen.vcproj samples\playpen\vc71
mkdir samples\playpen\vc8
copy ..\samples\playpen\vc8\playpen.sln samples\playpen\vc8
copy ..\samples\playpen\vc8\playpen.vcproj samples\playpen\vc8
mkdir samples\simple
copy ..\samples\simple\SConstruct samples\simple
copy ..\samples\simple\*.h samples\simple
copy ..\samples\simple\*.cpp samples\simple
mkdir samples\simple\vc71
copy ..\samples\simple\vc71\simple.sln samples\simple\vc71
copy ..\samples\simple\vc71\simple.vcproj samples\simple\vc71
mkdir samples\simple\vc8
copy ..\samples\simple\vc8\simple.sln samples\simple\vc8
copy ..\samples\simple\vc8\simple.vcproj samples\simple\vc8
cd ..
