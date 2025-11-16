#!/usr/bin/env python3

import json
import sys
import os
from jinja2 import Environment, FileSystemLoader


def json_type_to_cpp(prop, defs):
    """Map JSON Schema property to C++ type."""
    if "$ref" in prop:
        ref = prop["$ref"]
        if ref == "#/$defs/ip_address":
            return "union g_addr"
        elif ref == "#/$defs/uint8":
            return "std::uint8_t"
        elif ref == "#/$defs/in_address":
            return "struct in_addr"
        elif ref == "#/$defs/in6_address":
            return "struct in6_addr"
        elif ref.startswith("#/$defs/"):
            typename = ref.split("/")[-1]
            target = defs.get(typename, {})
            if target.get("type") == "string" and "enum" in target:
                return f"enum {typename}"
            else:
                return f"struct {typename}"
        else:
            return "void*"

    typ = prop.get("type")
    if typ == "integer":
        return "std::uint32_t" if prop.get("minimum", 0) >= 0 else "std::int32_t"
    elif typ == "string":
        return "std::string"
    elif typ == "array":
        item_type = json_type_to_cpp(prop.get("items", {}), defs)
        if "minItems" in prop and prop.get("minItems", 0) == 0:
            return  item_type
        return f"std::vector<{item_type}>"
    elif typ == "boolean":
        return "bool"
    elif typ == "null":
        return "std::nullptr_t"
    return "void"


def extract_enums(defs):
    """Extract enums from $defs."""
    enums = {}
    for name, schema in defs.items():
        if schema.get("type") == "string" and "enum" in schema:
            enums[name] = schema["enum"]
    return enums


def build_root_struct(schema, defs):
    """Build root struct from top-level properties."""
    fields = []
    for name, prop in schema.get("properties", {}).items():
        cpp_type = json_type_to_cpp(prop, defs)
        data = {"name": name, "cpp_type": cpp_type}
        position = prop.get("position", 0)
        data["position"] = position
        if "default_value" in prop:
            dvalue = prop.get("default_value")
            data["default_value"] = dvalue
        if "data_prefix" in prop:
            dvalue = prop.get("data_prefix")
            data["data_prefix"] = dvalue
        fields.append(data)
    name = schema.get("title", "NextHopGroupFull")
    return {"name": name, "fields": fields}


def build_def_structs(defs):
    """Build structs from $defs."""
    structs = {}
    for name, schema in defs.items():
        if schema.get("type") == "object":
            fields = []
            for fname, fprop in schema.get("properties", {}).items():
                cpp_type = json_type_to_cpp(fprop, defs)
                if name == "nexthop_srv6" and fname == "seg6_segs":
                    cpp_type = "struct seg6_seg_stack*"
                if fprop.get("type") == "array" and fprop.get("minItems", 0) == 0:
                    fields.append({"name": fname, "cpp_type": cpp_type, "zeroarray": True})
                else:
                    fields.append({"name": fname, "cpp_type": cpp_type})
            structs[name] = {"name": name, "fields": fields}
    return structs


def main():
    if len(sys.argv) != 5:
        print("Usage: ./render_schema.py <schema.json> <template_dir> <output_file> <mode>")
        print("  mode: 'header', 'source', or 'json_bindings'")
        sys.exit(1)

    schema_path = sys.argv[1]
    template_dir = sys.argv[2]
    output_path = sys.argv[3]
    mode = sys.argv[4]

    if mode not in ("header", "source", "json_bindings"):
        print("Error: mode must be 'header', 'source', or 'json_bindings'")
        sys.exit(1)

    # Load and parse schema
    with open(schema_path, 'r') as f:
        schema = json.load(f)

    defs = schema.get("$defs", {})
    enums = extract_enums(defs)
    root_struct = build_root_struct(schema, defs)
    def_structs = build_def_structs(defs)

    all_structs = def_structs.copy()
    all_structs[root_struct["name"]] = root_struct

    special_structs = {"nexthop_srv6", "seg6_seg_stack", root_struct["name"]}
    root_struct_name = root_struct["name"]

    # Jinja setup
    env = Environment(loader=FileSystemLoader(template_dir))
    template_name = None

    if mode == "header":
        template_name = "nexthopgroupfull.h.j2"
        context = {
            "enums": enums,
            "structs": all_structs,
            "special_structs": special_structs,
            "root_struct_name": root_struct_name
        }
    elif mode == "source":
        template_name = "nexthopgroupfull.cpp.j2"
        context = {
            "root_struct_name": root_struct_name
        }
    elif mode == "json_bindings":
        template_name = "nexthopgroupfull_json.h.j2"
        context = {
            "enums": enums,  # dict: name -> list of strings (e.g., ["NEXTHOP_TYPE_INVALID", ...])
            "root_struct_name": root_struct_name
        }

    # Render
    template = env.get_template(template_name)
    output = template.render(**context)

    # Write
    os.makedirs(os.path.dirname(output_path) if os.path.dirname(output_path) else ".", exist_ok=True)
    with open(output_path, 'w') as f:
        f.write(output)

    print(f"✅ Generated {output_path} (mode: {mode})")


if __name__ == '__main__':
    main()