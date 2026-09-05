#!/usr/bin/env python3
import os
import re
import subprocess
import sys
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
        raise RuntimeError(
            f"Cannot find dictionary {ods}, check files: [{', '.join(map(str, ods_locations))}]"
        )

    eliottxt = next(
        (f for f in eliottxt_locations if f.is_file() and os.access(f, os.X_OK)),
        Path(""),
    )
    if not eliottxt.is_file():
        raise RuntimeError(
            f"Cannot find the text interface executable in [{', '.join(map(str, eliottxt_locations))}]"
        )

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
        raise RuntimeError(f"Cannot open the scenario list: {e}") from e


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
        print(f"--> Error: execution of scenario failed ({out})")
        return False

    # Compare output with expectation
    diff_cmd = ["diff", str(ref_file), str(run_file)]
    diff_result = subprocess.run(diff_cmd, capture_output=True, text=True, check=True)

    if diff := diff_result.stdout:
        print("--> Error: found differences:")
        print(diff, end="")
        return False

    return True


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
    print("\nSummary: ", end="")
    if not errors:
        print("Everything was OK.")
    else:
        print(f"{len(errors)} error(s). The following scenario(s) have failed:")
        print(" ".join(errors))
        sys.exit(1)


if __name__ == "__main__":
    main()
