[CmdletBinding()]
param(
    [ValidateSet('Debug','Release','RelWithDebInfo')][string]$Configuration = 'Release',
    [string]$BuildDirectory = 'build',
    [string]$ToolchainBin = '',
    [switch]$Package,
    [switch]$SkipTests
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
$build = if ([IO.Path]::IsPathRooted($BuildDirectory)) { $BuildDirectory } else { Join-Path $project $BuildDirectory }
$previousPath = $env:PATH
$compilerBin = $null
$candidates = if ($ToolchainBin) { @($ToolchainBin) } else { @('C:\msys64\ucrt64\bin','D:\msys64\ucrt64\bin','D:\GFT\msys64\ucrt64\bin') }
foreach ($candidate in $candidates) {
    if (Test-Path -LiteralPath (Join-Path $candidate 'g++.exe') -PathType Leaf) {
        $compilerBin = (Resolve-Path -LiteralPath $candidate).Path
        break
    }
}
if ($ToolchainBin -and -not $compilerBin) { throw 'The requested toolchain folder does not contain g++.exe.' }

try {
    $toolPaths = @()
    if ($compilerBin) { $toolPaths += $compilerBin }
    foreach ($localTool in @('.tools\python\cmake\data\bin','.tools\python\bin','.tools\python\ninja\data\bin')) {
        $toolDirectory = Join-Path $project $localTool
        if (Test-Path -LiteralPath $toolDirectory -PathType Container) { $toolPaths += $toolDirectory }
    }
    $env:PATH = ($toolPaths + $previousPath) -join [IO.Path]::PathSeparator
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if (-not $cmake) { throw 'CMake was not found. Install CMake and a C++17 compiler, or use an MSYS2 UCRT64 toolchain with CMake and Ninja.' }
    $configureArgs = @('-S',$project,'-B',$build,"-DCMAKE_BUILD_TYPE=$Configuration",'-DBUILD_TESTING=ON')
    if ($compilerBin) {
        if (-not (Get-Command ninja -ErrorAction SilentlyContinue)) { throw 'Ninja was not found. Install Ninja in the selected toolchain.' }
        $configureArgs += @('-G','Ninja',"-DCMAKE_C_COMPILER=$compilerBin/gcc.exe","-DCMAKE_CXX_COMPILER=$compilerBin/g++.exe")
    }
    & $cmake.Source @configureArgs
    if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
    & $cmake.Source --build $build --config $Configuration --parallel
    if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
    if (-not $SkipTests) {
        & ctest --test-dir $build -C $Configuration --output-on-failure
        if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
    }
    if ($Package) {
        & cpack --config (Join-Path $build 'CPackConfig.cmake') -C $Configuration -B (Join-Path $build 'package')
        if ($LASTEXITCODE -ne 0) { throw 'Packaging failed.' }
    }
    Write-Host "Run: $(Join-Path $build 'bin\OpenGLApp.exe')"
} finally {
    $env:PATH = $previousPath
}
