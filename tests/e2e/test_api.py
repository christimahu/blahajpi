#!/usr/bin/env python3
"""
test_api.py - End-to-end test for BlahajPI API
"""

import os
import sys
import unittest
import subprocess
import json
import tempfile

class APITest(unittest.TestCase):
    def test_version_command_api(self):
        """Test that the version command works as an API"""
        # Just check if executable exists
        blahajpi_exe = "./blahajpi"
        if os.name == 'nt':  # Windows
            blahajpi_exe = "./blahajpi.exe"
            
        if not os.path.exists(blahajpi_exe):
            self.skipTest(f"CLI executable not found at {blahajpi_exe}")
        
        # Run the version command (should always work)
        result = subprocess.run(
            [blahajpi_exe, "version"],
            capture_output=True, text=True
        )
        
        # Check that it ran successfully
        self.assertEqual(result.returncode, 0)
        self.assertIn("Version", result.stdout)

    def test_help_command_api(self):
        """Test that the help command works as an API"""
        # Just check if executable exists
        blahajpi_exe = "./blahajpi"
        if os.name == 'nt':  # Windows
            blahajpi_exe = "./blahajpi.exe"
            
        if not os.path.exists(blahajpi_exe):
            self.skipTest(f"CLI executable not found at {blahajpi_exe}")
        
        # Run the help command (should always work)
        result = subprocess.run(
            [blahajpi_exe, "help"],
            capture_output=True, text=True
        )
        
        # Check that it ran successfully
        self.assertEqual(result.returncode, 0)
        self.assertIn("Usage", result.stdout)

if __name__ == "__main__":
    unittest.main()
