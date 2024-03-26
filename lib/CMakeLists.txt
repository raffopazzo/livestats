add_library(livestats_lib
    # include
    include/livestats/estimator.hpp
    include/livestats/naive_mean_estimator.hpp
    include/livestats/sliding_time_window_mean_estimator.hpp
    include/livestats/sliding_time_window_variance_estimator.hpp
    include/livestats/sliding_window_mean_estimator.hpp
    include/livestats/sliding_window_variance_estimator.hpp
    include/livestats/welford_mean_estimator.hpp
    include/livestats/welford_variance_estimator.hpp
    include/livestats/zscore_outlier_estimator_adaptor.hpp
    # src
    src/estimator.cpp
    src/naive_mean_estimator.cpp
    src/sliding_time_window_mean_estimator.cpp
    src/sliding_time_window_variance_estimator.cpp
    src/sliding_window_mean_estimator.cpp
    src/sliding_window_variance_estimator.cpp
    src/welford_mean_estimator.cpp
    src/welford_variance_estimator.cpp
    src/zscore_outlier_estimator_adaptor.cpp
    )
add_library(LiveStats ALIAS livestats_lib)
target_compile_features(livestats_lib PUBLIC cxx_std_20)
target_link_libraries(livestats_lib PUBLIC Boost::Boost)
target_include_directories(livestats_lib PUBLIC include)
