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
    def test_analyze_command_api(self):
        """Test that the analyze command works as an API"""
        # Create a temporary file with test content
        with tempfile.NamedTemporaryFile(mode='w', delete=False) as tmp:
            tmp.write("This is a test message.")
            tmp_path = tmp.name
            
        try:
            # Run the analyze command
            result = subprocess.run(
                ["./blahajpi", "analyze", "--file", tmp_path],
                capture_output=True, text=True
            )
            
            # Check that it ran successfully
            self.assertEqual(result.returncode, 0)
            self.assertIn("Analysis Result", result.stdout)
            
        finally:
            # Clean up
            if os.path.exists(tmp_path):
                os.unlink(tmp_path)
                
    def test_config_command_api(self):
        """Test that the config command works as an API"""
        result = subprocess.run(
            ["./blahajpi", "config", "list"],
            capture_output=True, text=True
        )
        
        # Check that it ran successfully
        self.assertEqual(result.returncode, 0)
        self.assertIn("Configuration", result.stdout)

if __name__ == "__main__":
    # Simple check to make sure we're in the right directory
    if not os.path.exists("./blahajpi") and not os.path.exists("./blahajpi.exe"):
        print("Error: CLI executable not found in current directory")
        print("Current directory:", os.getcwd())
        print("Files in directory:", os.listdir("."))
        sys.exit(1)
        
    unittest.main()
