#!/usr/bin/env python3
# render_schema.py
import json
import sys
import jinja2
import os

def main():
    if len(sys.argv) != 4:
        print("Usage: render_schema.py <template.j2> <schema.json> <output_file>")
        sys.exit(1)

    template_file = sys.argv[1]
    schema_file = sys.argv[2]
    output_file = sys.argv[3]

    with open(schema_file) as f:
        schema = json.load(f)

    template_dir = os.path.dirname(template_file)
    template_name = os.path.basename(template_file)

    env = jinja2.Environment(loader=jinja2.FileSystemLoader(template_dir))
    template = env.get_template(template_name)

    output = template.render(schema=schema)

    with open(output_file, 'w') as f:
        f.write(output)

if __name__ == '__main__':
    main()