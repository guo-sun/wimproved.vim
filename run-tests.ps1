function unzip($path) {
    Add-Type -assembly 'system.io.compression.filesystem'
<<<<<<< HEAD
    [io.compression.zipfile]::ExtractToDirectory($path + '.zip', '.')
=======
    [io.compression.zipfile]::ExtractToDirectory($path + '.zip', $path)
>>>>>>> 823ab137258b5cc949a313682017be32e79a441f
}

(npm install) | Out-Null

$path = $env:PATH

if (-not (Test-Path 'vim74-kaoriya-win32' -PathType container)) {
    curl http://files.kaoriya.net/vim/vim74-kaoriya-win32.zip -OutFile vim74-kaoriya-win32.zip
    unzip 'vim74-kaoriya-win32'
}

Write-Output 'Testing wimproved32.dll'
$env:PATH = $path + ";$pwd\vim74-kaoriya-win32"
.\node_modules\.bin\mocha

if (-not (Test-Path 'vim74-kaoriya-win64' -PathType container)) {
    curl http://files.kaoriya.net/vim/vim74-kaoriya-win64.zip -OutFile vim74-kaoriya-win64.zip
    unzip 'vim74-kaoriya-win64'
}

Write-Output 'Testing wimproved64.dll'
$env:PATH = $path + ";$pwd\vim74-kaoriya-win64"
.\node_modules\.bin\mocha

