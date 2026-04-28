from pytyr.formalism.planning import ParserOptions, Parser

from pathlib import Path

ROOT_DIR = (Path(__file__).parent.parent.parent.parent.parent).absolute()


def test_pddl_parser():
    """ Test parsing and translation of a PDDL domain and problem file.
    """
    domain_filepath = str(ROOT_DIR / "data" / "tests" / "classical" / "gripper" / "domain.pddl")
    problem_filepath = str(ROOT_DIR / "data" / "tests" / "classical" / "gripper" / "test-1.pddl")
    parser_options = ParserOptions()
    parser = Parser(domain_filepath, parser_options)

    domain = parser.get_domain()
    task = parser.parse_task(problem_filepath, parser_options)

    assert(domain.get_domain() == task.get_domain().get_domain())
