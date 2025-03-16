"""
test.py - Test runner functionality for Blahaj PI development tool
"""

import os
import sys
import subprocess
import shutil
from pathlib import Path

def run_tests(args):
   """Run tests based on command line arguments"""
   project_root = find_project_root()
   
   # Handle special test modes
   if args.run_asan:
       return run_asan_tests(args, project_root)
   
   if args.run_ubsan:
       return run_ubsan_tests(args, project_root)
       
   if args.coverage:
       return run_coverage_tests(args, project_root)
       
   if args.full_check:
       return run_full_check(args, project_root)
   
   # Determine test types to run
   test_types = []
   if args.unit:
       test_types.append("Unit")
   if args.integration:
       test_types.append("Integration")
   if args.e2e:
       test_types.append("E2E")
   
   # If no specific test type is specified, run all tests
   if not test_types:
       test_types = ["Unit", "Integration", "E2E"]
   
   print(f"Running {', '.join(test_types)} tests...")
   
   # Build the project if needed
   build_dir = project_root / "build"
   if not build_dir.exists():
       print("Build directory not found. Building project first...")
       build_module = __import__("devcmd.build", fromlist=["run_build"]).run_build
       if build_module(args) != 0:
           return 1
   
   # Set up CTest command with verbose output
   ctest_cmd = ["ctest", "--test-dir", str(build_dir), "--verbose"]
   
   # Add test filter by label
   if test_types:
       ctest_cmd.extend(["-L", "|".join(test_types)])
   
   # Add test filter by name if specified
   if hasattr(args, 'filter') and args.filter:
       ctest_cmd.extend(["-R", args.filter])
   
   # Run tests
   try:
       result = subprocess.run(ctest_cmd, check=False)
       
       if result.returncode == 0:
           print("\nAll tests passed!")
           return 0
       else:
           print("\nSome tests failed!")
           return result.returncode
   except Exception as e:
       print(f"Error running tests: {e}")
       return 1

def run_asan_tests(args, project_root):
   """Run tests with Address Sanitizer"""
   print("Running tests with Address Sanitizer...")
   
   # Create a build directory specifically for ASAN
   build_dir = project_root / "build-asan"
   os.makedirs(build_dir, exist_ok=True)
   
   # Configure with ASAN enabled
   cmake_cmd = [
       "cmake", 
       "-B", str(build_dir),
       "-DCMAKE_BUILD_TYPE=Debug",
       "-DENABLE_ASAN=ON"
   ]
   
   if args.verbose:
       cmake_cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")
   
   print("Configuring build with Address Sanitizer...")
   result = subprocess.run(cmake_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to configure build with Address Sanitizer")
       return result.returncode
   
   # Build the project
   build_cmd = ["cmake", "--build", str(build_dir)]
   print("Building project with Address Sanitizer...")
   result = subprocess.run(build_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to build project with Address Sanitizer")
       return result.returncode
   
   # Run tests with verbose output
   ctest_cmd = ["ctest", "--test-dir", str(build_dir), "--verbose"]
   
   # Add test filter if specified
   if hasattr(args, 'filter') and args.filter:
       ctest_cmd.extend(["-R", args.filter])
   
   print("Running tests with Address Sanitizer...")
   result = subprocess.run(ctest_cmd, cwd=build_dir)
   
   if result.returncode == 0:
       print("\nAll Address Sanitizer tests passed!")
   else:
       print("\nSome Address Sanitizer tests failed!")
   
   return result.returncode

def run_ubsan_tests(args, project_root):
   """Run tests with Undefined Behavior Sanitizer"""
   print("Running tests with Undefined Behavior Sanitizer...")
   
   # Create a build directory specifically for UBSAN
   build_dir = project_root / "build-ubsan"
   os.makedirs(build_dir, exist_ok=True)
   
   # Configure with UBSAN enabled
   cmake_cmd = [
       "cmake", 
       "-B", str(build_dir),
       "-DCMAKE_BUILD_TYPE=Debug",
       "-DENABLE_UBSAN=ON"
   ]
   
   if args.verbose:
       cmake_cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")
   
   print("Configuring build with Undefined Behavior Sanitizer...")
   result = subprocess.run(cmake_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to configure build with Undefined Behavior Sanitizer")
       return result.returncode
   
   # Build the project
   build_cmd = ["cmake", "--build", str(build_dir)]
   print("Building project with Undefined Behavior Sanitizer...")
   result = subprocess.run(build_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to build project with Undefined Behavior Sanitizer")
       return result.returncode
   
   # Run tests with verbose output
   ctest_cmd = ["ctest", "--test-dir", str(build_dir), "--verbose"]
   
   # Add test filter if specified
   if hasattr(args, 'filter') and args.filter:
       ctest_cmd.extend(["-R", args.filter])
   
   print("Running tests with Undefined Behavior Sanitizer...")
   result = subprocess.run(ctest_cmd, cwd=build_dir)
   
   if result.returncode == 0:
       print("\nAll Undefined Behavior Sanitizer tests passed!")
   else:
       print("\nSome Undefined Behavior Sanitizer tests failed!")
   
   return result.returncode

def run_coverage_tests(args, project_root):
   """Run tests with coverage measurement"""
   print("Running tests with coverage measurement...")
   
   # Check if lcov is installed
   if not shutil.which("lcov") or not shutil.which("genhtml"):
       print("Error: lcov and genhtml are required for coverage reports")
       print("Please install: sudo apt-get install lcov")
       return 1
   
   # Create a build directory specifically for coverage
   build_dir = project_root / "build-coverage"
   os.makedirs(build_dir, exist_ok=True)
   
   # Configure with coverage enabled
   cmake_cmd = [
       "cmake", 
       "-B", str(build_dir),
       "-DCMAKE_BUILD_TYPE=Debug",
       "-DENABLE_COVERAGE=ON"
   ]
   
   print("Configuring build with coverage measurement...")
   result = subprocess.run(cmake_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to configure build with coverage measurement")
       return result.returncode
   
   # Build the project
   build_cmd = ["cmake", "--build", str(build_dir)]
   print("Building project with coverage instrumentation...")
   result = subprocess.run(build_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to build project with coverage instrumentation")
       return result.returncode
   
   # Run tests with verbose output
   ctest_cmd = ["ctest", "--test-dir", str(build_dir), "--verbose"]
   
   # Add test filter if specified
   if hasattr(args, 'filter') and args.filter:
       ctest_cmd.extend(["-R", args.filter])
   
   print("Running tests for coverage measurement...")
   result = subprocess.run(ctest_cmd, cwd=build_dir)
   if result.returncode != 0:
       print("\nSome tests failed during coverage measurement!")
       # Continue anyway to generate the report
   
   # Generate coverage report
   print("Generating coverage report...")
   
   # Create coverage directory
   coverage_dir = build_dir / "coverage"
   os.makedirs(coverage_dir, exist_ok=True)
   
   # Run lcov to collect coverage data
   lcov_cmd = [
       "lcov",
       "--directory", str(build_dir),
       "--capture",
       "--output-file", str(coverage_dir / "coverage.info")
   ]
   result = subprocess.run(lcov_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to collect coverage data")
       return result.returncode
   
   # Filter out system files and test files
   lcov_filter_cmd = [
       "lcov",
       "--remove", str(coverage_dir / "coverage.info"),
       "/usr/*",
       "*/tests/*",
       "*/googletest/*",
       "--output-file", str(coverage_dir / "coverage_filtered.info")
   ]
   result = subprocess.run(lcov_filter_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to filter coverage data")
       return result.returncode
   
   # Generate HTML report
   genhtml_cmd = [
       "genhtml",
       str(coverage_dir / "coverage_filtered.info"),
       "--output-directory", str(coverage_dir)
   ]
   result = subprocess.run(genhtml_cmd, cwd=project_root)
   if result.returncode != 0:
       print("Failed to generate HTML coverage report")
       return result.returncode
   
   # Print coverage summary
   print(f"\nCoverage report generated: {coverage_dir}/index.html")
   
   # Try to open the report in a browser
   try:
       import webbrowser
       webbrowser.open(f"file://{coverage_dir}/index.html")
       print("Coverage report opened in browser")
   except:
       pass
   
   return 0

def run_full_check(args, project_root):
   """Run comprehensive verification: tests, sanitizers, and static analysis"""
   print("Running full verification...")
   
   # Run normal tests
   print("\n=== Step 1: Running standard tests ===")
   test_args = argparse.Namespace(**vars(args))
   test_args.run_asan = False
   test_args.run_ubsan = False
   test_args.coverage = False
   test_args.full_check = False
   result = run_tests(test_args)
   if result != 0:
       print("Standard tests failed!")
       return result
   
   # Run Address Sanitizer tests
   print("\n=== Step 2: Running Address Sanitizer tests ===")
   asan_args = argparse.Namespace(**vars(args))
   asan_args.run_asan = True
   asan_args.run_ubsan = False
   asan_args.coverage = False
   asan_args.full_check = False
   result = run_asan_tests(asan_args, project_root)
   if result != 0:
       print("Address Sanitizer tests failed!")
       return result
   
   # Run Undefined Behavior Sanitizer tests
   print("\n=== Step 3: Running Undefined Behavior Sanitizer tests ===")
   ubsan_args = argparse.Namespace(**vars(args))
   ubsan_args.run_asan = False
   ubsan_args.run_ubsan = True
   ubsan_args.coverage = False
   ubsan_args.full_check = False
   result = run_ubsan_tests(ubsan_args, project_root)
   if result != 0:
       print("Undefined Behavior Sanitizer tests failed!")
       return result
   
   # Run static analysis
   print("\n=== Step 4: Running static analysis ===")
   analyze_module = __import__("devcmd.analyze", fromlist=["run_analysis"]).run_analysis
   analyze_args = argparse.Namespace(**vars(args))
   analyze_args.analyze = True
   result = analyze_module(analyze_args)
   if result != 0:
       print("Static analysis failed!")
       return result
   
   print("\n=== All checks passed! ===")
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

# Add this import for run_full_check
import argparse
