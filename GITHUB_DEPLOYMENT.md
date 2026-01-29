# GitHub deployment

Notes for deploying or releasing **Qt + OpenCV Camera Player (QML)** via GitHub.

## Releases

1. **Tag a version**
   ```bash
   git tag -a v0.1.0 -m "Release 0.1.0"
   git push origin v0.1.0
   ```

2. **Create a GitHub Release**
   - Repo → **Releases** → **Draft a new release**
   - Choose tag `v0.1.0`, add title and notes
   - Attach build artifacts (e.g. Windows `.exe` + DLLs, or Linux tarball) if desired

## GitHub Actions (CI build)

Example workflow to build on push/PR (Linux):

```yaml
# .github/workflows/build.yml
name: Build
on: [push, pull_request]
jobs:
  build-linux:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-qt@v1
        with:
          qt-version: '6.6'
      - name: Install OpenCV
        run: sudo apt-get update && sudo apt-get install -y libopencv-dev
      - name: Configure and build
        run: |
          cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
          cmake --build build -j
```

Adjust Qt version and OpenCV install as needed for your repo.

## GitHub Pages (web / docs)

- Enable **Pages** in repo **Settings → Pages** (e.g. deploy from `main` / `docs` or GitHub Actions).
- Put static site or docs in the chosen branch/folder; the **web/** folder in this project can be used for a simple landing or docs if you add an entry page and link it from the repo description.

## Checklist

- [ ] Tag and push version for releases
- [ ] Attach platform-specific binaries to releases if needed
- [ ] Add/update `.github/workflows/` for CI (build, optional tests)
- [ ] Configure Pages if you want a project site or docs
