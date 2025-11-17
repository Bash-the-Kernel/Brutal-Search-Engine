<#
Simple PowerShell script to download a few Project Gutenberg plain-text books into `sample_texts/gutenberg/`.
Run from the repository root in PowerShell (requires internet access).
#>
Param(
    [string]$dest = "sample_texts/gutenberg"
)

Write-Host "Creating $dest"
New-Item -ItemType Directory -Force -Path $dest | Out-Null

# List of Gutenberg plain text URLs (these URLs are commonly available; they may change on the Gutenberg site).
$books = @(
    @{ id=1342; name="pride_and_prejudice"; url="https://www.gutenberg.org/files/1342/1342-0.txt" },
    @{ id=2701; name="moby_dick"; url="https://www.gutenberg.org/files/2701/2701-0.txt" },
    @{ id=11;   name="alice_in_wonderland"; url="https://www.gutenberg.org/files/11/11-0.txt" }
)

foreach ($b in $books) {
    $out = Join-Path $dest ("{0}_{1}.txt" -f $b.id, $b.name)
    Write-Host "Downloading $($b.name) to $out"
    try {
        Invoke-WebRequest -Uri $b.url -OutFile $out -UseBasicParsing -ErrorAction Stop
    } catch {
        Write-Warning "Failed to download $($b.url): $_"
    }
}

Write-Host "Done. You can index the downloaded files with:`n  .\search_engine.exe index $dest index.dat`" 
