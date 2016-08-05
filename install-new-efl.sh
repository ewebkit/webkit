#/bin/sh

basedir=$(pwd)
cd Tools/efl/new-efl
tar -xf efl-1.18.0-beta2.tar.gz
cd efl-1.18.0-beta2
export LD_LIBRARY_PATH=$basedir/WebKitBuild/DependenciesEFL/Root/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$basedir/WebKitBuild/DependenciesEFL/Root/lib/pkgconfig:$PKG_CONFIG_PATH
./configure --prefix $basedir/WebKitBuild/DependenciesEFL/Root --disable-fribidi --disable-audio --disable-libmount --disable-physics --enable-i-really-know-what-i-am-doing-and-that-this-will-probably-break-things-and-i-will-fix-them-myself-and-send-patches-abb
make --jobs=16
make install 
