import argparse

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("base", type=str, help="Base directory of the application")
    arg_parser.add_argument("platform", type=str, help="Platform: only 'windows' and 'linux' are supported")
    arg_parser.add_argument("directory", type=str, help="Temporary directory")
    arg_parser.add_argument("target", type=str, help="Target executable path")
    arg_parser.add_argument("--uncompressed", "-u", action="store_true", help="Disable compression")
    arg_parser.add_argument("--debug", "-d", action="store_true", help="Enable debug mode (disables compression)")
    args = arg_parser.parse_args()

    if args.platform == "linux":
        from compiler.platforms.linux import LinuxCompiler

        compiler = LinuxCompiler(args.base, args.directory, args.target, not args.uncompressed, args.debug)
    elif args.platform == "windows":
        from compiler.platforms.windows import WindowsCompiler

        compiler = WindowsCompiler(args.base, args.directory, args.target, not args.uncompressed, args.debug)
    else:
        raise ValueError(f"Unknown platform: {args.platform}, only 'windows' and 'linux' are supported.")

    compiler()
