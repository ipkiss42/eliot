#!/usr/bin/env python3
import difflib
import os
import re
import subprocess
import sys
from enum import Enum
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve()
TEST_ROOT_PATH = SCRIPT_PATH.parent
ROOT_PATH = TEST_ROOT_PATH.parent

DRIVER_FILE = TEST_ROOT_PATH / "driver"
TMP_DIR = Path("/tmp/eliot")
INPUT_EXT, REF_EXT, RUN_EXT = ".input", ".ref", ".run"


def locate_dependencies() -> tuple[Path, Path]:
    """Finds the dictionary and executable binaries, or exits on failure."""
    ods_locations = [
        Path.home() / "ods5.dawg",
        Path.home() / "dev/data/ods5.dawg",
        Path.home() / "Travail/eliot/data/ods5.dawg",
    ]

    eliottxt_locations = [
        ROOT_PATH / "utils/eliottxt",
        ROOT_PATH / "build/utils/eliottxt",
        ROOT_PATH / "linux/utils/eliottxt",
        ROOT_PATH / "win32/utils/eliottxt.exe",
    ]

    # Find first match using lazy generators
    ods = next((f for f in ods_locations if f.is_file()), Path(""))
    if not ods.is_file():
        locations = ", ".join(map(str, ods_locations))
        msg = f"Cannot find ods5.dawg at: [{locations}]"
        sys.exit(colorize(msg, Color.ERROR))

    eliottxt = next(
        (f for f in eliottxt_locations if f.is_file() and os.access(f, os.X_OK)),
        Path(""),
    )
    if not eliottxt.is_file():
        locations = ", ".join(map(str, eliottxt_locations))
        msg = f"Cannot find the text interface executable in [{locations}]"
        sys.exit(colorize(msg, Color.ERROR))

    return ods, eliottxt


def parse_driver(driver_path: Path) -> dict[str, str]:
    """Reads the test scenarios and their seeds from the driver file."""
    scenario_map: dict[str, str] = {}
    try:
        for raw_line in driver_path.read_text(encoding="utf-8").splitlines():
            line = re.sub(r"#.*", "", raw_line)

            if match := re.match(r"^\s*(\w+/\w+)\s+(\d+)\s*$", line):
                scenario, seed = match.groups()
                scenario_map[scenario] = seed

        return scenario_map
    except Exception as e:
        sys.exit(colorize(f"Cannot open the scenario list: {e}", Color.ERROR))


def select_scenarios(argv: list[str], all_scenarios: list[str]) -> list[str]:
    """Determines which scenarios to run based on command line arguments."""
    if not argv:
        return all_scenarios

    ext_pattern = "|".join(
        rf"{re.escape(ext)}$" for ext in (INPUT_EXT, REF_EXT, RUN_EXT)
    )
    return [re.sub(ext_pattern, "", item) for item in argv]


def run_scenario(scenario: str, eliottxt: Path, dic_path: Path, seed: str) -> bool:
    """Executes a single CLI scenario, sanitizes outputs, and diffs them against ref.

    Returns True if the scenario passed, False if an error occurred.
    """
    print(f"Scenario: {scenario}")
    input_file = TEST_ROOT_PATH / f"{scenario}{INPUT_EXT}"
    ref_file = TEST_ROOT_PATH / f"{scenario}{REF_EXT}"
    run_file = TEST_ROOT_PATH / f"{scenario}{RUN_EXT}"

    # Check that the needed files exist
    if not input_file.is_file():
        print(f"--> Error: missing file: {input_file}")
        return False
    if not ref_file.is_file():
        print(f"--> Error: missing file: {ref_file}")
        return False

    # Clean output from previous runs
    run_file.unlink(missing_ok=True)

    # Prepare command
    args = [str(eliottxt), str(dic_path)]
    if seed:
        args.append(seed)

    try:
        with (
            input_file.open("r", encoding="utf-8") as fin,
            run_file.open("w", encoding="utf-8") as fout,
        ):
            subprocess.run(
                args,
                stdin=fin,
                stdout=fout,
                stderr=subprocess.STDOUT,
                text=True,
                check=True,
            )
        out = ""
    except Exception as e:
        out = str(e)

    if out != "":
        print(colorize(f"--> Error: execution of scenario failed ({out})", Color.ERROR))
        return False

    return check_and_display_diff(ref_file, run_file)


def check_and_display_diff(ref_file: Path, run_file: Path) -> bool:
    """Compares the run output against expectations. Prints a colorized diff if differences exist."""
    ref_lines = ref_file.read_text(encoding="utf-8").splitlines()
    run_lines = run_file.read_text(encoding="utf-8").splitlines()

    diff_lines = list(
        difflib.unified_diff(
            ref_lines,
            run_lines,
            fromfile=str(ref_file),
            tofile=str(run_file),
            lineterm="",
        )
    )
    if not diff_lines:
        return True

    print(colorize("--> Error: found differences:", Color.ERROR))
    for line in diff_lines:
        if line.startswith(("---", "+++")):
            print(colorize(line, Color.MAGENTA))
        elif line.startswith("-"):
            print(colorize(line, Color.RED))
        elif line.startswith("+"):
            print(colorize(line, Color.GREEN))
        elif line.startswith("@@"):
            print(colorize(line, Color.CYAN))
        else:
            print(colorize(line, Color.DIM))

    return False


def main() -> None:
    os.chdir(TEST_ROOT_PATH)
    dic_path, eliottxt = locate_dependencies()

    # Parse configurations
    scenario_map = parse_driver(DRIVER_FILE)
    scenarios_to_play = select_scenarios(sys.argv[1:], list(scenario_map))

    # Run scenarios
    errors: list[str] = []
    for scenario in scenarios_to_play:
        seed = scenario_map.get(scenario, "")
        success = run_scenario(scenario, eliottxt, dic_path, seed)
        if not success:
            errors.append(scenario)

    # Display the results
    if not errors:
        print(colorize("Everything was OK.", Color.SUCCESS))
    else:
        print(
            colorize(
                f"{len(errors)} error(s). The following scenario(s) have failed:",
                Color.ERROR,
            )
        )
        print(" ".join(errors))
        sys.exit(1)


class Color(Enum):
    ERROR = "\033[38;5;196m"
    SUCCESS = "\033[38;5;40m"

    RED = "\033[38;5;124m"
    GREEN = "\033[38;5;28m"
    CYAN = "\033[38;5;44m"
    MAGENTA = "\033[38;5;13m"
    #     ORANGE = "\033[38;5;208m"
    DIM = "\033[38;5;244m"
    RESET = "\033[0m"


def colorize(text: str, color: Color) -> str:
    """Wraps text in a 256-color ANSI code and automatically appends a reset."""
    return f"{color.value}{text}{Color.RESET.value}"


if __name__ == "__main__":
    main()
