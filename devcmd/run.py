"""
run.py - Command execution functionality for Blahaj PI development tool
"""

import os
import sys
import subprocess
from pathlib import Path

def run_cli(args):
    """Run the Blahaj PI CLI with passed arguments"""
    project_root = find_project_root()
    
    # Find executable
    executable_path = find_executable(project_root)
    if not executable_path:
        # Try to build if the executable is not found
        print("Executable not found. Building project first...")
        build_module = __import__("devcmd.build", fromlist=["run_build"]).run_build
        if build_module(args) != 0:
            return 1
        
        # Try to find executable again
        executable_path = find_executable(project_root)
        if not executable_path:
            print("Error: Failed to find or build executable")
            return 1
    
    # Get configuration path
    config_path = None
    if args.config:
        config_path = get_config_path(project_root, args.config)
        if not config_path:
            # Try to use direct path
            config_path = args.config
    
    # Prepare command
    cmd = [str(executable_path)]
    
    # Add configuration if specified
    if config_path:
        cmd.extend(["--config", str(config_path)])
    
    # Add user arguments
    cmd.extend(args.blahajpi_args)
    
    # Print command for debugging
    print(f"Running: {' '.join(cmd)}")
    
    # Execute command
    try:
        result = subprocess.run(cmd)
        return result.returncode
    except Exception as e:
        print(f"Error running command: {e}")
        return 1

def get_config_path(project_root, config_name):
    """Get the path to a configuration file"""
    if not config_name:
        return None
    
    config_dir = project_root / "configs"
    
    # Map config names to file paths
    config_files = {
        "default": config_dir / "default.conf",
        "fast": config_dir / "fast_model.conf",
        "accurate": config_dir / "accurate_model.conf",
        "production": config_dir / "production_model.conf"
    }
    
    # Check if config exists
    if config_name in config_files:
        config_path = config_files[config_name]
    else:
        config_path = Path(config_name)
    
    if not config_path.exists():
        print(f"Warning: Configuration file not found: {config_path}")
        return None
    
    return config_path

def find_executable(project_root):
    """Find the Blahaj PI CLI executable"""
    import platform
    
    build_path = project_root / "build"
    if not build_path.exists():
        return None
    
    exe_name = "blahajpi" if platform.system() != "Windows" else "blahajpi.exe"
    
    # Try to find the executable
    potential_paths = []
    if platform.system() == "Windows":
        # Windows: check various possible locations
        for config in ["Debug", "Release", "RelWithDebInfo", "MinSizeRel"]:
            potential_paths.append(build_path / config / exe_name)
        potential_paths.append(build_path / "bin" / exe_name)
    else:
        # Unix: typically in bin directory
        potential_paths.append(build_path / "bin" / exe_name)
    
    # Return the first executable that exists
    for path in potential_paths:
        if path.exists():
            return path
    
    return None

def find_project_root():
    """Find the project root directory"""
    current_dir = Path.cwd()
    while current_dir != current_dir.parent:
        if (current_dir / "CMakeLists.txt").exists():
            return current_dir
        current_dir = current_dir.parent
    
    print("Error: Could not find project root directory.")
    print("Please run this script from within the Blahaj PI project directory.")
    sys.exit(1)
