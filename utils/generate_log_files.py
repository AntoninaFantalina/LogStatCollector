import os
import argparse
import json
import random

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--output_dir", type=str, required=True)
    parser.add_argument("--props_count", type=int, default=10)
    parser.add_argument("--files", type=int, default=1)
    parser.add_argument("--rows", type=int, default=1)
    args = parser.parse_args()

    timestamp = 1675514468
    fact_names = [f"fact{i}" for i in range(1, 16)]
    for i in range(1, args.files + 1):
        file_path = os.path.join(args.output_dir, f"file{i}.log")
        with open(file_path, "w") as file:
            print(f"Fill {file_path}")
            for j in range(0, args.rows):
                timestamp += 1
                props = {}
                for prop_id in range(1, args.props_count + 1):
                    props[f"prop{prop_id}"] = random.randrange(1, 10)
                action = {"ts_fact": timestamp,
                          "fact_name": random.choice(fact_names),
                          "actor_id": random.randrange(1001),
                          "props": props}
                file.write(json.dumps(action) + '\n')
