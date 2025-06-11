import argparse

from compiler.size_checker import SizeChecker

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("--platform", "-p", type=str, default="linux", help="Platform: only 'linux' is supported")
    args = arg_parser.parse_args()

    if args.platform != "linux":
        raise ValueError(f"Unknown platform: {args.platform}, only 'linux' is supported.")

    size_checker = SizeChecker(args.platform)
    size_checker()
