#!/usr/bin/env python3
"""
dev.py - Development tool for Blahaj PI

This script serves as a comprehensive development tool for the Blahaj PI project,
demonstrating the build process and CLI usage.

Quick usage examples:
    ./dev.py --build                # Build the project
    ./dev.py --test                 # Run tests
    ./dev.py --run -- analyze --file data/examples/twitter_example.csv   # Analyze a file
    ./dev.py --demo                 # Run complete functionality demo
    ./dev.py --docs                 # Generate documentation
"""

import os
import sys
import shutil
import platform
import argparse
import subprocess
from pathlib import Path

def parse_args():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description="Blahaj PI Development Tool")
    
    # Main operation mode flags
    action_group = parser.add_argument_group("Actions")
    action_group.add_argument("--build", action="store_true", help="Build the project")
    action_group.add_argument("--test", action="store_true", help="Run tests")
    action_group.add_argument("--run", action="store_true", help="Run Blahaj PI CLI")
    action_group.add_argument("--demo", action="store_true", help="Run a complete demonstration of functionality")
    action_group.add_argument("--clean", action="store_true", help="Clean build directory")
    action_group.add_argument("--docs", action="store_true", help="Generate documentation")
    
    # Build options
    build_group = parser.add_argument_group("Build Options")
    build_group.add_argument("--debug", action="store_true", help="Build in debug mode")
    build_group.add_argument("--verbose", action="store_true", help="Enable verbose output")
    build_group.add_argument("--jobs", "-j", type=int, default=0, 
                            help="Number of parallel jobs (0 for auto)")
    
    # Test options
    test_group = parser.add_argument_group("Test Options")
    test_group.add_argument("--unit", action="store_true", help="Run only unit tests")
    test_group.add_argument("--integration", action="store_true", help="Run only integration tests")
    test_group.add_argument("--e2e", action="store_true", help="Run only end-to-end tests")
    
    # Run options
    run_group = parser.add_argument_group("Run Options")
    run_group.add_argument("--config", help="Configuration to use (default, fast, accurate, production)")
    
    # Documentation options
    docs_group = parser.add_argument_group("Documentation Options")
    docs_group.add_argument("--open-docs", action="store_true", help="Open documentation in browser after generation")
    
    # Example usage block
    parser.epilog = """
Example CLI Usage:
  # Analyze a file for potentially harmful content
  ./dev.py --run -- analyze --file data/examples/twitter_example.csv
  
  # Train a custom model
  ./dev.py --run -- train --dataset data/examples/twitter_example.csv --output data/models/my_model
  
  # Generate a word cloud visualization of harmful content
  ./dev.py --run -- visualize --input data/examples/twitter_example.csv --output data/visualization.txt
  
  # Show help for a specific command
  ./dev.py --run -- help analyze
  
  # Run a complete demo of core functionality
  ./dev.py --demo
    """
    
    # Add a separator to distinguish Blahaj PI arguments from script arguments
    parser.add_argument("blahajpi_args", nargs="*", 
                       help="Arguments to pass to Blahaj PI CLI when using --run (use -- before CLI args)")
    
    args = parser.parse_args()
    
    # If no action specified, show help
    if not (args.build or args.test or args.run or args.clean or args.docs or args.demo):
        parser.print_help()
        sys.exit(1)
    
    return args

def find_project_root():
    """Find the project root directory"""
    # Start with the directory containing this script
    current_dir = Path(__file__).resolve().parent
    
    # Go up until we find a directory containing CMakeLists.txt
    while current_dir != current_dir.parent:
        if (current_dir / "CMakeLists.txt").exists():
            return current_dir
        current_dir = current_dir.parent
    
    # If we can't find the project root, use the current directory
    print("Warning: Could not find project root directory. Using current directory.")
    return Path.cwd()

def get_build_type(debug_mode):
    """Get the CMake build type based on debug flag"""
    return "Debug" if debug_mode else "Release"

def get_generator():
    """Get the appropriate CMake generator for the current platform"""
    if platform.system() == "Windows":
        # Try to detect Visual Studio
        try:
            # Check if Visual Studio is installed by running vswhere
            vswhere_path = r"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
            if os.path.exists(vswhere_path):
                result = subprocess.run(
                    [vswhere_path, "-latest", "-property", "installationVersion"],
                    capture_output=True, text=True
                )
                version = result.stdout.strip().split(".")[0]
                if version == "17":
                    return "Visual Studio 17 2022"
                elif version == "16":
                    return "Visual Studio 16 2019"
        except Exception as e:
            print(f"Warning: Error detecting Visual Studio: {e}")
        
        # Default to latest Visual Studio
        return "Visual Studio 17 2022"
    else:
        # On Unix-like systems, use Ninja if available, otherwise use Makefiles
        try:
            subprocess.run(["ninja", "--version"], capture_output=True)
            return "Ninja"
        except FileNotFoundError:
            return "Unix Makefiles"

def clean_build_directory(project_root):
    """Clean the build directory"""
    build_dir = project_root / "build"
    
    if build_dir.exists():
        print(f"Cleaning build directory: {build_dir}")
        shutil.rmtree(build_dir)
        print("Build directory cleaned")
        return True
    else:
        print("No build directory to clean")
        return False

def build_project(args, project_root):
    """Build the project using CMake"""
    build_dir = project_root / "build"
    
    # Clean build directory if requested
    if args.clean and build_dir.exists():
        clean_build_directory(project_root)
    
    # Create build directory if it doesn't exist
    build_dir.mkdir(exist_ok=True)
    
    # Determine build type and generator
    build_type = get_build_type(args.debug)
    generator = get_generator()
    
    print(f"Configuring project with CMake...")
    print(f"  Build type: {build_type}")
    print(f"  Generator: {generator}")
    
    # Configure with CMake
    cmake_configure_cmd = [
        "cmake",
        "-S", str(project_root),
        "-B", str(build_dir),
        f"-DCMAKE_BUILD_TYPE={build_type}",
        "-G", generator
    ]
    
    if args.verbose:
        cmake_configure_cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")
    
    configure_result = subprocess.run(cmake_configure_cmd)
    if configure_result.returncode != 0:
        print("Error: CMake configuration failed")
        return False
    
    # Build with CMake
    print("\nBuilding project...")
    
    cmake_build_cmd = [
        "cmake",
        "--build", str(build_dir),
        "--config", build_type
    ]
    
    if args.jobs > 0:
        cmake_build_cmd.extend(["-j", str(args.jobs)])
    
    build_result = subprocess.run(cmake_build_cmd)
    if build_result.returncode != 0:
        print("Error: Build failed")
        return False
    
    print("\nBuild completed successfully!")
    
    # Print path to executable
    cli_exe = find_executable(project_root)
    if cli_exe:
        print(f"\nBlahaj PI CLI executable: {cli_exe}")
        print("\nQuick usage examples:")
        print(f"  {cli_exe} analyze --file data/examples/twitter_example.csv  # Analyze a file")
        print(f"  {cli_exe} train --dataset data/examples/twitter_example.csv   # Train a custom model")
        print(f"  {cli_exe} help                      # Show all available commands")
    
    return True

def find_executable(project_root):
    """Find the Blahaj PI CLI executable"""
    build_path = project_root / "build"
    
    if not build_path.exists():
        print(f"Error: Build directory not found: {build_path}")
        print("Please build the project first with --build")
        return None
    
    # Look for the executable in different locations based on platform and build system
    potential_paths = []
    
    exe_name = "blahajpi" if platform.system() != "Windows" else "blahajpi.exe"
    
    if platform.system() == "Windows":
        # Visual Studio puts executables in different subdirectories based on configuration
        for config in ["Debug", "Release", "RelWithDebInfo", "MinSizeRel"]:
            potential_paths.append(build_path / config / exe_name)
        # CMake might also put executables directly in bin/
        potential_paths.append(build_path / "bin" / exe_name)
    else:
        # On Unix-like systems, executables are typically in the bin directory
        potential_paths.append(build_path / "bin" / exe_name)
    
    # Find the first executable that exists
    for path in potential_paths:
        if path.exists():
            return path
    
    print("Error: Blahaj PI CLI executable not found.")
    print("Please build the project first with --build")
    return None

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
    
    # If the config name is a known shorthand, use the mapped file
    if config_name in config_files:
        config_path = config_files[config_name]
    else:
        # Otherwise, treat it as a direct path to a config file
        config_path = Path(config_name)
    
    # Check if the config file exists
    if not config_path.exists():
        print(f"Warning: Configuration file not found: {config_path}")
        return None
    
    return config_path

def run_blahajpi(executable_path, config_path, args):
    """Run Blahaj PI CLI with the given arguments"""
    # Start with the executable path
    cmd = [str(executable_path)]
    
    # Add config file if specified
    if config_path:
        cmd.extend(["--config", str(config_path)])
    
    # Add any additional arguments
    cmd.extend(args)
    
    print(f"Running Blahaj PI CLI: {' '.join(cmd)}")
    
    # If no arguments provided, show help message for typical commands
    if not args:
        print("\nTIP: Common Blahaj PI commands include:")
        print("  analyze - Analyze text for harmful content")
        print("  train   - Train a new model")
        print("  help    - Show all available commands")
        print("\nFor detailed help on a command, run:")
        print(f"  {executable_path} help <command>")
        print("\nExample usage:")
        print(f"  {executable_path} analyze --file data/examples/twitter_example.csv")
    
    return subprocess.run(cmd)

def run_demo(project_root, args):
    """Run a complete demonstration of Blahaj PI capabilities"""
    executable_path = find_executable(project_root)
    if not executable_path:
        print("Error: CLI executable not found. Building project first...")
        if not build_project(args, project_root):
            return False
        executable_path = find_executable(project_root)
        if not executable_path:
            return False
    
    print("\n" + "="*80)
    print("BLAHAJ PI CAPABILITY DEMONSTRATION")
    print("="*80)
    
    # Path to the example data file
    sample_file = project_root / "data" / "examples" / "twitter_example.csv"
    if not sample_file.exists():
        print(f"Error: Sample data not found at {sample_file}")
        print("Please ensure the data directory is properly set up")
        return False
    
    # Build directory for output
    build_dir = project_root / "build"
    build_dir.mkdir(exist_ok=True)
    
    # Run each main command to demonstrate functionality
    commands = [
        ["version"],
        ["help"],
        ["analyze", "--file", str(sample_file)],
        ["config", "list"],
        ["visualize", "--input", str(sample_file), "--output", str(build_dir / "visualization.txt")]
    ]
    
    for i, cmd in enumerate(commands, 1):
        print(f"\n=== {i}. RUNNING: {' '.join(cmd)} ===\n")
        subprocess.run([str(executable_path)] + cmd)
    
    print("\n=== DEMO COMPLETE ===")
    print("For more details on specific commands, run: ./dev.py --run -- help <command>")
    
    return True

def run_tests(project_root, args):
    """Run project tests"""
    build_dir = project_root / "build"
    
    # Check if build directory exists
    if not build_dir.exists():
        print("Build directory not found. Building project first...")
        if not build_project(args, project_root):
            return False
    
    print("\nRunning tests...")
    
    # Determine which tests to run
    test_labels = []
    if args.unit:
        test_labels.append("Unit")
    if args.integration:
        test_labels.append("Integration")
    if args.e2e:
        test_labels.append("E2E")
    
    # If no specific tests are selected, run all tests
    if not test_labels:
        test_labels = ["Unit", "Integration", "E2E"]
    
    print(f"Running {', '.join(test_labels)} tests...")
    
    # Run tests using CTest
    ctest_cmd = [
        "ctest",
        "--test-dir", str(build_dir),
    ]
    
    # Add labels if specific tests are requested
    if args.unit or args.integration or args.e2e:
        ctest_cmd.append("-L")
        ctest_cmd.append("|".join(test_labels))
    
    if args.verbose:
        ctest_cmd.append("--verbose")
    
    if platform.system() == "Windows":
        # Specify build configuration on Windows
        build_type = get_build_type(args.debug)
        ctest_cmd.extend(["-C", build_type])
    
    test_result = subprocess.run(ctest_cmd)
    
    if test_result.returncode == 0:
        print("\nAll tests passed!")
        return True
    else:
        print("\nSome tests failed!")
        return False

def generate_docs(project_root, open_browser=False):
    """Generate project documentation"""
    docs_dir = project_root / "docs"
    build_dir = project_root / "build"
    
    # Check if Doxygen is installed
    try:
        subprocess.run(["doxygen", "--version"], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("Error: Doxygen not found. Please install Doxygen to generate documentation.")
        return False
    
    # Check if MkDocs is installed
    try:
        subprocess.run(["mkdocs", "--version"], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("Warning: MkDocs not found. Only API documentation will be generated.")
        has_mkdocs = False
    else:
        has_mkdocs = True
    
    print("Generating documentation...")
    
    # Create output directories
    (project_root / "web" / "docs").mkdir(exist_ok=True)
    (project_root / "web" / "docs" / "api").mkdir(exist_ok=True)
    
    # Run Doxygen for API documentation
    doxygen_file = project_root / "Doxyfile"
    if not doxygen_file.exists():
        print(f"Warning: Doxyfile not found at {doxygen_file}")
        print("Creating a default Doxyfile...")
        
        # Create a default Doxyfile
        subprocess.run(["doxygen", "-g"], cwd=project_root)
        
        # Configure the Doxyfile for our project
        with open(doxygen_file, 'a') as f:
            f.write("\n# Blahaj PI specific settings\n")
            f.write("PROJECT_NAME = \"Blahaj PI\"\n")
            f.write("PROJECT_BRIEF = \"Your friendly shark detective keeping social waters safe\"\n")
            f.write("OUTPUT_DIRECTORY = docs/doxygen\n")
            f.write("INPUT = lib/include lib/src cli/include cli/src\n")
            f.write("RECURSIVE = YES\n")
            f.write("EXTRACT_ALL = YES\n")
            f.write("GENERATE_HTML = YES\n")
            f.write("HTML_OUTPUT = html\n")
            f.write("GENERATE_LATEX = NO\n")
    
    print("Running Doxygen to generate API documentation...")
    result = subprocess.run(["doxygen"], cwd=project_root)
    
    if result.returncode != 0:
        print("Error: Doxygen documentation generation failed")
        return False
    
    # Copy Doxygen output to web directory
    doxygen_output = project_root / "docs" / "doxygen" / "html"
    web_api_docs = project_root / "web" / "docs" / "api"
    
    if doxygen_output.exists():
        # Copy all files from Doxygen output to web docs directory
        if platform.system() == "Windows":
            # Windows specific command
            subprocess.run(f'xcopy "{doxygen_output}" "{web_api_docs}" /E /I /Y', shell=True)
        else:
            # Unix-like systems
            subprocess.run(f'cp -R "{doxygen_output}/." "{web_api_docs}"', shell=True)
        
        print(f"API documentation copied to {web_api_docs}")
    else:
        print(f"Warning: Doxygen output not found at {doxygen_output}")
    
    # Run MkDocs if available
    if has_mkdocs:
        mkdocs_yml = project_root / "docs" / "mkdocs.yml"
        if not mkdocs_yml.exists():
            print(f"Warning: mkdocs.yml not found at {mkdocs_yml}")
            print("Creating a default mkdocs.yml...")
            
            # Create basic mkdocs directory structure
            os.makedirs(project_root / "docs" / "user-guide", exist_ok=True)
            os.makedirs(project_root / "docs" / "developer-guide", exist_ok=True)
            
            # Create a default index.md
            with open(project_root / "docs" / "index.md", 'w') as f:
                f.write("# Blahaj PI Documentation\n\n")
                f.write("Your friendly shark detective keeping social waters safe.\n\n")
                f.write("## Quick Links\n\n")
                f.write("- [User Guide](user-guide/index.md)\n")
                f.write("- [Developer Guide](developer-guide/index.md)\n")
                f.write("- [API Documentation](api/index.html)\n")
            
            # Create a default mkdocs.yml
            with open(mkdocs_yml, 'w') as f:
                f.write("site_name: Blahaj PI\n")
                f.write("site_description: Your friendly shark detective keeping social waters safe\n")
                f.write("theme: readthedocs\n\n")
                f.write("nav:\n")
                f.write("  - Home: index.md\n")
                f.write("  - User Guide: user-guide/index.md\n")
                f.write("  - Developer Guide: developer-guide/index.md\n")
                f.write("  - API: api/index.html\n")
        
        print("Running MkDocs to generate user documentation...")
        result = subprocess.run(["mkdocs", "build", "-d", str(project_root / "web" / "docs")], 
                              cwd=project_root)
        
        if result.returncode != 0:
            print("Warning: MkDocs documentation generation failed")
    
    print("\nDocumentation generated successfully!")
    print(f"Documentation can be found at: {project_root / 'web' / 'docs'}")
    
    # Open documentation in browser if requested
    if open_browser:
        try:
            import webbrowser
            index_path = project_root / "web" / "docs" / "index.html"
            if index_path.exists():
                webbrowser.open(f"file://{index_path}")
                print("Documentation opened in your web browser.")
            else:
                print(f"Warning: Documentation index not found at {index_path}")
        except Exception as e:
            print(f"Error opening documentation in browser: {e}")
    
    return True

def main():
    """Main function"""
    args = parse_args()
    project_root = find_project_root()
    
    print(f"Blahaj PI Development Tool")
    print(f"Project root: {project_root}")
    
    # Handle clean request first
    if args.clean:
        clean_build_directory(project_root)
        if not (args.build or args.test or args.run or args.docs or args.demo):
            return 0
    
    # Handle documentation generation
    if args.docs:
        if not generate_docs(project_root, args.open_docs):
            return 1
    
    # Handle build request (or needed for other actions)
    if args.build or args.test or args.run or args.demo:
        # Check if we need to build
        executable_path = find_executable(project_root)
        need_build = args.build or not executable_path
        
        if need_build:
            if not build_project(args, project_root):
                return 1
            # Always run tests after building unless explicitly testing
            if not args.test and not args.demo:
                run_tests(project_root, args)
    
    # Handle test request
    if args.test:
        if not run_tests(project_root, args):
            return 1
    
    # Handle demo request
    if args.demo:
        if not run_demo(project_root, args):
            return 1
    
    # Handle run request
    if args.run:
        executable_path = find_executable(project_root)
        if not executable_path:
            return 1
        
        config_path = get_config_path(project_root, args.config)
        result = run_blahajpi(executable_path, config_path, args.blahajpi_args)
        return result.returncode
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
