# LearnDesignPatterns Docusaurus - Deploy to GitHub Pages
# Usage: .\deploy.ps1 [-Repo "https://github.com/user/repo.git"]

param(
    [string]$Repo = "https://github.com/Tskysheep/LearnDesignPatterns.git",
    [string]$Branch = "gh-pages"
)

$ErrorActionPreference = "Stop"

Write-Host ">> Building Docusaurus site..." -ForegroundColor Cyan
npm run build
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

Write-Host ">> Deploying to $Branch..." -ForegroundColor Cyan
Push-Location build

git init
git add -A
git commit -m "deploy $(Get-Date -Format 'yyyy-MM-dd HH:mm')"
git branch -M main
git push -f $Repo "main:$Branch"

Pop-Location

Write-Host ">> Done! Site deployed to $Branch" -ForegroundColor Green
