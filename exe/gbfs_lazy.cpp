/*
 * Copyright (C) 2025-2026 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <argparse/argparse.hpp>
#include <chrono>
#include <fstream>
#include <queue>
#include <tyr/tyr.hpp>

using namespace tyr;

int main(int argc, char** argv)
{
    auto program = argparse::ArgumentParser("Lazy GBFS search.");
    program.add_argument("-D", "--domain-filepath").required().help("The path to the PDDL domain file.");
    program.add_argument("-P", "--problem-filepath").required().help("The path to the PDDL problem file.");
    program.add_argument("-O", "--plan-filepath").default_value(std::string("plan.out")).help("The path to the output plan file.");
    program.add_argument("-N", "--num-worker-threads").default_value(size_t(1)).scan<'u', size_t>().help("The number of worker threads.");
    program.add_argument("-R", "--random-seed").default_value(uint64_t(0)).scan<'u', uint64_t>().help("The random seed.");
    program.add_argument("-S", "--shuffle-labeled-succ-nodes").default_value(false).implicit_value(true).help("Toggle shuffling the labeled successor nodes.");
    program.add_argument("-G", "--instantiate-ground-task")
        .default_value(false)
        .implicit_value(true)
        .help("Enable instantiating the ground task before search.");
    program.add_argument("-H", "--heuristic-type").default_value("blind").choices("blind", "goal_count", "rpg_max", "rpg_add", "rpg_ff");
    program.add_argument("-V", "--verbosity")
        .default_value(size_t(0))
        .scan<'u', size_t>()
        .help("The verbosity level. Defaults to minimal amount of debug output.");

    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err)
    {
        std::cerr << err.what() << "\n";
        std::cerr << program;
        std::exit(1);
    }

    auto total_time = std::chrono::nanoseconds { 0 };
    {
        auto stop_watch = StopwatchScope(total_time);

        auto domain_filepath = program.get<std::string>("--domain-filepath");
        auto problem_filepath = program.get<std::string>("--problem-filepath");
        auto plan_filepath = program.get<std::string>("--plan-filepath");
        auto num_worker_threads = program.get<std::size_t>("--num-worker-threads");
        auto random_seed = program.get<uint64_t>("--random-seed");
        auto shuffle_labeled_succ_nodes = program.get<bool>("--shuffle-labeled-succ-nodes");
        auto instantiate_ground_task = program.get<bool>("--instantiate-ground-task");
        auto heuristic_type = program.get<std::string>("--heuristic-type");
        auto verbosity = program.get<size_t>("--verbosity");

        std::cout << "[INPUT] Num worker threads: " << num_worker_threads << std::endl;
        std::cout << "[INPUT] Random seed: " << random_seed << std::endl;
        std::cout << "[INPUT] Shuffle labeled successor nodes: " << shuffle_labeled_succ_nodes << std::endl;

        auto parser_options = loki::ParserOptions();
        // parser_options.strict = true;
        auto parser = formalism::planning::Parser(domain_filepath, parser_options);
        auto domain = parser.get_domain();

        auto lifted_task = planning::LiftedTask::create(parser.parse_task(problem_filepath));

        if (verbosity > 0)
            std::cout << domain << std::endl;

        if (verbosity > 0)
            std::cout << *lifted_task << std::endl;

        auto execution_context = ExecutionContext::create(num_worker_threads);

        if (!instantiate_ground_task)
        {
            auto successor_generator = planning::SuccessorGenerator<planning::LiftedTag>(lifted_task, execution_context);

            auto options = planning::gbfs_lazy::Options<planning::LiftedTag>();
            options.start_node = successor_generator.get_initial_node();
            options.event_handler = planning::gbfs_lazy::DefaultEventHandler<planning::LiftedTag>::create(verbosity);
            options.random_seed = random_seed;
            options.shuffle_labeled_succ_nodes = shuffle_labeled_succ_nodes;

            auto heuristic = std::shared_ptr<planning::Heuristic<planning::LiftedTag>> { nullptr };
            if (heuristic_type == "blind")
                heuristic = planning::BlindHeuristic<planning::LiftedTag>::create();
            else if (heuristic_type == "goal_count")
                heuristic = planning::GoalCountHeuristic<planning::LiftedTag>::create(lifted_task);
            else if (heuristic_type == "rpg_add")
                heuristic = planning::AddRPGHeuristic<planning::LiftedTag>::create(lifted_task, execution_context);
            else if (heuristic_type == "rpg_max")
                heuristic = planning::MaxRPGHeuristic<planning::LiftedTag>::create(lifted_task, execution_context);
            else if (heuristic_type == "rpg_ff")
                heuristic = planning::FFRPGHeuristic<planning::LiftedTag>::create(lifted_task, execution_context);
            else
                throw std::invalid_argument("The heuristic is not implemented.");

            auto result = planning::gbfs_lazy::find_solution(*lifted_task, successor_generator, *heuristic, options);

            if (result.status == planning::SearchStatus::SOLVED)
            {
                std::ofstream plan_file;
                plan_file.open(plan_filepath);
                if (!plan_file.is_open())
                {
                    std::cerr << "Error opening file!" << std::endl;
                    return 1;
                }
                plan_file << result.plan.value();
                plan_file.close();
            }

            successor_generator.print_summary(1);
            successor_generator.get_state_repository()->get_axiom_evaluator()->print_summary(1);
            heuristic->print_summary(1);

            std::cout << "[Total] Number of fluent atoms: " << lifted_task->get_repository()->size<formalism::planning::GroundAtom<formalism::FluentTag>>()
                      << std::endl;
            std::cout << "[Total] Number of derived atoms: " << lifted_task->get_repository()->size<formalism::planning::GroundAtom<formalism::DerivedTag>>()
                      << std::endl;
            std::cout << "[Total] Number of fluent fterms: "
                      << lifted_task->get_repository()->size<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>() << std::endl;
            std::cout << "[Total] States memory usage: " << successor_generator.get_state_repository()->memory_usage() << " bytes" << std::endl;
        }
        else
        {
            auto ground_task = lifted_task->instantiate_ground_task(*execution_context);

            std::cout << ground_task->get_formalism_task() << std::endl;

            auto successor_generator = planning::SuccessorGenerator<planning::GroundTag>(ground_task, execution_context);

            auto options = planning::gbfs_lazy::Options<planning::GroundTag>();
            options.start_node = successor_generator.get_initial_node();
            options.event_handler = planning::gbfs_lazy::DefaultEventHandler<planning::GroundTag>::create(verbosity);
            options.random_seed = random_seed;
            options.shuffle_labeled_succ_nodes = shuffle_labeled_succ_nodes;

            auto heuristic = std::shared_ptr<planning::Heuristic<planning::GroundTag>> { nullptr };
            if (heuristic_type == "blind")
                heuristic = planning::BlindHeuristic<planning::GroundTag>::create();
            else if (heuristic_type == "goal_count")
                heuristic = planning::GoalCountHeuristic<planning::GroundTag>::create(ground_task);
            else
                throw std::invalid_argument("The heuristic is not implemented.");

            auto result = planning::gbfs_lazy::find_solution(*ground_task, successor_generator, *heuristic, options);

            if (result.status == planning::SearchStatus::SOLVED)
            {
                std::ofstream plan_file;
                plan_file.open(plan_filepath);
                if (!plan_file.is_open())
                {
                    std::cerr << "Error opening file!" << std::endl;
                    return 1;
                }
                plan_file << result.plan.value();
                plan_file.close();
            }

            std::cout << "[Total] Number of fluent atoms: " << ground_task->get_repository()->size<formalism::planning::GroundAtom<formalism::FluentTag>>()
                      << std::endl;
            std::cout << "[Total] Number of derived atoms: " << ground_task->get_repository()->size<formalism::planning::GroundAtom<formalism::DerivedTag>>()
                      << std::endl;
            std::cout << "[Total] Number of fluent fterms: "
                      << ground_task->get_repository()->size<formalism::planning::GroundFunctionTerm<formalism::FluentTag>>() << std::endl;
            std::cout << "[Total] States memory usage: " << successor_generator.get_state_repository()->memory_usage() << " bytes" << std::endl;
        }
    }

    std::cout << "[Total] Peak memory usage: " << get_peak_memory_usage_in_bytes() << " bytes" << std::endl;
    std::cout << "[Total] Total time: " << to_ms(total_time) << " ms (" << to_ns(total_time) << " ns)" << std::endl;

    return 0;
}