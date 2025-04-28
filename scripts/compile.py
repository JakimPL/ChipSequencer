import argparse

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("platform", type=str, help="Platform: dos or linux")
    arg_parser.add_argument("directory", type=str, help="Temporary directory")
    arg_parser.add_argument("target", type=str, help="Target executable path")
    arg_parser.add_argument("--uncompressed", "-u", action="store_true", help="Disable compression")
    args = arg_parser.parse_args()
    if args.platform == "dos":
        from compiler.platforms.dos import DOSCompiler

        compiler = DOSCompiler(args.directory, args.target, not args.uncompressed)
    elif args.platform == "linux":
        from compiler.platforms.linux import LinuxCompiler

        compiler = LinuxCompiler(args.directory, args.target, not args.uncompressed)
    else:
        raise ValueError(f"Unknown platform: {args.platform}, should be 'dos' or 'linux'")

    compiler()
