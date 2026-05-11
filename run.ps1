# run.ps1 - Build and launch ConnectifyUI
# Usage (from C:\CONNECT): .\run.ps1

$QT_ROOT = "C:\Qt\6.5.3\mingw_64"

if (-not (Test-Path $QT_ROOT)) {
    $qtBase = "C:\Qt"
    if (Test-Path $qtBase) {
        $versions = Get-ChildItem $qtBase -Directory | Where-Object { $_.Name -match '^\d+\.\d+' } | Sort-Object Name -Descending
        foreach ($ver in $versions) {
            $candidate = "$qtBase\$($ver.Name)\mingw_64"
            if (Test-Path $candidate) { $QT_ROOT = $candidate; Write-Host "Auto-detected Qt at: $QT_ROOT"; break }
        }
    }
}

if (-not (Test-Path $QT_ROOT)) {
    Write-Host "ERROR: Qt not found. Run: Get-ChildItem C:\Qt\" -ForegroundColor Red
    Write-Host "Edit QT_ROOT at the top of run.ps1" -ForegroundColor Yellow
    exit 1
}

$mingwPath = "C:\Qt\Tools\mingw1310_64\bin"
if (-not (Test-Path $mingwPath)) { $mingwPath = "C:\msys64\ucrt64\bin" }
$env:PATH = "$QT_ROOT\bin;$mingwPath;C:\Qt\Tools\Ninja;" + $env:PATH
$env:CMAKE_PREFIX_PATH = $QT_ROOT

Write-Host "==> Qt:    $QT_ROOT"
Write-Host "==> uic:   $((Get-Command uic -ErrorAction SilentlyContinue).Source)"
Write-Host "==> cmake: $((Get-Command cmake -ErrorAction SilentlyContinue).Source)"
Write-Host ""

$BUILD_DIR = "build-ps-debug"
Write-Host "==> Configuring..." -ForegroundColor Cyan
cmake -S . -B $BUILD_DIR -G "Ninja" -DCMAKE_BUILD_TYPE=Debug "-DCMAKE_PREFIX_PATH=$QT_ROOT"
if ($LASTEXITCODE -ne 0) { Write-Host "CONFIGURE FAILED" -ForegroundColor Red; exit 1 }

Write-Host ""; Write-Host "==> Building..." -ForegroundColor Cyan
cmake --build $BUILD_DIR --parallel
if ($LASTEXITCODE -ne 0) { Write-Host "BUILD FAILED" -ForegroundColor Red; exit 1 }

$EXE = "$BUILD_DIR\ConnectifyUI.exe"
if (-not (Test-Path "$BUILD_DIR\.dlls_deployed")) {
    Write-Host ""; Write-Host "==> Deploying Qt DLLs..." -ForegroundColor Cyan
    & "$QT_ROOT\bin\windeployqt.exe" $EXE
    New-Item -ItemType File -Path "$BUILD_DIR\.dlls_deployed" -Force | Out-Null
}

Write-Host ""; Write-Host "==> Launching ConnectifyUI..." -ForegroundColor Green
& $EXE