import argparse

from compiler.compiler import Compiler

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("directory", type=str, help="Temporary directory")
    arg_parser.add_argument("target", type=str, help="Target executable path")
    arg_parser.add_argument("--uncompressed", "-u", action="store_true", help="Disable compression")
    args = arg_parser.parse_args()
    compiler = Compiler(args.directory, args.target, not args.uncompressed)
    compiler()
