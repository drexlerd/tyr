
import pytyr.formalism.planning as tfp
import pytyr.planning as tp

from pathlib import Path

ROOT_DIR = (Path(__file__).parent.parent.parent).absolute()

def main():
    domain_filepath = ROOT_DIR / "data" / "gripper" / "domain.pddl"
    task_filepath = ROOT_DIR / "data" / "gripper" / "p-2-0.pddl"

    parser_options = tp.ParserOptions()
    parser = tp.Parser(domain_filepath, parser_options)
    lifted_task = parser.parse_task(task_filepath, parser_options)

    lifted_state_repository = tp.LiftedStateRepository(lifted_task)
    lifted_init_state = lifted_state_repository.get_initial_state()
    print(lifted_init_state)

    # for x in lifted_init_state.static_atoms():
    #     print(x)

    lifted_succ_gen = tp.LiftedSuccessorGenerator(lifted_task)
    lifted_init_node = lifted_succ_gen.get_initial_node()
    print(lifted_init_node)
    lifted_labeled_succ_nodes = lifted_succ_gen.get_labeled_successor_nodes(lifted_init_node)

    ground_task = lifted_task.instantiate_ground_task()

    ground_state_repository = tp.GroundStateRepository(ground_task)
    ground_init_state = ground_state_repository.get_initial_state()
    print(ground_init_state)

    ground_succ_gen = tp.GroundSuccessorGenerator(ground_task)
    ground_init_node = ground_succ_gen.get_initial_node()
    print(ground_init_node)
    ground_labeled_succ_nodes = ground_succ_gen.get_labeled_successor_nodes(ground_init_node)
    


if __name__ == "__main__":
    main()
