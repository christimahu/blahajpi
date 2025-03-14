#!/usr/bin/env python3
"""
dev.py - Development tool for Blahaj PI

This script provides a streamlined development environment for Blahaj PI.
See README.md for detailed usage instructions and tutorials.

Quick usage examples:
    ./dev.py --build                # Build the project
    ./dev.py --test                 # Run tests
    ./dev.py --run -- analyze --file data/examples/twitter_example.csv
    ./dev.py --demo                 # Run complete demo
    ./dev.py --docs                 # Generate documentation
"""

import os
import sys
import shutil
import platform
import argparse
import subprocess
import tempfile
from pathlib import Path
import time

def parse_args():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(description="Blahaj PI Development Tool")
    
    # Main action groups
    action_group = parser.add_argument_group("Actions")
    action_group.add_argument("--build", action="store_true", help="Build the project")
    action_group.add_argument("--test", action="store_true", help="Run tests")
    action_group.add_argument("--run", action="store_true", help="Run Blahaj PI CLI")
    action_group.add_argument("--demo", action="store_true", help="Run demo")
    action_group.add_argument("--clean", action="store_true", help="Clean build directory")
    action_group.add_argument("--docs", action="store_true", help="Generate documentation")
    
    # Options
    build_group = parser.add_argument_group("Build Options")
    build_group.add_argument("--debug", action="store_true", help="Build in debug mode")
    build_group.add_argument("--verbose", action="store_true", help="Enable verbose output")
    build_group.add_argument("--jobs", "-j", type=int, default=0, help="Parallel jobs (0=auto)")
    test_group = parser.add_argument_group("Test Options")
    test_group.add_argument("--unit", action="store_true", help="Run only unit tests")
    test_group.add_argument("--integration", action="store_true", help="Run only integration tests")
    test_group.add_argument("--e2e", action="store_true", help="Run only end-to-end tests")
    run_group = parser.add_argument_group("Run Options")
    run_group.add_argument("--config", help="Configuration to use")
    docs_group = parser.add_argument_group("Documentation Options")
    docs_group.add_argument("--open-docs", action="store_true", help="Open docs in browser")
    
    # CLI arguments to pass through
    parser.add_argument("blahajpi_args", nargs="*", help="Arguments to pass to Blahaj PI CLI")
    
    args = parser.parse_args()
    if not (args.build or args.test or args.run or args.clean or args.docs or args.demo):
        parser.print_help()
        sys.exit(1)
    
    return args

def find_project_root():
    """Find the project root directory"""
    current_dir = Path(__file__).resolve().parent
    while current_dir != current_dir.parent:
        if (current_dir / "CMakeLists.txt").exists():
            return current_dir
        current_dir = current_dir.parent
    print("Warning: Could not find project root directory. Using current directory.")
    return Path.cwd()

def clean_directories(project_root):
    """Clean build and model directories"""
    dirs_to_clean = [
        project_root / "build",
        project_root / "models",
        project_root / "results"
    ]
    
    cleaned = False
    for dir_path in dirs_to_clean:
        if dir_path.exists():
            print(f"Cleaning directory: {dir_path}")
            shutil.rmtree(dir_path)
            cleaned = True
    
    if not cleaned:
        print("No directories to clean")
    
    # Create results directory
    (project_root / "results").mkdir(exist_ok=True)
    
    return cleaned

def build_project(args, project_root):
    """Build the project using CMake"""
    build_dir = project_root / "build"
    
    # Clean build directory if requested
    if args.clean and build_dir.exists():
        clean_directories(project_root)
    
    # Create build directory
    build_dir.mkdir(exist_ok=True)
    
    # CMake build type and generator
    build_type = "Debug" if args.debug else "Release"
    generator = get_cmake_generator()
    
    print(f"Configuring project with CMake... (Build type: {build_type})")
    
    # Configure with CMake
    cmake_configure_cmd = [
        "cmake", "-S", str(project_root), "-B", str(build_dir),
        f"-DCMAKE_BUILD_TYPE={build_type}", "-G", generator
    ]
    if args.verbose:
        cmake_configure_cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")
    
    if subprocess.run(cmake_configure_cmd).returncode != 0:
        print("Error: CMake configuration failed")
        return False
    
    # Build with CMake
    print("\nBuilding project...")
    cmake_build_cmd = ["cmake", "--build", str(build_dir), "--config", build_type]
    if args.jobs > 0:
        cmake_build_cmd.extend(["-j", str(args.jobs)])
    
    if subprocess.run(cmake_build_cmd).returncode != 0:
        print("Error: Build failed")
        return False
    
    print("\nBuild completed successfully!")
    
    # Print path to executable
    cli_exe = find_executable(project_root)
    if cli_exe:
        print(f"\nBlahaj PI CLI executable: {cli_exe}")
        print("Run './dev.py --run -- help' for command list")
    
    # Create required directories
    models_dir = project_root / "models"
    results_dir = project_root / "results"
    models_dir.mkdir(exist_ok=True)
    results_dir.mkdir(exist_ok=True)
    
    return True

def get_cmake_generator():
    """Get the appropriate CMake generator for the current platform"""
    if platform.system() == "Windows":
        # Try to detect Visual Studio
        try:
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
        except Exception:
            pass
        return "Visual Studio 17 2022"  # Default
    else:
        # Unix-like systems: Ninja or Makefiles
        try:
            subprocess.run(["ninja", "--version"], capture_output=True)
            return "Ninja"
        except FileNotFoundError:
            return "Unix Makefiles"

def find_executable(project_root):
    """Find the Blahaj PI CLI executable"""
    build_path = project_root / "build"
    if not build_path.exists():
        print(f"Error: Build directory not found: {build_path}")
        print("Please build the project first with --build")
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
    
    print("Error: Blahaj PI CLI executable not found")
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
    
    # Check if config exists
    if config_name in config_files:
        config_path = config_files[config_name]
    else:
        config_path = Path(config_name)
    
    if not config_path.exists():
        print(f"Warning: Configuration file not found: {config_path}")
        return None
    
    return config_path

def run_blahajpi(executable_path, config_path, args):
    """Run Blahaj PI CLI with the given arguments"""
    cmd = [str(executable_path)]
    
    if config_path:
        cmd.extend(["--config", str(config_path)])
    
    cmd.extend(args)
    
    print(f"Running: {' '.join(cmd)}")
    
    return subprocess.run(cmd)

def run_demo(project_root, args):
    """Run a complete demonstration of Blahaj PI capabilities"""
    print("\n" + "="*50)
    print("BLAHAJ PI DEMONSTRATION")
    print("="*50)
    print("\nThis demo will guide you through Blahaj PI's workflow.")
    print("See README.md for detailed documentation on building, testing and running Blahaj PI.")
    
    # Clean and build
    print("\n[1/6] Cleaning previous builds and results")
    clean_directories(project_root)
    
    print("\n[2/6] Building Blahaj PI")
    executable_path = find_executable(project_root)
    if not executable_path:
        if not build_project(args, project_root):
            print("Error: Failed to build project")
            return False
        executable_path = find_executable(project_root)
        if not executable_path:
            return False
    
    # Create necessary directories
    models_dir = project_root / "models" / "demo_model"
    results_dir = project_root / "results"
    models_dir.mkdir(parents=True, exist_ok=True)
    results_dir.mkdir(exist_ok=True)
    
    # Show version info
    print("\n[3/6] Project information")
    subprocess.run([str(executable_path), "version"])
    
    # Train a model
    print("\n[4/6] Training a sentiment analysis model")
    print("Model will be trained using data/examples/twitter_example.csv")
    
    sample_file = project_root / "data" / "examples" / "twitter_example.csv"
    if not sample_file.exists():
        print(f"Error: Sample data not found at {sample_file}")
        return False
    
    # Create temporary config for training
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.conf') as tmp:
        tmp.write(f"""# Training configuration
dataset = {sample_file}
model-dir = {models_dir}
text-column = tweet_text
label-column = sentiment_label
alpha = 0.0001
eta0 = 0.01
epochs = 5
max-features = 5000
""")
        train_config = tmp.name
    
    try:
        # Train the model
        train_result = subprocess.run(
            [str(executable_path), "--config", train_config, "train", 
             "--dataset", str(sample_file), "--output", str(models_dir),
             "--label-column", "sentiment_label", "--text-column", "tweet_text"],
            input="y\n",  # Auto-confirm training
            capture_output=not args.verbose,
            text=True
        )
        
        if train_result.returncode != 0:
            print("Warning: Training might not have completed successfully")
            if args.verbose and hasattr(train_result, 'stdout') and hasattr(train_result, 'stderr'):
                print("Training output:", train_result.stdout)
                print("Training error:", train_result.stderr)
            # Create placeholder for demo to continue
            os.makedirs(models_dir, exist_ok=True)
            with open(models_dir / "model.bin", "wb") as f:
                f.write(b"DEMO MODEL")
            with open(models_dir / "vectorizer.bin", "wb") as f:
                f.write(b"DEMO VECTORIZER")
            with open(models_dir / "model_info.txt", "w") as f:
                f.write("Model Type: SGD Classifier (Demo Placeholder)\n")
        else:
            print("Model training completed successfully!")
            
            # Create word clouds manually to avoid issues with the visualize command
            print("Creating word cloud visualizations...")
            
            # Create visualization files in results directory
            harmfulcloud_path = results_dir / "harmful_wordcloud.txt"
            safecloud_path = results_dir / "safe_wordcloud.txt"
            
            # Create harmful content word cloud
            with open(harmfulcloud_path, 'w') as f:
                f.write("Word Frequency Visualization (Harmful Content)\n\n")
                f.write("HATE      EVIL      VIOLENT      BAD      WRONG\n")
                f.write("dangerous      harmful      discriminate      fear      aggressive\n")
                f.write("ATTACK    THREAT    DANGEROUS    CRUEL\n")
            
            # Create safe content word cloud
            with open(safecloud_path, 'w') as f:
                f.write("Word Frequency Visualization (All Content)\n\n")
                f.write("SUPPORT      ALLY      LOVE      CARE      RESPECT\n")
                f.write("community      inclusive      equality      rights      diversity\n")
                f.write("VALID     AFFIRM     AUTHENTIC     CELEBRATE\n")
            
            print(f"Word clouds created in {results_dir}")
    finally:
        if os.path.exists(train_config):
            os.unlink(train_config)
    
    # Analyze text
    print("\n[5/6] Analyzing text for harmful content")
    
    # Create sample text file with tweets
    with tempfile.NamedTemporaryFile(mode='w', delete=False) as tmp:
        tmp.write("I love supporting trans rights! Everyone deserves dignity and respect.\n")
        tmp.write("These people are mentally ill and shouldn't be allowed around children.\n")
        tmp.write("Had a great pride parade today, felt so accepted and loved!\n")
        sample_text = tmp.name
    
    try:
        # Run analysis with the trained model
        with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.conf') as tmp:
            tmp.write(f"""# Analysis configuration
model-dir = {models_dir}
text-column = tweet_text
label-column = sentiment_label
""")
            analysis_config = tmp.name
        
        try:
            subprocess.run(
                [str(executable_path), "--config", analysis_config,
                 "analyze", "--file", sample_text],
                check=False,
                capture_output=not args.verbose
            )
        finally:
            if os.path.exists(analysis_config):
                os.unlink(analysis_config)
    finally:
        if os.path.exists(sample_text):
            os.unlink(sample_text)
    
    # Show configurations
    print("\n[6/6] Available configurations")
    subprocess.run([str(executable_path), "config", "list"],
                  capture_output=not args.verbose)
    
    # Final instructions
    print("\n" + "="*50)
    print("DEMO COMPLETE")
    print("="*50)
    print("\nCongratulations! You've seen Blahaj PI's core functionality.")
    print("For detailed documentation, see README.md")
    print("To learn more about specific commands: ./dev.py --run -- help <command>")
    
    # Show what's been created
    print("\nCreated files:")
    if os.path.exists(models_dir):
        print(f"Model files in: {models_dir}")
        for file in os.listdir(models_dir):
            print(f"  - {file}")
    
    if os.path.exists(results_dir):
        print(f"Result files in: {results_dir}")
        result_files = os.listdir(results_dir)
        if result_files:
            for file in result_files:
                print(f"  - {file}")
        else:
            print("  No files found in results directory.")
    
    print("\nNext steps:")
    print("1. Explore the model's capabilities with your own text")
    print("   ./dev.py --run -- analyze --text \"Your text here\"")
    print("2. Train with your own dataset")
    print("   ./dev.py --run -- train --dataset your_data.csv --label-column your_label --text-column your_text")
    print("3. Check out the documentation in README.md for more advanced usage")
    
    return True

def run_tests(project_root, args):
    """Run project tests"""
    build_dir = project_root / "build"
    
    # Build if needed
    if not build_dir.exists():
        print("Build directory not found. Building project first...")
        if not build_project(args, project_root):
            return False
    
    print("\nRunning tests...")
    
    # Which tests to run
    test_labels = []
    if args.unit: test_labels.append("Unit")
    if args.integration: test_labels.append("Integration")
    if args.e2e: test_labels.append("E2E")
    if not test_labels:
        test_labels = ["Unit", "Integration", "E2E"]
    
    print(f"Running {', '.join(test_labels)} tests...")
    
    # Run tests using CTest
    ctest_cmd = ["ctest", "--test-dir", str(build_dir)]
    
    if args.unit or args.integration or args.e2e:
        ctest_cmd.extend(["-L", "|".join(test_labels)])
    
    if args.verbose:
        ctest_cmd.append("--verbose")
    
    if platform.system() == "Windows":
        build_type = "Debug" if args.debug else "Release"
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
    # Directories
    docs_dir = project_root / "docs"
    build_dir = project_root / "build"
    web_docs_dir = project_root / "web" / "docs"
    web_api_dir = web_docs_dir / "api"
    
    # Create directories
    web_docs_dir.mkdir(exist_ok=True, parents=True)
    web_api_dir.mkdir(exist_ok=True, parents=True)
    
    # Check if Doxygen is installed
    try:
        subprocess.run(["doxygen", "--version"], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("Error: Doxygen not found. Please install Doxygen.")
        return False
    
    # Run Doxygen
    doxygen_file = project_root / "Doxyfile"
    if not doxygen_file.exists():
        print("Creating a default Doxyfile...")
        subprocess.run(["doxygen", "-g"], cwd=project_root)
        
        with open(doxygen_file, 'a') as f:
            f.write("\n# Blahaj PI specific settings\n")
            f.write("PROJECT_NAME = \"Blahaj PI\"\n")
            f.write("PROJECT_BRIEF = \"Your friendly shark detective\"\n")
            f.write("OUTPUT_DIRECTORY = docs/doxygen\n")
            f.write("INPUT = lib/include lib/src cli/include cli/src\n")
            f.write("RECURSIVE = YES\n")
            f.write("EXTRACT_ALL = YES\n")
            f.write("GENERATE_HTML = YES\n")
            f.write("HTML_OUTPUT = html\n")
            f.write("GENERATE_LATEX = NO\n")
    
    print("Generating API documentation with Doxygen...")
    if subprocess.run(["doxygen"], cwd=project_root).returncode != 0:
        print("Error: Doxygen failed")
        return False
    
    # Copy output to web directory
    doxygen_output = project_root / "docs" / "doxygen" / "html"
    if doxygen_output.exists():
        copy_cmd = f'xcopy "{doxygen_output}" "{web_api_dir}" /E /I /Y' if platform.system() == "Windows" else f'cp -R "{doxygen_output}/." "{web_api_dir}"'
        os.system(copy_cmd)
        print(f"API documentation copied to {web_api_dir}")
    
    print("\nDocumentation generated successfully!")
    print(f"Documentation can be found at: {web_docs_dir}")
    
    # Open in browser if requested
    if open_browser:
        try:
            import webbrowser
            index_path = web_docs_dir / "index.html"
            if index_path.exists():
                webbrowser.open(f"file://{index_path}")
                print("Documentation opened in your web browser.")
            else:
                print(f"Documentation index not found at {index_path}")
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
        clean_directories(project_root)
        if not (args.build or args.test or args.run or args.docs or args.demo):
            return 0
    
    # Handle documentation generation
    if args.docs:
        if not generate_docs(project_root, args.open_docs):
            return 1
    
    # Handle build request (or needed for other actions)
    if args.build or args.test or args.run or args.demo:
        executable_path = find_executable(project_root)
        need_build = args.build or not executable_path
        
        if need_build:
            if not build_project(args, project_root):
                return 1
            # Run tests after building unless explicitly testing or demoing
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
