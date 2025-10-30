# Refactoring Analysis: util::Map → std::map

## Current State Assessment

### Custom Container Design
The codebase uses custom containers (`util::Map`, `util::List`, `util::IntMap`) that were likely written before C++11 or for specific performance/memory characteristics:

1. **util::Map**: Hash table with:
   - Manual memory management (MapLink* pointers)
   - Custom hash function using Object::hash()
   - Linked list for insertion order preservation
   - Hash table for O(1) lookup
   - Manual iterator pattern (init/hasNext/next)

2. **util::List**: Doubly-linked list with:
   - Manual memory management (ListLink* pointers)
   - Iterator pattern matching Map

3. **util::IntMap**: Specialized integer-key map

### Usage Patterns in Codebase

**Files affected**: 9 core files in src/
**Instantiations**: 49+ `new Map/List/IntMap` calls
**Key locations**:
- `infect/System.h`: Core data structures (patients, facilities, episodes)
- `infect/SystemHistory.h`: Historical tracking
- `modeling/` various model implementations
- `lognormal/` specialized models

### Current Pain Points

1. **Manual Memory Management**
   - Potential for memory leaks
   - Dangling pointers causing segfaults
   - No RAII principles

2. **Raw Pointer Returns**
   - `Map::nextValue()` returns `Object*`
   - Type safety relies on runtime casting
   - No compile-time type checking

3. **Iterator Pattern Issues**
   - Manual init/hasNext/next pattern error-prone
   - No range-based for loop support
   - Iterator state stored in container (not thread-safe)

4. **Rcpp Integration Challenges**
   - Type conversion issues (as we just experienced)
   - Need RCPP_EXPOSED_AS/RCPP_EXPOSED_CLASS for each type
   - Pointers don't automatically convert to R objects

## Refactoring Option 1: Full STL Migration

### Replace with std::map + std::shared_ptr

```cpp
// Before
class System {
    IntMap *pat;
    IntMap *fac;
    Map *pepis;  // Map<Patient*, Map<Episode*, Episode*>>
};

// After
class System {
    std::map<int, std::shared_ptr<Patient>> patients;
    std::map<int, std::shared_ptr<Facility>> facilities;
    std::map<Patient*, std::map<Episode*, std::shared_ptr<Episode>>> episodes;
};
```

### Advantages
1. **Memory Safety**
   - Automatic memory management via smart pointers
   - No manual delete calls
   - RAII principles enforced

2. **Type Safety**
   - Template parameters enforce types at compile time
   - No runtime casting needed
   - Iterator types are properly typed

3. **Modern C++ Features**
   - Range-based for loops: `for (auto& [key, val] : patients)`
   - Move semantics for efficiency
   - Standard algorithms (std::find_if, etc.)

4. **Better Rcpp Integration**
   - std::map can be wrapped automatically
   - Smart pointers integrate better with Rcpp

### Disadvantages
1. **Massive Code Changes**
   - ~49+ allocation sites to update
   - All iteration patterns need rewriting
   - Every file that uses these containers

2. **API Breaking Changes**
   - All public interfaces change
   - R wrapper code needs complete rewrite
   - Tests need updating

3. **Performance Considerations**
   - std::map is O(log n) not O(1) like hash table
   - Could use std::unordered_map but different tradeoffs
   - Memory overhead of smart pointers

4. **Time Estimate**
   - **Core changes**: 3-5 days
   - **Testing/debugging**: 2-3 days
   - **R integration updates**: 1-2 days
   - **Total**: ~1-2 weeks full-time

## Refactoring Option 2: Targeted Safety Improvements

Keep util::Map but make it safer:

```cpp
// Add safe wrappers
template<typename T>
class TypedMap {
    util::Map* map_;
public:
    T* get(Object* key) {
        return static_cast<T*>(map_->get(key));
    }
    
    class Iterator {
        util::Map* map_;
        bool finished_;
    public:
        Iterator(util::Map* m) : map_(m), finished_(false) {
            map_->init();
        }
        T* next() {
            if (!map_->hasNext()) {
                finished_ = true;
                return nullptr;
            }
            return static_cast<T*>(map_->nextValue());
        }
        bool hasNext() const { return !finished_ && map_->hasNext(); }
    };
    
    Iterator begin() { return Iterator(map_); }
};
```

### Advantages
1. **Incremental Changes**
   - Can be done gradually
   - Existing code continues to work
   - Add safety layer by layer

2. **Type Safety with Minimal Changes**
   - Templates provide compile-time checking
   - Existing Map implementation unchanged

3. **Time Estimate**
   - **Wrapper creation**: 1-2 days
   - **Gradual migration**: 2-3 days
   - **Total**: ~1 week

### Disadvantages
1. **Still Manual Memory**
   - Doesn't solve core memory management issues
   - Leak potential remains

2. **Two Systems**
   - Old and new code coexist
   - Confusion about which to use

## Refactoring Option 3: Smart Pointer Wrapper (Hybrid)

Wrap existing Map with smart pointer management:

```cpp
// Create RAII wrapper for util::Map
class SafeMap {
    std::unique_ptr<util::Map> map_;
public:
    SafeMap() : map_(new util::Map()) {}
    ~SafeMap() = default;  // automatic cleanup
    
    util::Map* get() { return map_.get(); }
    
    // Prevent copying, allow moving
    SafeMap(const SafeMap&) = delete;
    SafeMap& operator=(const SafeMap&) = delete;
    SafeMap(SafeMap&&) = default;
    SafeMap& operator=(SafeMap&&) = default;
};
```

### Advantages
1. **Minimal Code Changes**
   - Mostly just allocation sites
   - Internal Map code unchanged
   - Iteration patterns unchanged

2. **Memory Safety**
   - Automatic deallocation
   - No leaks from Map container itself

3. **Time Estimate**
   - **Wrapper + conversion**: 2-3 days
   - **Testing**: 1-2 days
   - **Total**: ~3-5 days

### Disadvantages
1. **Objects Inside Map Still Raw**
   - Map contains Object* still
   - Doesn't solve the Episode/Event pointer issues
   - Partial solution only

## Recommendation

Given your current situation (segfaults in tests), I recommend a **phased approach**:

### Phase 1: Immediate Fix (1-2 days)
- Fix current segfault by properly exposing Episode class
- Add null checks in critical paths
- Document current memory ownership patterns

### Phase 2: Safety Layer (1 week)
- Implement Option 3 (Smart Pointer Wrapper)
- Wrap Map/List/IntMap in unique_ptr wrappers
- Update allocation sites to use SafeMap/SafeList

### Phase 3: Type Safety (1-2 weeks) 
- Add TypedMap<T> wrappers (Option 2)
- Migrate System class to use typed versions
- Better compile-time checking

### Phase 4: Full Modernization (2-4 weeks - future work)
- Consider std::map migration for new features
- Keep existing code stable
- Gradual migration as needed

## Current Segfault Root Cause

The immediate issue isn't the container choice, but:
1. Episode class not properly exposed to Rcpp
2. Type erasure when Map returns Object*
3. Need to use `nextValue()` not `step()` for typed access

**Immediate fix**: Properly expose Episode properties in Module-Infect.cpp (already done)
**Pattern fix**: Use `nextValue()` which preserves type info

## Risk Assessment

| Approach | Risk | Benefit | Time | Priority |
|----------|------|---------|------|----------|
| Fix current test | Low | High | Hours | **Do Now** |
| Smart ptr wrappers | Medium | High | Days | Phase 2 |
| Typed wrappers | Medium | Medium | 1 week | Phase 3 |
| Full std::map | High | Very High | 2+ weeks | Future |

The custom containers work but are fragile. A complete rewrite to STL would be safest long-term but is a major undertaking. The phased approach lets you get immediate safety improvements while planning for eventual modernization.
