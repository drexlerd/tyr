#include "tyr/common/json_loader.hpp"
#include "tyr/formalism/planning/parser.hpp"
#include "tyr/planning/algorithms/gbfs_lazy.hpp"
#include "tyr/planning/algorithms/gbfs_lazy/event_handler.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/heuristics/rpg_ff.hpp"
#include "tyr/planning/lifted_task/node.hpp"
#include "tyr/planning/lifted_task/successor_generator.hpp"

#include <benchmark/benchmark.h>
#include <boost/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fp = tyr::formalism::planning;
namespace p = tyr::planning;
namespace json = boost::json;

namespace
{
class ScopedCoutSilencer
{
public:
    ScopedCoutSilencer() : m_null_stream("/dev/null"), m_old_buffer(std::cout.rdbuf(m_null_stream.rdbuf())) {}
    ~ScopedCoutSilencer() { std::cout.rdbuf(m_old_buffer); }

    ScopedCoutSilencer(const ScopedCoutSilencer&) = delete;
    ScopedCoutSilencer& operator=(const ScopedCoutSilencer&) = delete;

private:
    std::ofstream m_null_stream;
    std::streambuf* m_old_buffer;
};

struct BenchmarkCase
{
    std::string name;
    std::filesystem::path domain;
    std::filesystem::path task;
};

std::vector<BenchmarkCase> load_cases()
{
    const auto document = tyr::common::load_json_file(tyr::common::profiling_path("planning/lifted_task/heuristics/rpg.json"));
    const auto& root = document.as_object();
    const auto& domains = root.at("domains").as_object();

    auto result = std::vector<BenchmarkCase>();

    for (const auto& [domain_name_key, domain_value] : domains)
    {
        const auto& domain_object = domain_value.as_object();
        const auto domain_name = std::string(domain_name_key);
        const auto domain = tyr::common::data_path(json::value_to<std::string>(domain_object.at("domain_file")));
        const auto& tasks = domain_object.at("tasks").as_object();

        for (const auto& [task_name_key, task_value] : tasks)
        {
            const auto task_name = std::string(task_name_key);
            const auto run_name = domain_name + "/" + task_name;
            const auto task = tyr::common::data_path(json::value_to<std::string>(task_value));

            result.push_back(BenchmarkCase { run_name, domain, task });
        }
    }

    return result;
}

p::LiftedTaskPtr create_task(const BenchmarkCase& benchmark_case)
{
    return p::LiftedTask::create(fp::Parser(benchmark_case.domain).parse_task(benchmark_case.task));
}

void benchmark_gbfs_lazy_rpg_ff(benchmark::State& state, const BenchmarkCase& benchmark_case)
{
    auto task = create_task(benchmark_case);
    auto execution_context = tyr::ExecutionContext::create(1);
    auto initial_h_value = tyr::float_t(0);
    auto cost = tyr::float_t(0);
    auto length = std::size_t(0);
    auto num_expanded = uint64_t(0);
    auto num_generated = uint64_t(0);
    auto solved = false;

    for (auto _ : state)
    {
        auto successor_generator = p::SuccessorGenerator<p::LiftedTag>(task, execution_context);
        auto heuristic = p::FFRPGHeuristic<p::LiftedTag>::create(task, execution_context);
        auto event_handler = p::gbfs_lazy::DefaultEventHandler<p::LiftedTag>::create(0);

        auto options = p::gbfs_lazy::Options<p::LiftedTag>();
        options.start_node = successor_generator.get_initial_node();
        options.event_handler = event_handler;
        initial_h_value = heuristic->evaluate(options.start_node->get_state());

        auto result = p::SearchResult<p::LiftedTag>();
        {
            const auto silence_cout = ScopedCoutSilencer();
            result = p::gbfs_lazy::find_solution(*task, successor_generator, *heuristic, options);
        }

        num_expanded = event_handler->get_statistics().get_num_expanded();
        num_generated = event_handler->get_statistics().get_num_generated();
        solved = result.status == p::SearchStatus::SOLVED;
        cost = result.plan ? result.plan->get_cost() : tyr::float_t(0);
        length = result.plan ? result.plan->get_length() : std::size_t(0);

        benchmark::DoNotOptimize(static_cast<int>(result.status));
        benchmark::DoNotOptimize(initial_h_value);
        benchmark::DoNotOptimize(cost);
        benchmark::DoNotOptimize(length);
        benchmark::DoNotOptimize(num_expanded);
        benchmark::DoNotOptimize(num_generated);
        benchmark::DoNotOptimize(solved);
    }

    state.counters["initial_h_value"] = benchmark::Counter(static_cast<double>(initial_h_value));
    state.counters["cost"] = benchmark::Counter(static_cast<double>(cost));
    state.counters["length"] = benchmark::Counter(static_cast<double>(length));
    state.counters["num_expanded"] = benchmark::Counter(static_cast<double>(num_expanded));
    state.counters["num_generated"] = benchmark::Counter(static_cast<double>(num_generated));
    state.counters["solved"] = benchmark::Counter(solved ? 1.0 : 0.0);
}
}

int main(int argc, char** argv)
{
    benchmark::Initialize(&argc, argv);

    for (const auto& benchmark_case : load_cases())
    {
        benchmark::RegisterBenchmark((benchmark_case.name + "/gbfs_lazy").c_str(),
                                     [benchmark_case](benchmark::State& state) { benchmark_gbfs_lazy_rpg_ff(state, benchmark_case); });
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
