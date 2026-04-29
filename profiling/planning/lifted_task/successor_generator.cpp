#include "tyr/common/json_loader.hpp"
#include "tyr/planning/lifted_task/successor_generator.hpp"

#include "tyr/formalism/planning/parser.hpp"
#include "tyr/formalism/planning/views.hpp"
#include "tyr/planning/lifted_task.hpp"
#include "tyr/planning/lifted_task/node.hpp"

#include <benchmark/benchmark.h>
#include <boost/json.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace fp = tyr::formalism::planning;
namespace p = tyr::planning;
namespace json = boost::json;

namespace
{
struct BenchmarkCase
{
    std::string name;
    std::filesystem::path domain;
    std::filesystem::path task;
};

std::vector<BenchmarkCase> load_cases()
{
    const auto document = tyr::common::load_json_file(tyr::common::profiling_path("planning/lifted_task/successor_generator.json"));
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

void benchmark_initial_successors(benchmark::State& state, const BenchmarkCase& benchmark_case)
{
    auto task = create_task(benchmark_case);
    auto successor_generator = p::SuccessorGenerator<p::LiftedTag>(task, tyr::ExecutionContext::create(1));
    const auto initial_node = successor_generator.get_initial_node();
    auto successors = std::vector<p::LabeledNode<p::LiftedTag>>();

    for (auto _ : state)
    {
        successor_generator.get_labeled_successor_nodes(initial_node, successors);
        benchmark::DoNotOptimize(successors.data());
        benchmark::DoNotOptimize(successors.size());
    }

    state.counters["num_successors"] = benchmark::Counter(static_cast<double>(successors.size()));
}

void benchmark_interned_action_bindings(benchmark::State& state, const BenchmarkCase& benchmark_case)
{
    auto task = create_task(benchmark_case);
    auto successor_generator = p::SuccessorGenerator<p::LiftedTag>(task, tyr::ExecutionContext::create(1));
    const auto initial_node = successor_generator.get_initial_node();
    auto bindings = std::vector<fp::ActionBindingView>();

    for (auto _ : state)
    {
        successor_generator.get_applicable_action_bindings(initial_node, bindings);
        benchmark::DoNotOptimize(bindings.data());
        benchmark::DoNotOptimize(bindings.size());
    }

    state.counters["num_successors"] = benchmark::Counter(static_cast<double>(bindings.size()));
}

void benchmark_action_binding_iterator(benchmark::State& state, const BenchmarkCase& benchmark_case)
{
    auto task = create_task(benchmark_case);
    auto successor_generator = p::SuccessorGenerator<p::LiftedTag>(task, tyr::ExecutionContext::create(1));
    const auto initial_node = successor_generator.get_initial_node();
    auto num_bindings = size_t(0);

    for (auto _ : state)
    {
        num_bindings = 0;
        successor_generator.for_each_applicable_action_binding(initial_node,
                                                               [&](const auto& binding)
                                                               {
                                                                   benchmark::DoNotOptimize(&binding);
                                                                   benchmark::DoNotOptimize(binding.objects.data());
                                                                   benchmark::DoNotOptimize(binding.objects.size());
                                                                   ++num_bindings;
                                                               });
        benchmark::DoNotOptimize(num_bindings);
    }

    state.counters["num_successors"] = benchmark::Counter(static_cast<double>(num_bindings));
}
}

int main(int argc, char** argv)
{
    benchmark::Initialize(&argc, argv);

    for (const auto& benchmark_case : load_cases())
    {
        benchmark::RegisterBenchmark((benchmark_case.name + "/labeled_successors").c_str(),
                                     [benchmark_case](benchmark::State& state) { benchmark_initial_successors(state, benchmark_case); });
        benchmark::RegisterBenchmark((benchmark_case.name + "/interned_bindings").c_str(),
                                     [benchmark_case](benchmark::State& state) { benchmark_interned_action_bindings(state, benchmark_case); });
        benchmark::RegisterBenchmark((benchmark_case.name + "/binding_iterator").c_str(),
                                     [benchmark_case](benchmark::State& state) { benchmark_action_binding_iterator(state, benchmark_case); });
    }

    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
}
