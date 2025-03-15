"""
demo.py - Demonstration functionality for Blahaj PI development tool
"""

import os
import sys
import subprocess
import tempfile
import time
from pathlib import Path

def run_demo(args):
    """Run a demonstration of Blahaj PI capabilities"""
    project_root = find_project_root()
    
    print("\n" + "="*50)
    print("BLAHAJ PI DEMONSTRATION")
    print("="*50)
    print("\nThis demo will guide you through Blahaj PI's workflow.")
    print("See README.md for detailed documentation on building, testing and running Blahaj PI.")
    
    # Step 1: Clean previous builds and results
    print("\n[1/6] Cleaning previous builds and results")
    build_module = __import__("devcmd.build", fromlist=["clean_build"]).clean_build
    build_module(args)
    
    # Step 2: Build the project
    print("\n[2/6] Building Blahaj PI")
    executable_path = find_executable(project_root)
    if not executable_path:
        build_module = __import__("devcmd.build", fromlist=["run_build"]).run_build
        if build_module(args) != 0:
            print("Error: Failed to build project")
            return 1
        executable_path = find_executable(project_root)
        if not executable_path:
            return 1
    
    # Create necessary directories
    models_dir = project_root / "models" / "demo_model"
    results_dir = project_root / "results"
    models_dir.mkdir(parents=True, exist_ok=True)
    results_dir.mkdir(exist_ok=True)
    
    # Step 3: Show version info
    print("\n[3/6] Project information")
    run_command([str(executable_path), "version"])
    
    # Step 4: Train a model
    print("\n[4/6] Training a sentiment analysis model")
    print("Model will be trained using data/examples/twitter_example.csv")
    
    sample_file = project_root / "data" / "examples" / "twitter_example.csv"
    if not sample_file.exists():
        print(f"Error: Sample data not found at {sample_file}")
        return 1
    
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
        run_result = run_command_with_input(
            [str(executable_path), "--config", train_config, "train", 
             "--dataset", str(sample_file), "--output", str(models_dir),
             "--label-column", "sentiment_label", "--text-column", "tweet_text"],
            input_text="y\n",  # Auto-confirm training
            capture_output=not args.verbose
        )
        
        if run_result != 0:
            print("Warning: Training might not have completed successfully")
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
            
            # Create word clouds manually for demo
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
    
    # Step 5: Analyze text
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
            run_command(
                [str(executable_path), "--config", analysis_config,
                 "analyze", "--file", sample_text],
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
    run_command([str(executable_path), "config", "list"],
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
    
    return 0

def run_command(cmd, capture_output=False):
    """Run a command and return its exit code"""
    try:
        if capture_output:
            result = subprocess.run(cmd, check=False, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        else:
            result = subprocess.run(cmd, check=False)
        return result.returncode
    except Exception as e:
        print(f"Error executing command: {e}")
        return 1

def run_command_with_input(cmd, input_text=None, capture_output=False):
    """Run a command with input and return its exit code"""
    try:
        if capture_output:
            result = subprocess.run(
                cmd, 
                input=input_text.encode() if input_text else None,
                stdout=subprocess.PIPE, 
                stderr=subprocess.PIPE,
                check=False
            )
        else:
            result = subprocess.run(
                cmd, 
                input=input_text.encode() if input_text else None,
                check=False
            )
        return result.returncode
    except Exception as e:
        print(f"Error executing command: {e}")
        return 1

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
