"""
docs.py - Documentation generation for Blahaj PI development tool

This module provides functionality to generate documentation for the 
Blahaj PI project using Doxygen, with proper output to the web/docs directory.
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path
import webbrowser

def generate_docs(args):
    """
    Generate project documentation using Doxygen.
    
    This function runs Doxygen to generate HTML documentation for
    the Blahaj PI project. It ensures documents are created directly
    in the web/docs directory with the proper structure.
    
    Args:
        args: Command line arguments object with optional 'open_docs' attribute
        
    Returns:
        int: Exit code (0 for success, non-zero for errors)
    """
    project_root = find_project_root()
    
    # Define target directory for documentation
    docs_dir = project_root / "web" / "docs"
    
    # Create the target directory if it doesn't exist
    os.makedirs(docs_dir, exist_ok=True)
    print(f"Documentation output directory: {docs_dir}")
    
    # Check for Doxygen installation
    doxygen_exe = shutil.which("doxygen")
    if not doxygen_exe:
        print("Error: Doxygen not found. Please install Doxygen.")
        print("See https://www.doxygen.nl/download.html for installation instructions.")
        return 1
    
    # Show Doxygen version for diagnostics
    print(f"Using Doxygen: {get_doxygen_version(doxygen_exe)}")
    
    # Run Doxygen with the project Doxyfile
    doxyfile_path = project_root / "Doxyfile"
    if not doxyfile_path.exists():
        print(f"Error: Doxyfile not found at {doxyfile_path}")
        print("Please create a Doxyfile in the project root directory.")
        return 1
    
    print(f"Running Doxygen with configuration file: {doxyfile_path}")
    result = subprocess.run([doxygen_exe, str(doxyfile_path)], 
                           cwd=project_root,
                           capture_output=True, 
                           text=True)
    
    # Check Doxygen execution result
    if result.returncode != 0:
        print("Error: Doxygen failed to generate documentation")
        print("Doxygen output:")
        print(result.stderr)
        return 1
    
    # Print any warnings from Doxygen
    if result.stderr:
        print("Doxygen warnings:")
        print(result.stderr)
    
    # After Doxygen completes, copy all theme files to ensure they're in the right place
    print("Copying theme files to documentation directory...")
    
    # Copy all files from doxytheme to web/docs
    doxytheme_dir = project_root / "doxytheme"
    if doxytheme_dir.exists():
        for file in os.listdir(doxytheme_dir):
            source_file = doxytheme_dir / file
            if source_file.is_file() and file.endswith(('.css', '.html', '.js')):
                dest_file = docs_dir / file
                shutil.copy(source_file, dest_file)
                print(f"Copied {file} to {dest_file}")
    
    # Copy media files
    media_dir = project_root / "web" / "media"
    if media_dir.exists():
        # Copy logo
        logo_file = media_dir / "blahajpi.webp"
        if logo_file.exists():
            logo_dest = docs_dir / "blahajpi.webp"
            shutil.copy(logo_file, logo_dest)
            print(f"Copied blahajpi.webp to {logo_dest}")
    
    # Copy favicon (in web directory, not media)
    favicon_file = project_root / "web" / "favicon.ico"
    if favicon_file.exists():
        favicon_dest = docs_dir / "favicon.ico"
        shutil.copy(favicon_file, favicon_dest)
        print(f"Copied favicon.ico to {favicon_dest}")
    
    # Verify documentation was generated
    index_path = docs_dir / "index.html"
    if not index_path.exists():
        print(f"Warning: Documentation index not found at {index_path}")
        print("Doxygen may have generated files in a different location.")
    else:
        print(f"Documentation generated successfully to {docs_dir}")
    
    # Open in browser if requested
    if hasattr(args, 'open_docs') and args.open_docs and index_path.exists():
        try:
            webbrowser.open(f"file://{index_path}")
            print("Documentation opened in your web browser.")
        except Exception as e:
            print(f"Error opening documentation in browser: {e}")
    
    return 0

def get_doxygen_version(doxygen_exe):
    """
    Get the version string of the installed Doxygen.
    
    Args:
        doxygen_exe: Path to the Doxygen executable
        
    Returns:
        str: Version string or error message
    """
    try:
        result = subprocess.run([doxygen_exe, "--version"], 
                              capture_output=True, 
                              text=True)
        return result.stdout.strip()
    except Exception as e:
        return f"Unknown version (Error: {e})"

def find_project_root():
    """
    Find the project root directory based on CMakeLists.txt file.
    
    Returns:
        Path: Path to the project root directory
        
    Raises:
        SystemExit: If the project root directory cannot be found
    """
    current_dir = Path.cwd()
    while current_dir != current_dir.parent:
        if (current_dir / "CMakeLists.txt").exists():
            return current_dir
        current_dir = current_dir.parent
    
    print("Error: Could not find project root directory.")
    print("Please run this script from within the Blahaj PI project directory.")
    sys.exit(1)
