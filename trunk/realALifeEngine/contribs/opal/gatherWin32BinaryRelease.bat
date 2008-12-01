mkdir opal-0.3.1-win32
cd opal-0.3.1-win32
copy ..\win32BinaryReadme.txt .
copy ..\changelog.txt .
copy ..\license-LGPL.txt .
copy ..\license-BSD.txt .

mkdir include
copy ..\src\*.h include

mkdir lib
mkdir lib\single
mkdir lib\double

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
mkdir samples\bin
mkdir samples\bin\release
copy ..\samples\bin\*.cfg samples\bin\release
copy ..\samples\bin\Release\*.exe samples\bin\release
copy ..\samples\bin\Release\*.dll samples\bin\release

cd ..
