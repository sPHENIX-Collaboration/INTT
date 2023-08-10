import argparse
import datetime
import logging
import os
import pathlib
import pprint
import socket
import subprocess
import sys
import time

from argparse import (OPTIONAL, SUPPRESS, ZERO_OR_MORE,
                      ArgumentDefaultsHelpFormatter, ArgumentParser,
                      RawDescriptionHelpFormatter, RawTextHelpFormatter)


class MyHelpFormatter(
    RawTextHelpFormatter, RawDescriptionHelpFormatter, ArgumentDefaultsHelpFormatter
):
    def _format_action(self, action: argparse.Action) -> str:
        return super()._format_action(action) + "\n"

    def _get_help_string(self, action):
        help = action.help
        if action.required:
            help += " (required)"

        if "%(default)" not in action.help:
            if action.default is not SUPPRESS:
                defaulting_nargs = [OPTIONAL, ZERO_OR_MORE]
                if action.option_strings or action.nargs in defaulting_nargs:
                    if action.default is not None and not action.const:
                        help += " (default: %(default)s)"
        return help

if __name__ == "__main__" :
    print( "test" )
