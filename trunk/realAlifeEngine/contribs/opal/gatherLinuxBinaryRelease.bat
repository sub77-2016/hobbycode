mkdir opal-0.3.1-linux
cd opal-0.3.1-linux
copy ..\linuxBinaryReadme.txt .
copy ..\changelog.txt .
copy ..\license-LGPL.txt .
copy ..\license-BSD.txt .

mkdir include
copy ..\src\*.h include

mkdir lib
mkdir lib\single
mkdir lib\double

cd ..
