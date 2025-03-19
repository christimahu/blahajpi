"""
common.py - Shared utilities for Blahaj PI development tool
"""

import os
import sys
import platform
from pathlib import Path
import shutil
import re

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

def find_executable(project_root):
    """Find the Blahaj PI CLI executable"""
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

def get_cmake_generator():
    """Get the appropriate CMake generator for the current platform"""
    if platform.system() == "Windows":
        # Try to detect Visual Studio
        try:
            vswhere_path = r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
            if os.path.exists(vswhere_path):
                import subprocess
                result = subprocess.run(
                    [vswhere_path, "-latest", "-property", "installationVersion"],
                    capture_output=True, text=True
                )
                version = result.stdout.strip().split(".")[0]
                if version == "17":
                    return "Visual Studio 17 2022"
                elif version == "16":
                    return "Visual Studio 16 2019"
        except Exception:
            pass
        return "Visual Studio 17 2022"  # Default
    else:
        # Unix-like systems: Ninja or Makefiles
        try:
            import subprocess
            subprocess.run(["ninja", "--version"], capture_output=True)
            return "Ninja"
        except FileNotFoundError:
            return "Unix Makefiles"

def log_info(message):
    """Log an informational message"""
    print(f"INFO: {message}")

def log_warning(message):
    """Log a warning message"""
    print(f"WARNING: {message}")

def log_error(message):
    """Log an error message"""
    print(f"ERROR: {message}")

def log_success(message):
    """Log a success message"""
    print(f"SUCCESS: {message}")

def colorize_text(text, color_code):
    """Colorize text for terminal output if supported"""
    # Check if running in a terminal that supports colors
    if sys.stdout.isatty():
        return f"\033[{color_code}m{text}\033[0m"
    return text

def get_version_from_cmake():
    """Extract version from CMakeLists.txt"""
    project_root = find_project_root()
    cmake_file = project_root / "CMakeLists.txt"
    
    if not cmake_file.exists():
        return "Unknown"
    
    with open(cmake_file, 'r') as f:
        content = f.read()
    
    # Try to find project version
    version_match = re.search(r'project\s*\(\s*\w+\s+VERSION\s+([0-9.]+)', content)
    if version_match:
        return version_match.group(1)
    
    return "Unknown"

def check_requirements():
    """Check if required tools are available"""
    requirements = {
        "cmake": "CMake is required for building the project",
        "python3": "Python 3.6+ is required for build scripts",
        "g++": "C++ compiler is required for building the project",
    }
    
    missing = []
    for req, message in requirements.items():
        if not shutil.which(req):
            missing.append(f"{req}: {message}")
    
    if missing:
        print("The following required tools are missing:")
        for item in missing:
            print(f"  - {item}")
        return False
    
    return True

def print_header(title):
    """Print a formatted header"""
    width = max(len(title) + 4, 40)
    print("="*width)
    print(f"  {title}")
    print("="*width)

def get_build_config(args):
    """Get build configuration based on args"""
    config = {
        "build_type": "Debug" if args.debug else "Release",
        "verbose": args.verbose,
        "jobs": args.jobs if args.jobs > 0 else None,
    }
    return config

def flatten_dict(d, parent_key='', sep='_'):
    """Flatten a nested dictionary"""
    items = []
    for k, v in d.items():
        new_key = f"{parent_key}{sep}{k}" if parent_key else k
        if isinstance(v, dict):
            items.extend(flatten_dict(v, new_key, sep=sep).items())
        else:
            items.append((new_key, v))
    return dict(items)
