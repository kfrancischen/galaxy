#include "cpp/internal/galaxy_stats_internal.h"

namespace galaxy {
    namespace stats {
        opencensus::stats::MeasureDouble internal::LatencyMsMeasure() {
            static const auto measure = opencensus::stats::MeasureDouble::Register(
                internal::kLatencyMeasureName, "The latency in milliseconds", "ms");
            return measure;
        }

        opencensus::stats::MeasureInt64 internal::QueryCountMeasure() {
            static const auto measure = opencensus::stats::MeasureInt64::Register(
                kCountMeasureName, "The number of queries", "By");
            return measure;
        }

        opencensus::tags::TagKey internal::MethodKey() {
            static const auto key = opencensus::tags::TagKey::Register("method");
            return key;
        }
    }
}
