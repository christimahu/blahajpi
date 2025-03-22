"""
build.py - Build system functionality for Blahaj PI development tool
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path

def run_build(args):
    """Build the project"""
    project_root = find_project_root()
    build_dir = project_root / "build"
    
    # Clean build directory if it exists and clean is requested
    if args.clean and build_dir.exists():
        return clean_build(args)
    
    # Create build directory
    build_dir.mkdir(exist_ok=True)
    
    # Determine build type
    build_type = "Debug" if args.debug else "Release"
    
    # Determine generator based on platform
    generator = get_cmake_generator()
    
    print(f"Configuring project with CMake... (Build type: {build_type})")
    
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
    
    result = subprocess.run(cmake_configure_cmd)
    if result.returncode != 0:
        print("Error: CMake configuration failed")
        return result.returncode
    
    # Build with CMake
    print("\nBuilding project...")
    cmake_build_cmd = [
        "cmake", 
        "--build", str(build_dir), 
        "--config", build_type
    ]
    
    # Add parallel jobs argument if specified
    if args.jobs > 0:
        cmake_build_cmd.extend(["-j", str(args.jobs)])
    
    result = subprocess.run(cmake_build_cmd)
    if result.returncode != 0:
        print("Error: Build failed")
        return result.returncode
    
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
    
    # Generate documentation if needed
    if not args.no_docs and os.path.exists(os.path.join(project_root, "Doxyfile")):
        print("\nGenerating documentation...")
        generate_docs(project_root)
    
    return 0

def generate_docs(project_root):
    """
    Generate documentation with Doxygen using custom theme
    
    Args:
        project_root: Path to the project root directory
    """
    # Check if Doxygen is installed
    doxygen_path = shutil.which("doxygen")
    if not doxygen_path:
        print("Warning: Doxygen not found, skipping documentation generation")
        return
    
    # Ensure web/docs directory exists
    docs_dir = project_root / "web" / "docs"
    os.makedirs(docs_dir, exist_ok=True)
    
    # Copy Blahaj image to the documentation directory
    # This ensures the image is available when docs are viewed
    image_src = project_root / "web" / "media" / "blahajpi.webp"
    image_dest = docs_dir / "blahajpi.webp"
    if image_src.exists():
        print(f"Copying logo image to docs directory: {image_dest}")
        shutil.copy(image_src, image_dest)
    else:
        print(f"Warning: Logo image not found at {image_src}")
    
    # Copy favicon if it exists
    favicon_src = project_root / "web" / "media" / "favicon.ico"
    favicon_dest = docs_dir / "favicon.ico"
    if favicon_src.exists():
        print(f"Copying favicon to docs directory: {favicon_dest}")
        shutil.copy(favicon_src, favicon_dest)
    
    # Run Doxygen to generate documentation
    print("Running Doxygen to generate documentation...")
    result = subprocess.run([doxygen_path], cwd=project_root)
    
    if result.returncode != 0:
        print("Warning: Doxygen returned non-zero exit code")
    else:
        print(f"Documentation generated successfully in {docs_dir}")
        # Check if index.html was generated
        if (docs_dir / "index.html").exists():
            print("Main documentation page available at: web/docs/index.html")
        else:
            print("Warning: index.html not found in documentation directory")

def clean_build(args):
    """Clean build and generated directories"""
    project_root = find_project_root()
    
    dirs_to_clean = [
        project_root / "build",
        project_root / "build-asan",
        project_root / "build-ubsan",
        project_root / "build-coverage"
    ]
    
    output_dirs = [
        project_root / "models",
        project_root / "results"
    ]
    
    # Clean documentation directory if needed
    if hasattr(args, 'clean_docs') and args.clean_docs:
        dirs_to_clean.append(project_root / "web" / "docs")
    
    # Clean build directories
    cleaned = False
    for dir_path in dirs_to_clean:
        if dir_path.exists():
            print(f"Cleaning directory: {dir_path}")
            try:
                shutil.rmtree(dir_path)
                cleaned = True
            except Exception as e:
                print(f"Error cleaning {dir_path}: {e}")
    
    # Clean output directories if --clean-all option exists
    if hasattr(args, 'clean_all') and args.clean_all:
        for dir_path in output_dirs:
            if dir_path.exists():
                print(f"Cleaning directory: {dir_path}")
                try:
                    shutil.rmtree(dir_path)
                    cleaned = True
                except Exception as e:
                    print(f"Error cleaning {dir_path}: {e}")
    
    if not cleaned:
        print("No directories to clean")
    
    # Re-create output directories
    for dir_path in output_dirs:
        os.makedirs(dir_path, exist_ok=True)
    
    return 0

def get_cmake_generator():
    """Get the appropriate CMake generator for the current platform"""
    import platform
    
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
    import platform
    
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
