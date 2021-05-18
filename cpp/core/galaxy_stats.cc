#include "cpp/core/galaxy_stats.h"
#include "cpp/internal/galaxy_stats_internal.h"
#include "glog/logging.h"

namespace galaxy {
    namespace stats {
        void RegisterViews() {
            // Register latency view.
            internal::LatencyMsMeasure();
            const opencensus::stats::ViewDescriptor latency_view = opencensus::stats::ViewDescriptor()
                .set_name("galaxy_server/latency")
                .set_description("The various methods' latencies in milliseconds")
                .set_measure(internal::kLatencyMeasureName)
                .set_aggregation(opencensus::stats::Aggregation::Distribution(
                    opencensus::stats::BucketBoundaries::Explicit(
                        {0, 25, 50, 75, 100, 200, 400, 600, 800, 1000, 2000, 4000,
                        6000})))
                .add_column(internal::MethodKey());
            opencensus::stats::View view(latency_view);
            CHECK(view.IsValid());
            latency_view.RegisterForExport();
        }
    }
}