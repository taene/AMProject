# 01_Git_Infra

## [2026-08-26] Git 프로젝트 초기화 및 형상관리 파이프라인 구축 트러블슈팅

프로젝트 초기 세팅 과정에서 발생한 인증 실패, 원격 저장소 충돌, 그리고 캐시 데이터 업로드 문제를 해결하고 최적의 워크플로우를 확립했다.

### 1. GitHub HTTPS 비밀번호 인증 거부 (Authentication failed)
* **현상:** GitHub 원격 저장소에 코드를 push하는 과정에서 비밀번호 인증 거부(Authentication failed) 상황이 발생했다.
* **원인:** GitHub의 보안 정책 변경으로 인해 더 이상 계정 비밀번호로 push를 수행할 수 없는 상황이 발생했기 때문이다.
* **해결 파이프라인:** 
  * 보안과 자동화(CI/CD) 관점에서 가장 완벽한 해결책인 SSH 키 기반 인증을 적용했다. 
  * `ssh-keygen`으로 키 쌍을 생성하고, 공개키를 GitHub에 등록하여 영구적이고 안전한 암호화 터널을 개통하여 문제를 해결했다.

### 2. 원격 저장소 충돌 (non-fast-forward 에러)
* **현상:** 원격 저장소로 push를 시도했을 때 `non-fast-forward` 에러가 발생하며 push가 거부되었다.
* **원인:** GitHub에서 레포지토리 생성 시 기본 파일(README 등)이 생성되어 로컬 역사와 원격 역사가 단절되는 현상이 발생했기 때문이다.
* **해결 파이프라인:** 
  * 현재 로컬의 언리얼 프로젝트가 유일하고 정확한 기준(진실의 원천)이므로, `--force` 옵션을 사용하여 쓸데없는 원격 역사를 강제로 덮어씌워 해결했다.

### 3. Git 캐시에 잔존한 더미 데이터 업로드 문제 (.idea, DerivedDataCache)
* **현상:** `.gitignore` 파일에 무시 규칙을 작성했음에도, 수 기가바이트의 캐시/바이너리 폴더들이 그대로 push 되는 상황이었다.
* **원인:** `.gitignore`를 나중에 수정하면서, 이미 Git 추적망(Index Cache)에 해당 파일들이 올라가버렸기 때문이다. 캐시를 지우고(`git rm -r --cached`) 재시도를 했으나 규칙 저장 문제로 꼬이게 되었다.
* **해결 파이프라인:** 
  * 초기 세팅 단계라는 점을 적극 활용하여, 의미 없는 커밋 히스토리와 숨겨진 찌꺼기 데이터를 남기지 않기 위해 `.git` 폴더를 폭파하고 완벽한 상태로 다시 초기화하는 방식을 채택했다.
  * 아래의 워크플로우를 순서대로 실행하여 과거의 꼬인 내역을 모두 지우고 완벽하게 깔끔한 상태로 언리얼 프로젝트를 업로드했다.

```bash
# 1. 과거 기록 완전 소거 및 Git 초기화
rm -rf .git
git init
git branch -M main

# 2. gitignore 적용 및 완벽한 첫 스냅샷 생성
git add .
git commit -m "Initial commit: Base Unreal project setup"

# 3. SSH 기반 원격 저장소 연결 및 강제 덮어쓰기
git remote add origin git@github.com:taene/AMProject.git
git push -u origin main --force
```