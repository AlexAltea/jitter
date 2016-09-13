Jitter
======

Random C++ utilities and classes relying on Just-In-Time compilation via LLVM.

__Disclaimer__: None of this is production-ready, and it definitely does not meet the quality standards anyone would expect from a serious library. Jitter is just a rushed proof-of-concept. If you feel like improving it, please submit a pull request.

## Classes

### `jitter::map`
Matches the specifications of `std::map`. Changes:

* The method `jitter::map::compile` causes the internal key-value pairs *((k1,v1), ..., (kn,vn))* to be compiled into: `switch (k) { case k1: return v1; ... case kn: return vn; }`.

* The methods `jitter::map::operator[]` and `jitter::map::at` will try to run the latest compiled function if available, otherwise they will fall back to `std::map::operator[]` and `std::map::at` respectively.

### `jitter::unordered_map`
Matches the specifications of `std::unordered_map`. Changes:

* The method `jitter::unordered_map::compile` causes the internal key-value pairs *((k1,v1), ..., (kn,vn))* to be compiled into: `switch (k) { case k1: return v1; ... case kn: return vn; }`.

* The methods `jitter::unordered_map::operator[]` and `jitter::unordered_map::at` will to run try the latest compiled function if available, otherwise they will fall back to `std::unordered_map::operator[]` and `std::unordered_map::at` respectively.