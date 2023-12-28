import argparse
import json

def convert_value(original_value, new_value_str):
    try:
        if isinstance(original_value, int):
            return int(new_value_str)
        elif isinstance(original_value, float):
            return float(new_value_str)
        elif isinstance(original_value, str):
            return new_value_str
        elif isinstance(original_value, bool):
            return new_value_str.lower() == 'true'
        else:
            raise ValueError("Unsupported type")
    except ValueError as e:
        raise ValueError(f"Type conversion error: {e}")

def update_json_value(json_data, path, new_value):
    keys = path.split('.')
    ref = json_data
    for key in keys[:-1]:
        if key not in ref:
            return False, "Path not found"  # パスが存在しない
        ref = ref[key]

    if keys[-1] not in ref:
        return False, "Key not found"  # 最終キーが存在しない

    try:
        ref[keys[-1]] = convert_value(ref[keys[-1]], new_value)
        return True, "Success"
    except ValueError as e:
        return False, f"Type conversion error: {e}"

def main():
    parser = argparse.ArgumentParser(description="Update a value in a JSON file.")
    parser.add_argument("json_file", help="Path to the JSON file")
    parser.add_argument("path", help="Dot-separated path to the element to update")
    parser.add_argument("new_value", help="New value for the specified element")
    args = parser.parse_args()

    with open(args.json_file, 'r') as file:
        data = json.load(file)

    success, message = update_json_value(data, args.path, args.new_value)

    if not success:
        print(f"Error: {message}. Path: '{args.path}'")
        return

    with open(args.json_file, 'w') as file:
        json.dump(data, file, indent=4)

if __name__ == "__main__":
    main()
