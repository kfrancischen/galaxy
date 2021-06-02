#ifndef CPP_INTERNAL_GALAXY_STATS_INTERNAL_H_
#define CPP_INTERNAL_GALAXY_STATS_INTERNAL_H_

#include "absl/strings/string_view.h"
#include "opencensus/stats/stats.h"
#include "opencensus/tags/tag_key.h"

namespace galaxy {
    namespace stats {
        namespace internal {
            ABSL_CONST_INIT const absl::string_view kLatencyMeasureName = "grpc/latency";
            ABSL_CONST_INIT const absl::string_view kCountMeasureName = "grpc/count";
            opencensus::stats::MeasureDouble LatencyMsMeasure();
            opencensus::stats::MeasureInt64 QueryCountMeasure();
            opencensus::tags::TagKey MethodKey();
        }
    }
}


#endif  // CPP_INTERNAL_GALAXY_STATS_INTERNAL_H_
