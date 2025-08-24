rm -rf components/Bosch-BME68x-Library/CMakeLists.txt
rm -rf components/Bosch-BSEC2-Library/CMakeLists.txt

cd components/Bosch-BME68x-Library
git checkout v1.3.40408
touch CMakeLists.txt
cd ../Bosch-BSEC2-Library
git checkout 1.10.2610
touch CMakeLists.txt
cd ../..

cp extra_cmakes/bme688_CMakeLists.txt components/Bosch-BME68x-Library/CMakeLists.txt
cp extra_cmakes/bsec2_CMakeLists.txt components/Bosch-BSEC2-Library/CMakeLists.txt