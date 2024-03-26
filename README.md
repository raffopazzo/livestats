# Live Stats

C++ library to compute real-time statistics.

# Main Concepts

An `Estimator` is a device that can accept real-time observations,
for example to compute real-time mean, average.
Upon receiving a new observation an estimator must update its internal state;
some computation may be deferred until the `get()` method is invoked,
but they must have O(1) complexity.

Additionally, some estimators can be combined with other estimators to produce a pipeline.
For example, `zscore_outlier_estimator_adaptor` and a `welford_variance_estimator`
can be combined together to produce a variance estimator that rejects outliers.

Welford estimators (to detect outliers across the entire sequence) or
sliding window estimators for an "adaptive" outlier detector.

Finally, estimators are templated over the type of the sample value,
allowing to use both integer and floating-point types, as well as
other number-like types such as `boost::multiprecision::cpp_int`.

# Estimators Inventory

Some of the currently implemented estimators include:
  - `naive_mean_estimator`:
    implements the naive algorithm to compute means,
    by keeping the running sum of all samples and dividing by the sample count
    when necessary. With primitive types it can overflow, so can only be used
    if the input data is sanity checked, but it can safely be combined with
    `boost::multiprecision::cpp_int` since this one does not overflow;
  - `welford_mean_estimator`:
    implements the classic Welford algorithm to update the running mean;
    it does not overflow on unsigned integer types;
  - `welford_variance_estimator`:
    implements the classic Welford algorithm to update the running variance;
  - `sliding_window_mean_estimator`:
    computes the mean over a sliding window of N samples;
  - `sliding_window_variance_estimator`:
    computes the variance over a sliding window of N samples;
  - `sliding_time_window_mean_estimator`:
    computes the mean over one or more time windows, eg 1ms, 50ms and 100ms;
  - `sliding_time_window_variance_estimator`:
    computes the variance (and mean) over one or more time windows;
  - `zscore_outlier_estimator_adaptor`:
    filters out outliers before passing new observation to another estimator.

# How to build using devbox

First you need to build the devbox docker image via:

(NB if you're behind a proxy you might need to edit the Dockerfile)

```
$ ./tools/devbox/build-image.sh
```

Then you can run conan and cmake as normal within a devbox environment:

```
$ ./tools/devbox/devbox.sh
...$ conan profile new --detect default
...$ conan profile update settings.compiler.libcxx=libstdc++11 default
...$ conan install . --install-folder build --build=boost
...$ cmake . -Bbuild -DCMAKE_MODULE_PATH=$PWD/build
...$ cmake --build build -j
...$ ctest --test-dir build -VV
...$ exit
```

Alternatively:

```
$ ./tools/devbox/devbox.sh conan profile new --detect default
$ ./tools/devbox/devbox.sh conan profile update settings.compiler.libcxx=libstdc++11 default
$ ./tools/devbox/devbox.sh conan install . --install-folder build --build=boost
$ ./tools/devbox/devbox.sh cmake . -Bbuild -DCMAKE_MODULE_PATH=$PWD/build
$ ./tools/devbox/devbox.sh cmake --build build -j
$ ./tools/devbox/devbox.sh ctest --test-dir build -VV
```

# How to without devbox

You will need:
 - conan 1.55
 - cmake 3.20 or greater
 - gcc11 or greater (other compilers have not be tested)

You shoud then be able to follow the same `conan/cmake/ctest` steps documented above.
