#!/usr/bin/env python3
"""
@file test_api.py
@brief End-to-end test for BlahajPI API
@ingroup e2e_tests

Contains end-to-end tests for the BlahajPI API functionality,
verifying that the API works correctly in a real environment.
"""

import os
import sys
import unittest
import subprocess
import json
import tempfile

class APITest(unittest.TestCase):
    """
    @brief Test suite for API end-to-end tests
    @ingroup e2e_tests
    
    Tests the BlahajPI API by executing commands and verifying
    their behavior with the executable.
    """
    
    def test_version_command_api(self):
        """
        @test
        @brief Test that the version command works as an API
        @ingroup e2e_tests
        
        Verifies that the version command runs through the API and
        returns appropriate information.
        """
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
        """
        @test
        @brief Test that the help command works as an API
        @ingroup e2e_tests
        
        Verifies that the help command runs through the API and
        returns appropriate information.
        """
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
