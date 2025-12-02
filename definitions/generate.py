#!/usr/bin/env python3

import sys
import json
from os import makedirs, environ, remove
from os.path import join, abspath, dirname, isdir, splitext, basename
from subprocess import run
from glob import glob
from jsonschema import validate
import yaml

# Paths
DEF_DIR = abspath(dirname(__file__))
ROOT_DIR = dirname(DEF_DIR)
DEF_SCHEMA = join(DEF_DIR, "definitions.schema.json")
VENV_DIR = join(DEF_DIR, ".venv")

CONNECTOR_DEST_DIR = join(ROOT_DIR, "connector", "connector", "generated")
DRIVER_DEST_DIR = join(ROOT_DIR, "driver", "lib", "generated")
PROTO_SRC = join(DEF_DIR, "communication.bitproto")

CLEANUP_WHITELIST = [
    ".gitignore",
    ".gitkeep",
    "README.md",
    "__init__.py",
]

KEY_FILE_DOC = "_doc_"
KEY_DEF_NAME = "name"
KEY_DEF_VALUE = "value"
KEY_DEF_C_TYPE = "c_type"
KEY_DEF_C_VALUE = "c_value"
KEY_DEF_COMMENT = "comment"

def generate():
    """Generate code from definition files."""
    print("# Reading definition files")

    # Read schema
    with open(DEF_SCHEMA, encoding="utf-8") as file:
        schema = json.load(file)

    # Detect all yaml files in the definitions directory
    def_files = glob("*.yaml", root_dir=DEF_DIR)
    definitions = []
    for file_name in def_files:
        print(" -", file_name)
        # Read yaml
        with open(file_name, encoding="utf-8") as file:
            data = yaml.load(file, Loader=yaml.SafeLoader)
            # Validate
            validate(instance=data, schema=schema)
            # Save
            definitions.append((file_name, data))

    # Driver
    print("# Preparing generated code directory for driver")
    if not isdir(DRIVER_DEST_DIR):
        makedirs(DRIVER_DEST_DIR)
    else:
        # Cleanup old generated files
        for file in [*glob("*.h", root_dir=DRIVER_DEST_DIR), *glob("*.c", root_dir=DRIVER_DEST_DIR)]:
            if file not in CLEANUP_WHITELIST:
                print(" - Removing old file:", file)
                remove(join(DRIVER_DEST_DIR, file))

    # TODO Cleanup old generated files

    print("# Generating serial protocol for driver")
    run_cmd(["bitproto", "c", PROTO_SRC, DRIVER_DEST_DIR, "-O"], cwd=DEF_DIR)

    print("# Generating definitions for driver")
    generate_def_files(definitions, DRIVER_DEST_DIR, ".h", generate_c_header, generate_c_comment, generate_c_definition, DYNAMIC_GENERATORS_C)

    # Connector
    print("# Preparing generated code directory for connector")
    if not isdir(CONNECTOR_DEST_DIR):
        makedirs(CONNECTOR_DEST_DIR)
    else:
        # Cleanup old generated files
        for file in glob("*.py", root_dir=CONNECTOR_DEST_DIR):
            if file not in CLEANUP_WHITELIST:
                print(" - Removing old file:", file)
                remove(join(CONNECTOR_DEST_DIR, file))

    print("# Generating serial protocol for connector")
    run_cmd(["bitproto", "py", PROTO_SRC, CONNECTOR_DEST_DIR], cwd=DEF_DIR)

    print("# Generating definitions for connector")
    generate_def_files(definitions, CONNECTOR_DEST_DIR, ".py", lambda _: None, generate_py_comment, generate_py_definition, DYNAMIC_GENERATORS_PY)

    print("# Done")


def generate_def_files(definitions, dest_dir, file_extension, generate_header, generate_comment, generate_definition, dynamic_generators: dict):
    """Generate target files from definition YAML files."""
    for name, data in definitions:
        base_name = splitext(basename(name))[0]
        out_file = join(dest_dir, base_name + file_extension)
        print(" -", out_file)
        # Write target file
        with open(out_file, "w", encoding="ascii") as output:
            generate_comment(output, f"Auto-generated from {basename(name)}")
            generate_comment(output, "DO NOT MODIFY!\n")

            if KEY_FILE_DOC in data:
                # out.write("\n") # New line before section comment
                if isinstance(data[KEY_FILE_DOC], list):
                    for line in data[KEY_FILE_DOC]:
                        generate_comment(output, line)
                else:
                    generate_comment(output, data[KEY_FILE_DOC])
                output.write("\n")

            generate_header(output)
            
            for key, value in data.items():
                if key == KEY_FILE_DOC: # Skip documentation entry
                    continue
                if isinstance(value, dict):
                    if KEY_DEF_NAME not in value:
                        generate_definition(output, {KEY_DEF_NAME: key, **value})
                    else:
                        generate_definition(output, value)
                elif isinstance(value, list):
                    fn_name = value[1]
                    if fn_name in dynamic_generators:
                        dynamic_generators[fn_name](output, data, key, value[2])
                    else:
                        raise ValueError(f"Unknown dynamic generator function '{fn_name}' in '{key}'")
                else:
                    generate_definition(output, {KEY_DEF_NAME: key, KEY_DEF_VALUE: value})


### C code generation ###      

def generate_c_header(out):
    """Generate C header preamble."""
    out.write("#pragma once\n")
    out.write("#include <Arduino.h>\n\n")

def generate_c_comment(out, line):
    """Generate C comment."""
    out.write("// " + line + "\n")

def generate_c_definition(out, data):
    """Generate C definition from data."""
    has_value = KEY_DEF_C_VALUE in data or KEY_DEF_VALUE in data

    if KEY_DEF_COMMENT in data:
        if not has_value:
            out.write("\n") # New line before section comment
            if isinstance(data[KEY_DEF_COMMENT], str): # Single line section comment
                generate_c_comment(out, data[KEY_DEF_COMMENT])
        if isinstance(data[KEY_DEF_COMMENT], list): # Multi-line comment (always before definition)
            for line in data[KEY_DEF_COMMENT]:
                generate_c_comment(out, line)
    
    if has_value:
        if KEY_DEF_C_VALUE in data:
            value = data[KEY_DEF_C_VALUE]
        else:
            value = data[KEY_DEF_VALUE]

        out.write("const ")
        if KEY_DEF_C_TYPE in data:
            out.write(data[KEY_DEF_C_TYPE] + " ")
        else:
            if isinstance(value, int):
                out.write("int ")
            elif isinstance(value, float):
                out.write("float ")
            elif isinstance(value, bool):
                out.write("bool ")
            elif isinstance(value, str):
                out.write("char* ")
            else:
                raise ValueError("Cannot deduce C type from value:", value)
        out.write(data[KEY_DEF_NAME].upper() + " = ")
        if isinstance(value, bool):
            out.write("true" if value else "false")
        elif isinstance(value, str) and KEY_DEF_C_TYPE in data:
            out.write(f"{value}") # Use as literal
        elif isinstance(value, str):
            out.write(f"\"{value}\"")
        else:
            out.write(str(value))

        out.write(";")
        if KEY_DEF_COMMENT in data and isinstance(data[KEY_DEF_COMMENT], str):
            out.write(" ")
            generate_c_comment(out, data[KEY_DEF_COMMENT])
        else:
            out.write("\n")


### Python code generation ###

def generate_py_comment(out, line):
    """Generate Python comment."""
    out.write("# " + line + "\n")

def generate_py_definition(out, data):
    """Generate Python definition from data."""

    if KEY_DEF_COMMENT in data:
        if KEY_DEF_VALUE not in data:
            out.write("\n") # New line before section comment
            if isinstance(data[KEY_DEF_COMMENT], str): # Single line section comment
                generate_py_comment(out, data[KEY_DEF_COMMENT])
        if isinstance(data[KEY_DEF_COMMENT], list): # Multi-line comment (always before definition)
            for line in data[KEY_DEF_COMMENT]:
                generate_py_comment(out, line)

    if KEY_DEF_VALUE in data:
        out.write(data[KEY_DEF_NAME].upper() + " = ")
        if isinstance(data[KEY_DEF_VALUE], str):
            out.write(f"\"{data[KEY_DEF_VALUE]}\"")
        else:
            out.write(str(data[KEY_DEF_VALUE]))

        if KEY_DEF_COMMENT in data and isinstance(data[KEY_DEF_COMMENT], str):
            out.write(" ")
            generate_py_comment(out, data[KEY_DEF_COMMENT])
        else:
            out.write("\n")


### Dynamic Generator Functions ###

def dyn_generate_array_mapping_c(out, defs: dict, def_key, param):
    out.write("const ")
    out.write(param[1].strip() + " ")
    out.write(def_key.upper() + "[] = {\n")

    # Collect entries
    entries = {}
    for key, value in defs.items():
        if key == def_key:
            continue
        value_key = key + param[0]
        if value_key in defs.keys():
            if not isinstance(value, int):
                raise ValueError(f"Cannot use non-integer value in key '{key}' for index in array mapping '{def_key}'")
            value_value = defs[value_key]
            if isinstance(value_value, list) or isinstance(value_value, dict):
                raise ValueError(f"Cannot use list or dict value in key '{value_key}' for value in array mapping '{def_key}'")
            entries[value] = value_value
    # Write entries
    max_index = max(entries.keys())
    for index in range(max_index + 1):
        out.write("   ")

        if index in entries:
            entry_value = entries[index]
            if isinstance(entry_value, bool):
                out.write("true" if entry_value else "false")
            elif isinstance(entry_value, str):
                out.write(f"\"{entry_value}\"")
            else:
                out.write(str(entry_value))
        else:
            out.write("NULL")

        if index < max_index:
            out.write(",")
        out.write("\n")

    out.write("};\n")


#TODO keep up to date
DYNAMIC_GENERATORS_C = {
    "array-mapping": dyn_generate_array_mapping_c
}

def dyn_generate_array_mapping_py(out, defs: dict, def_key, param):
    out.write(def_key.upper() + " = {\n")

    for key, value in defs.items():
        if key == def_key:
            continue
        value_key = key + param[0]
        if value_key in defs.keys():
            if not isinstance(value, int):
                raise ValueError(f"Cannot use non-integer value in key '{key}' for index in array mapping '{def_key}'")
            value_value = defs[value_key]
            if isinstance(value_value, list) or isinstance(value_value, dict):
                raise ValueError(f"Cannot use list or dict value in key '{value_key}' for value in array mapping '{def_key}'")
            out.write(f"    {str(value)}: ")
            if isinstance(value_value, str):
                out.write(f"\"{value_value}\"")
            else:
                out.write(str(value_value))
            out.write(",\n")
    out.write("}\n")


#TODO keep up to date
DYNAMIC_GENERATORS_PY = {
    "array-mapping": dyn_generate_array_mapping_py
}

### Util ###

def run_cmd(cmd, check=True, **kwargs):
    print(">> Running:", *cmd)
    ps = run(cmd, stdout=sys.stdout, stderr=sys.stderr, check=False, **kwargs, env=dict(environ, VIRTUAL_ENV=VENV_DIR, PATH=join(VENV_DIR, "bin") + ":" + environ["PATH"]))
    if ps.returncode != 0:
        print("Command returned non-zero exit status", ps.returncode)
        if check:
            print("Error!")
            sys.exit(-2)


### Main ###

if __name__ == "__main__":
    generate()
