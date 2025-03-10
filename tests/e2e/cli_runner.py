#!/usr/bin/env python3
"""
cli_runner.py - End-to-end test for BlahajPI CLI
"""

import os
import sys
import subprocess
import unittest

class CLITest(unittest.TestCase):
    def test_version_command(self):
        """Test that the version command works"""
        result = subprocess.run(["./blahajpi", "version"], 
                               capture_output=True, text=True)
        self.assertEqual(result.returncode, 0)
        self.assertIn("Version:", result.stdout)
        
    def test_help_command(self):
        """Test that the help command works"""
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
