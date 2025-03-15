#!/usr/bin/env python3
"""
dev.py - Development tool for Blahaj PI

This script provides a streamlined development environment for Blahaj PI.
See README.md for detailed usage instructions and tutorials.
"""

import os
import sys
import argparse
import importlib.util

# Ensure devcmd modules can be imported
script_dir = os.path.dirname(os.path.abspath(__file__))
if script_dir not in sys.path:
    sys.path.insert(0, script_dir)

def import_module(module_name):
    """Import a module from the devcmd package dynamically"""
    try:
        module_path = os.path.join(script_dir, 'devcmd', f'{module_name}.py')
        spec = importlib.util.spec_from_file_location(module_name, module_path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        return module
    except (ImportError, FileNotFoundError) as e:
        print(f"Error: Could not import module '{module_name}' from devcmd package")
        print(f"Please ensure the devcmd directory exists and contains {module_name}.py")
        print(f"Detailed error: {e}")
        sys.exit(1)

def create_parser():
    """Create argument parser based on command definitions"""
    # Import commands from devcmd
    commands_module = import_module('commands')
    COMMANDS = commands_module.COMMANDS
    OPTIONS = commands_module.OPTIONS
    
    parser = argparse.ArgumentParser(description="Blahaj PI Development Tool")
    groups = {}
    
    # Create argument groups and add command flags
    for cmd_name, cmd_info in COMMANDS.items():
        group_name = cmd_info["group"]
        if group_name not in groups:
            groups[group_name] = parser.add_argument_group(group_name)
        
        groups[group_name].add_argument(
            f"--{cmd_name}", 
            action=cmd_info["action"], 
            help=cmd_info["help"]
        )
    
    # Add options to appropriate groups
    for cmd_name, opts in OPTIONS.items():
        group_name = COMMANDS[cmd_name]["group"]
        for opt in opts:
            option_args = []
            option_kwargs = {k: v for k, v in opt.items() if k not in ["name", "flags"]}
            
            # Add main option flag
            option_args.append(f"--{opt['name']}")
            
            # Add any additional flags
            if "flags" in opt:
                option_args.extend(opt["flags"])
            
            groups[group_name].add_argument(*option_args, **option_kwargs)
    
    # Add arguments to pass to Blahaj PI CLI
    parser.add_argument("blahajpi_args", nargs="*", help="Arguments to pass to Blahaj PI CLI")
    
    return parser, COMMANDS, OPTIONS

def determine_command(args, COMMANDS, OPTIONS):
    """Determine which command to run based on args"""
    for cmd_name, cmd_info in COMMANDS.items():
        if getattr(args, cmd_name, False):
            return cmd_info
            
    # Check for implied commands based on options
    for cmd_name, opts in OPTIONS.items():
        for opt in opts:
            if hasattr(args, opt["name"].replace("-", "_")) and getattr(args, opt["name"].replace("-", "_")):
                return COMMANDS[cmd_name]
                
    return None

def main():
    """Main entry point for the dev script"""
    parser, COMMANDS, OPTIONS = create_parser()
    args = parser.parse_args()
    
    # Determine which command to run
    cmd_info = determine_command(args, COMMANDS, OPTIONS)
    
    if cmd_info:
        # Import the appropriate module and call its function
        module = import_module(cmd_info["module"])
        func = getattr(module, cmd_info["function"])
        return func(args)
    else:
        # No command specified, print help
        print("No action specified. Use --help for usage information.")
        return 1

if __name__ == "__main__":
    sys.exit(main())
