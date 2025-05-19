$notasSharp = @("C#", "D#", "F#", "G#", "A#")
$octavas = 1..7
foreach ($n in $notasSharp) {
    foreach ($o in $octavas) {
        $filename = "$n$o" + "vH.wav"
        if (-not (Test-Path $filename)) {
            Write-Host "❌ Falta: $filename"
        }
    }
}
