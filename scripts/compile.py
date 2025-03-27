import argparse

from compiler.compiler import Compiler

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("directory", type=str)
    args = arg_parser.parse_args()
    compiler = Compiler(args.directory)
    compiler()
