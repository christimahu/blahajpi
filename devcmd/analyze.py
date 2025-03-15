"""
analyze.py - Static analysis and code formatting for Blahaj PI development tool
"""

import os
import sys
import subprocess
from pathlib import Path
import shutil

def run_analysis(args):
    """Run static analysis tools on the codebase"""
    project_root = find_project_root()
    
    if args.format:
        return format_code(args)
    
    print("Running static analysis...")
    
    # Run clang-tidy if available
    clang_tidy_exe = shutil.which("clang-tidy")
    if clang_tidy_exe:
        return run_clang_tidy(args, project_root, clang_tidy_exe)
    
    # Run cppcheck if available
    cppcheck_exe = shutil.which("cppcheck")
    if cppcheck_exe:
        return run_cppcheck(args, project_root, cppcheck_exe)
    
    print("No static analysis tools found.")
    print("Please install clang-tidy and/or cppcheck.")
    return 1

def format_code(args):
    """Format code using clang-format"""
    project_root = find_project_root()
    
    print("Formatting code using clang-format...")
    
    # Check if clang-format is installed
    clang_format_exe = shutil.which("clang-format")
    if not clang_format_exe:
        print("Error: clang-format not found.")
        print("Please install clang-format.")
        return 1
    
    # Find all C++ source files
    source_extensions = [".cpp", ".hpp", ".h", ".cc", ".c"]
    source_files = []
    
    # Directories to search for source files
    search_dirs = [
        project_root / "lib",
        project_root / "cli"
    ]
    
    # Find source files
    for dir_path in search_dirs:
        if not dir_path.exists():
            continue
        
        for ext in source_extensions:
            source_files.extend(dir_path.glob(f"**/*{ext}"))
    
    if not source_files:
        print("No source files found.")
        return 0
    
    # Format each file
    for file_path in source_files:
        print(f"Formatting {file_path.relative_to(project_root)}")
        
        # Check if we should only print reformatting changes
        if hasattr(args, 'dry_run') and args.dry_run:
            cmd = [clang_format_exe, "--dry-run", "--Werror", str(file_path)]
            result = subprocess.run(cmd, capture_output=True)
            if result.returncode != 0:
                print(f"Would reformat {file_path.relative_to(project_root)}")
        else:
            # Format in-place
            cmd = [clang_format_exe, "-i", str(file_path)]
            result = subprocess.run(cmd)
            if result.returncode != 0:
                print(f"Error formatting {file_path.relative_to(project_root)}")
                return result.returncode
    
    print("Code formatting complete.")
    return 0

def run_clang_tidy(args, project_root, clang_tidy_exe):
    """Run clang-tidy on the codebase"""
    print("Running clang-tidy...")
    
    # Create compilation database if it doesn't exist
    build_dir = project_root / "build"
    if not (build_dir / "compile_commands.json").exists():
        print("Compilation database not found.")
        print("Running CMake to generate compilation database...")
        
        # Create build directory if it doesn't exist
        build_dir.mkdir(exist_ok=True)
        
        # Configure with CMake
        cmd = [
            "cmake", 
            "-B", str(build_dir),
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
        ]
        
        result = subprocess.run(cmd, cwd=project_root)
        if result.returncode != 0:
            print("Error: Failed to generate compilation database.")
            return result.returncode
    
    # Directories to analyze
    source_dirs = [
        project_root / "lib" / "src",
        project_root / "lib" / "include",
        project_root / "cli" / "src",
        project_root / "cli" / "include"
    ]
    
    # Clang-tidy checks
    checks = (
        "bugprone-*,"
        "cppcoreguidelines-*,"
        "performance-*,"
        "portability-*,"
        "readability-*,"
        "-cppcoreguidelines-pro-bounds-pointer-arithmetic,"
        "-cppcoreguidelines-avoid-magic-numbers,"
        "-readability-magic-numbers"
    )
    
    # Run clang-tidy on each directory
    for dir_path in source_dirs:
        if not dir_path.exists():
            continue
        
        print(f"Analyzing {dir_path.relative_to(project_root)}...")
        
        # Find source files
        source_extensions = [".cpp", ".hpp", ".h", ".cc", ".c"]
        source_files = []
        
        for ext in source_extensions:
            source_files.extend([str(f) for f in dir_path.glob(f"**/*{ext}")])
        
        if not source_files:
            continue
        
        # Run clang-tidy
        cmd = [
            clang_tidy_exe,
            f"-checks={checks}",
            "-p", str(build_dir)
        ]
        cmd.extend(source_files)
        
        result = subprocess.run(cmd)
        if result.returncode != 0:
            print(f"Clang-tidy found issues in {dir_path.relative_to(project_root)}.")
            if not args.verbose:
                print("Run with --verbose for detailed output.")
            # Continue with other directories even if issues found
    
    print("Clang-tidy analysis complete.")
    return 0  # Return success even if issues found

def run_cppcheck(args, project_root, cppcheck_exe):
    """Run cppcheck on the codebase"""
    print("Running cppcheck...")
    
    # Directories to analyze
    source_dirs = [
        project_root / "lib" / "src",
        project_root / "lib" / "include",
        project_root / "cli" / "src",
        project_root / "cli" / "include"
    ]
    
    # Combine source directories that exist
    existing_dirs = [str(d) for d in source_dirs if d.exists()]
    
    if not existing_dirs:
        print("No source directories found.")
        return 0
    
    # Prepare cppcheck command
    cmd = [
        cppcheck_exe,
        "--enable=all",
        "--suppress=missingIncludeSystem",
        "--inline-suppr",
        "--template={file}:{line}: {severity}: {message}"
    ]
    
    # Add language standard
    cmd.append("--std=c++23")
    
    # Add verbose output if requested
    if args.verbose:
        cmd.append("--verbose")
    
    # Add source directories
    cmd.extend(existing_dirs)
    
    # Run cppcheck
    result = subprocess.run(cmd)
    if result.returncode != 0:
        print("Cppcheck found issues in the codebase.")
        if not args.verbose:
            print("Run with --verbose for detailed output.")
    
    print("Cppcheck analysis complete.")
    return 0  # Return success even if issues found

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
