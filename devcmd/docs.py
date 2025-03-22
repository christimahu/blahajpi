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
    
    # Define target directory directly
    web_docs_dir = project_root / "web" / "docs"
    
    # Create the target directory
    os.makedirs(web_docs_dir, exist_ok=True)
    print(f"Created output directory: {web_docs_dir}")
    
    # Check if Doxygen is installed
    doxygen_exe = shutil.which("doxygen")
    if not doxygen_exe:
        print("Error: Doxygen not found. Please install Doxygen.")
        return 1
    
    # Get Doxygen version
    try:
        version_result = subprocess.run([doxygen_exe, "--version"], capture_output=True, text=True)
        print(f"Doxygen version used: {version_result.stdout.strip()}")
    except Exception as e:
        print(f"Could not determine Doxygen version: {e}")
    
    # Create a temporary Doxyfile directly using a string
    temp_doxyfile = project_root / "temp_doxyfile"
    with open(temp_doxyfile, 'w') as f:
        f.write(f"""
# Temporary Doxyfile for Blahaj PI
PROJECT_NAME           = "Blahaj PI"
PROJECT_BRIEF          = "Your friendly shark detective"
OUTPUT_DIRECTORY       = {web_docs_dir}
CREATE_SUBDIRS         = NO
ALLOW_UNICODE_NAMES    = YES
OUTPUT_LANGUAGE        = English
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
INPUT                  = {project_root}/lib/include {project_root}/lib/src {project_root}/cli/include {project_root}/cli/src
FILE_PATTERNS          = *.hpp *.cpp *.h *.c
RECURSIVE              = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = YES
EXTRACT_STATIC         = YES
EXTRACT_LOCAL_CLASSES  = YES
GENERATE_HTML          = YES
HTML_OUTPUT            = .
GENERATE_LATEX         = NO
GENERATE_XML           = NO
GENERATE_MAN           = NO
GENERATE_RTF           = NO
CASE_SENSE_NAMES       = NO
VERBATIM_HEADERS       = YES
""")
    
    print("Generated temporary Doxyfile for direct output")
    
    # Run Doxygen with the temporary config
    print("Generating API documentation with Doxygen...")
    result = subprocess.run([doxygen_exe, str(temp_doxyfile)], cwd=project_root)
    
    # Remove the temporary Doxyfile
    os.remove(temp_doxyfile)
    print("Removed temporary Doxyfile")
    
    if result.returncode != 0:
        print("Error: Doxygen failed")
        return 1
    
    print(f"Doxygen documentation generated to {web_docs_dir}")
    
    # Check for MkDocs installation
    mkdocs_exe = shutil.which("mkdocs")
    if mkdocs_exe:
        print("Generating documentation website with MkDocs...")
        
        # Create basic docs structure if needed
        docs_dir = project_root / "docs"
        os.makedirs(docs_dir, exist_ok=True)
        os.makedirs(docs_dir / "user-guide", exist_ok=True)
        os.makedirs(docs_dir / "developer-guide", exist_ok=True)
        
        # Create basic index.md if it doesn't exist
        index_file = docs_dir / "index.md"
        if not index_file.exists():
            with open(index_file, 'w') as f:
                f.write("# Blahaj PI\n\n")
                f.write("Your friendly shark detective keeping social waters safe.\n")
        
        # Create mkdocs.yml if it doesn't exist
        mkdocs_file = docs_dir / "mkdocs.yml"
        if not mkdocs_file.exists():
            with open(mkdocs_file, 'w') as f:
                f.write("site_name: Blahaj PI\n")
                f.write("theme: readthedocs\n")
        
        # Build MkDocs directly to web/docs
        try:
            print(f"Building MkDocs to {web_docs_dir}...")
            subprocess.run(
                [mkdocs_exe, "build", "-f", str(mkdocs_file), "-d", str(web_docs_dir)], 
                cwd=project_root,
                check=True
            )
            print("MkDocs build completed successfully.")
        except Exception as e:
            print(f"Error building MkDocs: {e}")
    else:
        print("MkDocs not found. Only API documentation will be available.")
        print("To install MkDocs, run: pip install mkdocs mkdocs-material")
    
    print("\nDocumentation generation completed.")
    print(f"Documentation available at: {web_docs_dir}")
    
    # Open in browser if requested
    if args.open_docs:
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
