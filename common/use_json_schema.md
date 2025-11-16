# ** Using JSON Schema to Define Zebra, fpmsyncd Messages **

# Purpose of a Data Schema
The main goal of adopting a data schema is to precisely specify the data exchanged between Zebra and fpmsyncd, and to automatically generate the corresponding C++ serialization and deserialization code. This approach eliminates errors commonly introduced by manual coding and ensures consistency across the system.

# Why JSON Schema?
JSON Schema is chosen because it offers a standardized, human-readable, and language-agnostic format for describing data structure, types, and validation rules. This facilitates reliable data validation and seamless code generation across heterogeneous components—specifically Zebra (typically C-based) and fpmsyncd (C++), while promoting interoperability. Furthermore, JSON Schema works well with modern development tooling, enabling automated code generation, runtime data validation, and clear, self-documenting interfaces between Zebra and fpmsyncd.

# Design Philosophy
The JSON schema defines the logical data model and serves as the single source of truth for the message format. However, C++-specific implementation details such as constructors, memory management, unions, and logging—are explicitly handled in handwritten code, as these concepts cannot be expressed in JSON Schema.

# Code Organization
To support this approach, three new directories have been introduced:

* schema/ – Contains the JSON schemas that define the data exchanged between Zebra and fpmsyncd.
* templates/ – Holds code generation templates used to produce C++ serialization/deserialization logic from the schemas.
* scripts/ – Includes render_schema.py, a script that processes the JSON schemas using the templates to generate the final C++ source files.