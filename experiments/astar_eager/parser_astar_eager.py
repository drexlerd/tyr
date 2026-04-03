#! /usr/bin/env python

from lab.parser import Parser
from lab import tools
from lab.reports import Attribute, geometric_mean, arithmetic_mean

import re


class AStarEagerParser(Parser):
    """
    [Search] Number of expanded states at last snapshot: 17
    [Search] Number of generated states at last snapshot: 32
    [Search] Number of pruned states at last snapshot: 0
    """
    def __init__(self):
        super().__init__()
        self.add_pattern("num_expanded_until_last_jump", r"\[Search\] Number of expanded states at last snapshot: (\d+)", type=int)
        self.add_pattern("num_generated_until_last_jump", r"\[Search\] Number of generated states at last snapshot: (\d+)", type=int)

    @staticmethod
    def get_attributes():
        return [
            "num_expanded_until_last_jump",
            "num_generated_until_last_jump",
            
        ]
