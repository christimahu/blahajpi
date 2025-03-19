#!/usr/bin/env python3
"""
@file cli_runner.py
@brief End-to-end test for BlahajPI CLI
@ingroup tests
@defgroup e2e_tests End-to-End Tests

Contains end-to-end tests for the BlahajPI command-line interface,
verifying that commands execute correctly in a real environment.
"""

import os
import sys
import subprocess
import unittest

class CLITest(unittest.TestCase):
    """
    @brief Test suite for CLI end-to-end tests
    @ingroup e2e_tests
    
    Tests the BlahajPI CLI by executing actual commands and verifying
    their behavior.
    """
    
    def test_version_command(self):
        """
        @test
        @brief Test that the version command works
        @ingroup e2e_tests
        
        Verifies that the version command runs and displays version information.
        """
        result = subprocess.run(["./blahajpi", "version"], 
                               capture_output=True, text=True)
        self.assertEqual(result.returncode, 0)
        self.assertIn("Version:", result.stdout)
        
    def test_help_command(self):
        """
        @test
        @brief Test that the help command works
        @ingroup e2e_tests
        
        Verifies that the help command runs and displays help information.
        """
        result = subprocess.run(["./blahajpi", "help"], 
                               capture_output=True, text=True)
        self.assertEqual(result.returncode, 0)
        self.assertIn("Commands:", result.stdout)

if __name__ == "__main__":
    # Simple check to make sure we're in the right directory
    if not os.path.exists("./blahajpi") and not os.path.exists("./blahajpi.exe"):
        print("Error: CLI executable not found in current directory")
        print("Current directory:", os.getcwd())
        print("Files in directory:", os.listdir("."))
        sys.exit(1)
        
    unittest.main()
