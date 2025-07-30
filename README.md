EDI Parser – JSON Converter
This C++ program is designed to parse EDI (Electronic Data Interchange) files and convert their structured, delimiter-separated content into readable and usable JSON objects. The parser is built to handle EDI 837-like healthcare claims data, with robust segment and element-level mapping.

How It Works:
1. File Reading & Splitting:

    EDI lines are split by asterisks (*) using the splitAndStore function to extract individual elements.

2. Segment Identification:

    Each segment (e.g., ISA, GS, NM1) is identified and mapped based on its position and context (e.g., loop names like 2310A, 2010BA).

3. Data Mapping:

    The Map_Elements_General and Map_Elements_Loop functions associate raw EDI element positions (like NM109) with human-readable field names using a predefined map (DataElement_Name_Pairs).

4. Claim Type Customization:

    Handles both Professional and Institutional claims by applying context-specific mappings and flags (_I suffix).

5. JSON Output:

    The extracted key-value pairs are appended to or written into JSON files, organized by filename, using the map_to_json function with pretty printing.

6. Extensibility:

    The mapping logic is modular, allowing new segment identifiers and loops to be added easily without changing the core parsing logic.

Use Case:
This tool is ideal for converting raw EDI healthcare claims into structured formats for downstream data analysis, validation, auditing, or integration into modern systems like databases or web applications.
