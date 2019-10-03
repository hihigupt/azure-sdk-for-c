$location = $PSScriptRoot
$build = Join-Path $location "build"

$curl = Join-Path $location "dep/curl"
$curlBuild = Join-Path $curl "build"

CMake -S $curl -B $curlBuild
CMake -B $curlBuild

CMake -S $location -B $build
CMake --build $build
