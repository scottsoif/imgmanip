base_dir=$(pwd)
echo "Installing all necessary dependencies (open install.sh for more detail)"
echo "Unzipping:\n\t $base_dir/lib/jpegsrc.v9e.tar.gz"
echo "*****"

tar -xzf "$base_dir/lib/libpng-1.6.37.tar.xz" -C "$base_dir/lib/"

echo "Installing libpng library"
echo "*****"
cd "$base_dir/lib/libpng-1.6.37"
./configure --prefix="$base_dir/lib/"
make test
make
make install
cd "$base_dir"
rm -rf "$base_dir/lib/libpng-1.6.37"


tar -xzf "$base_dir/lib/jpegsrc.v9e.tar.gz" -C "$base_dir/lib/"

echo "Installing jpeg-9e library"
echo "*****"
cd "$base_dir/lib/jpeg-9e"
./configure --prefix="$base_dir/lib/"
make test
make
make install
cd "$base_dir"
rm -rf "$base_dir/lib/jpeg-9e"

echo "Unzipping:\n\t $base_dir/lib/armadillo-11.0.0.tar.xz"
echo "*****"

tar -xzf "$base_dir/lib/armadillo-11.0.0.tar.xz" -C "$base_dir/lib/"

echo "\n\nInstalling armadillo library"
echo "*****"
cd "$base_dir/lib/armadillo-11.0.0"
cmake . -DCMAKE_INSTALL_PREFIX:PATH="$base_dir/lib/"
make install
cd "$base_dir"

rm -rf "$base_dir/lib/armadillo-11.0.0"


echo "\n\nUnzipping:\n\t $base_dir/lib/boost_1_77_0.zip "
echo "*****"
cd "$base_dir/lib"
echo "Please wait this will take a few minutes"
unzip -q -n boost_1_77_0.zip
echo "Unzip complete"

cd "$base_dir"

echo "\n\n***Done installing***"