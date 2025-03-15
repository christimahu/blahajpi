"""
docs.py - Documentation generation for Blahaj PI development tool
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path

def generate_docs(args):
    """Generate project documentation"""
    project_root = find_project_root()
    
    # Directories
    docs_dir = project_root / "docs"
    build_dir = project_root / "build"
    web_docs_dir = project_root / "web" / "docs"
    web_api_dir = web_docs_dir / "api"
    
    # Create directories
    web_docs_dir.mkdir(exist_ok=True, parents=True)
    web_api_dir.mkdir(exist_ok=True, parents=True)
    
    # Check if Doxygen is installed
    doxygen_exe = shutil.which("doxygen")
    if not doxygen_exe:
        print("Error: Doxygen not found. Please install Doxygen.")
        return 1
    
    # Run Doxygen
    doxygen_file = project_root / "Doxyfile"
    if not doxygen_file.exists():
        print("Creating a default Doxyfile...")
        subprocess.run([doxygen_exe, "-g"], cwd=project_root)
        
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
    result = subprocess.run([doxygen_exe], cwd=project_root)
    if result.returncode != 0:
        print("Error: Doxygen failed")
        return 1
    
    # Copy output to web directory
    doxygen_output = project_root / "docs" / "doxygen" / "html"
    if doxygen_output.exists():
        # Use appropriate copy command based on platform
        import platform
        if platform.system() == "Windows":
            copy_cmd = f'xcopy "{doxygen_output}" "{web_api_dir}" /E /I /Y'
            os.system(copy_cmd)
        else:
            # For Unix-like systems
            copy_cmd = ['cp', '-R', f"{doxygen_output}/.", f"{web_api_dir}"]
            subprocess.run(copy_cmd)
        
        print(f"API documentation copied to {web_api_dir}")
    
    # Check for MkDocs installation
    mkdocs_exe = shutil.which("mkdocs")
    if mkdocs_exe:
        print("Generating documentation website with MkDocs...")
        
        # Create basic docs if they don't exist
        os.makedirs(project_root / "docs" / "user-guide", exist_ok=True)
        os.makedirs(project_root / "docs" / "developer-guide", exist_ok=True)
        
        mkdocs_file = project_root / "docs" / "mkdocs.yml"
        if not mkdocs_file.exists():
            print("Creating basic MkDocs configuration...")
            with open(mkdocs_file, 'w') as f:
                f.write("site_name: Blahaj PI\n")
                f.write("theme: readthedocs\n")
                
                # Create a minimal index if it doesn't exist
                index_file = project_root / "docs" / "index.md"
                if not index_file.exists():
                    with open(index_file, 'w') as idx:
                        idx.write("# Blahaj PI\n")
                        idx.write("Your friendly shark detective keeping social waters safe.\n")
        
        # Build MkDocs
        subprocess.run([mkdocs_exe, "build", "-f", str(mkdocs_file), "-d", str(web_docs_dir)], 
                      cwd=project_root, 
                      capture_output=True)
    
    print("\nDocumentation generated successfully!")
    print(f"Documentation can be found at: {web_docs_dir}")
    
    # Open in browser if requested
    if args.open_docs:
        try:
            import webbrowser
            index_path = web_docs_dir / "index.html"
            api_index_path = web_api_dir / "index.html"
            
            # Try opening API docs first, then general docs
            if api_index_path.exists():
                webbrowser.open(f"file://{api_index_path}")
                print("API documentation opened in your web browser.")
            elif index_path.exists():
                webbrowser.open(f"file://{index_path}")
                print("Documentation opened in your web browser.")
            else:
                print(f"Documentation index not found")
        except Exception as e:
            print(f"Error opening documentation in browser: {e}")
    
    return 0

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
