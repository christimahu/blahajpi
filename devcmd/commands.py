"""
commands.py - Command and option definitions for Blahaj PI development tool
"""

# Command definitions
COMMANDS = {
    "build": {
        "group": "Actions",
        "help": "Build the project",
        "action": "store_true",
        "module": "build",
        "function": "run_build"
    },
    "test": {
        "group": "Actions",
        "help": "Run tests",
        "action": "store_true",
        "module": "test",
        "function": "run_tests"
    },
    "run": {
        "group": "Actions",
        "help": "Run Blahaj PI CLI",
        "action": "store_true",
        "module": "run",
        "function": "run_cli"
    },
    "demo": {
        "group": "Actions",
        "help": "Run demo",
        "action": "store_true",
        "module": "demo",
        "function": "run_demo"
    },
    "clean": {
        "group": "Actions",
        "help": "Clean build directory",
        "action": "store_true",
        "module": "build",
        "function": "clean_build"
    },
    "docs": {
        "group": "Actions",
        "help": "Generate documentation",
        "action": "store_true",
        "module": "docs",
        "function": "generate_docs"
    },
    "analyze": {
        "group": "Actions",
        "help": "Run static analysis",
        "action": "store_true",
        "module": "analyze",
        "function": "run_analysis"
    },
    "format": {
        "group": "Actions",
        "help": "Format code with clang-format",
        "action": "store_true",
        "module": "analyze",
        "function": "format_code"
    }
}

# Command options
OPTIONS = {
    "build": [
        {"name": "debug", "help": "Build in debug mode", "action": "store_true"},
        {"name": "verbose", "help": "Enable verbose output", "action": "store_true"},
        {"name": "jobs", "flags": ["-j"], "help": "Parallel jobs (0=auto)", "type": int, "default": 0}
    ],
    "test": [
        {"name": "unit", "help": "Run only unit tests", "action": "store_true"},
        {"name": "integration", "help": "Run only integration tests", "action": "store_true"},
        {"name": "e2e", "help": "Run only end-to-end tests", "action": "store_true"},
        {"name": "filter", "help": "Filter tests by name"},
        {"name": "run-asan", "help": "Run with Address Sanitizer", "action": "store_true"},
        {"name": "run-ubsan", "help": "Run with Undefined Behavior Sanitizer", "action": "store_true"},
        {"name": "coverage", "help": "Generate test coverage report", "action": "store_true"},
        {"name": "full-check", "help": "Run all tests, sanitizers, and static analysis", "action": "store_true"}
    ],
    "run": [
        {"name": "config", "help": "Configuration to use"}
    ],
    "docs": [
        {"name": "open-docs", "help": "Open docs in browser", "action": "store_true"}
    ]
}
