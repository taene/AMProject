# 00_Ucpp

## [2026-08-27] Pointer & Reference

언리얼 엔진 코드베이스에서 포인터(Pointer)와 참조(Reference)를 구분해서 쓰는 기준을 정리한다. `UAMAssetManager::Get()` 싱글톤 접근자의 반환 타입을 분석하며 정리한 내용이다.

### 1. 참조(Reference) 반환 — "항상 유효함"을 타입으로 보장

* **핵심:** 함수가 절대 실패하지 않고(실패 시 크래시로 종료), 항상 유효한 객체 하나만 돌려준다면 반환 타입을 참조로 잡는다.
* **이유:** 참조는 애초에 `nullptr`을 가리킬 수 없는 문법이기 때문에, 호출부에서 null 체크 없이 바로 멤버 함수를 호출할 수 있다. "이 함수는 항상 유효한 객체를 준다"는 계약을 타입 시스템 레벨에서 강제하는 셈이다.

```cpp
// AMAssetManager.cpp
UAMAssetManager& UAMAssetManager::Get()
{
    check(GEngine); // 실패 시 여기서 크래시 → 이후 코드는 항상 유효를 전제

    if (UAMAssetManager* Singleton = Cast<UAMAssetManager>(GEngine->AssetManager))
    {
        return *Singleton; // 포인터를 역참조해 참조로 반환
    }

    UE_LOG(LogAM, Fatal, TEXT("invalid AssetManagerClassname...")); // 여기서도 크래시
    return *NewObject<UAMAssetManager>();
}

// 호출부: null 체크 없이 바로 사용
UAMAssetManager::Get().DoSomething();
```

### 2. 포인터(Pointer)가 필요한 경우

참조는 "없음"과 "재할당"을 표현할 수 없기 때문에, 아래 상황에서는 포인터를 쓴다.

* **2-1. 결과가 없을 수도 있을 때 (Null 가능성)**
  `Cast<T>()`, `FindObject`, `GetOwner()` 등은 실패 시 `nullptr`을 반환해야 하므로 포인터를 쓴다.
  ```cpp
  UAMAssetManager* Singleton = Cast<UAMAssetManager>(GEngine->AssetManager);
  if (Singleton) { ... } // 캐스팅 실패 가능성이 있으므로 체크 필요
  ```

* **2-2. 나중에 다른 대상을 가리키도록 재할당해야 할 때**
  참조는 한번 초기화되면 대상을 바꿀 수 없다(재대입 시 대상 객체의 값이 바뀌어버림). 포인터는 자유롭게 재할당된다.
  ```cpp
  UAMAssetManager* Current = nullptr;
  Current = &A;
  Current = &B; // OK, 가리키는 대상 자체를 변경

  UAMAssetManager& Ref = A;
  Ref = B; // 대상이 B로 바뀌는 게 아니라 A의 내용에 B가 대입(복사)됨
  ```

* **2-3. UPROPERTY 멤버 변수 / GC 추적 대상**
  UObject를 클래스 멤버로 보유할 때는 거의 항상 포인터(`TObjectPtr` 권장)를 쓴다. 참조는 기본 생성자에서 즉시 초기화해야 하고 대입 연산자가 깨지기 때문에 멤버 변수로 다루기 부적합하며, GC 추적이 필요한 UObject는 UE 시스템 자체가 포인터 기반으로 설계되어 있다.
  ```cpp
  UPROPERTY()
  TObjectPtr<UAMAssetManager> CachedManager;
  ```

* **2-4. 컨테이너(배열 등)에 담을 때**
  `TArray<T&>`는 문법적으로 불가능하다. 참조들의 집합이 필요하면 포인터 배열을 쓴다.
  ```cpp
  TArray<UAMAssetManager*> Managers;
  ```
