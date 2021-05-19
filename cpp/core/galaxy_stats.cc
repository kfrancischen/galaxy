#include "cpp/core/galaxy_stats.h"
#include "cpp/internal/galaxy_stats_internal.h"
#include "glog/logging.h"

namespace galaxy {
    namespace stats {
        void RegisterServerViews() {
            // Register server latency view.
            internal::LatencyMsMeasure();
            const opencensus::stats::ViewDescriptor server_latency_view = opencensus::stats::ViewDescriptor()
                .set_name("galaxy_server/latency")
                .set_description("The various methods' latencies in milliseconds")
                .set_measure(internal::kLatencyMeasureName)
                .set_aggregation(opencensus::stats::Aggregation::Distribution(
                    opencensus::stats::BucketBoundaries::Explicit(
                        {0, 25, 50, 75, 100, 200, 400, 800, 1000, 4000})))
                .add_column(internal::MethodKey());
            opencensus::stats::View server_view(server_latency_view);
            CHECK(server_view.IsValid()) << "Failed to create server latency view.";
            server_latency_view.RegisterForExport();
        }

        void RegisterClientViews() {
            // Register client latency view.
            const opencensus::stats::ViewDescriptor client_latency_view = opencensus::stats::ViewDescriptor()
                .set_name("galaxy/client/roundtrip_latency/cumulative")
                .set_measure(internal::kRpcClientRoundtripLatencyMeasureName)
                .set_aggregation(opencensus::stats::Aggregation::Distribution(
                    opencensus::stats::BucketBoundaries::Explicit(
                        {0, 25, 50, 75, 100, 200, 400, 800, 1000, 4000})))
                .add_column(internal::ClientMethodTagKey())
                .add_column(internal::ClientStatusTagKey())
                .add_column(internal::MethodKey());
            opencensus::stats::View client_view(client_latency_view);
            CHECK(client_view.IsValid()) << "Failed to create client latency view.";
            client_latency_view.RegisterForExport();
        }
    }
}