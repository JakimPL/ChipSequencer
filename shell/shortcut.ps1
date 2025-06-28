param(
    [string]$ShortcutPath,
    [string]$TargetPath,
    [string]$WorkingDirectory
)

$WshShell = New-Object -comObject WScript.Shell
$Shortcut = $WshShell.CreateShortcut($ShortcutPath)
$Shortcut.TargetPath = $TargetPath
$Shortcut.WorkingDirectory = $WorkingDirectory
$Shortcut.Save()

Write-Host "Created shortcut: $ShortcutPath -> $TargetPath"
