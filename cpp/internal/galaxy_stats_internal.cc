#include "cpp/internal/galaxy_stats_internal.h"

namespace galaxy {
    namespace stats {
        opencensus::stats::MeasureDouble internal::LatencyMsMeasure() {
            static const auto measure = opencensus::stats::MeasureDouble::Register(
                internal::kLatencyMeasureName, "The latency in milliseconds", "ms");
            return measure;
        }

        opencensus::tags::TagKey internal::MethodKey() {
            static const auto key = opencensus::tags::TagKey::Register("method");
            return key;
        }

        opencensus::tags::TagKey internal::ClientMethodTagKey() {
            static const auto method_tag_key =
                opencensus::tags::TagKey::Register("grpc_client_method");
            return method_tag_key;
        }

        opencensus::tags::TagKey internal::ClientStatusTagKey() {
            static const auto status_tag_key =
                opencensus::tags::TagKey::Register("grpc_client_status");
            return status_tag_key;
        }
    }
}
