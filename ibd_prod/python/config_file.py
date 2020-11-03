"Utilities for handling the SelectorFramework config file format"

import os


class ConfigFile(dict):
    "Wrapper with a dict interface"

    def __init__(self, path):
        super().__init__()
        self._read_from(path)

    def _read_from(self, path):
        for line in open(path):
            line = line.strip()

            if (not line) or line[0] == '#':
                continue

            key, val = line.split(maxsplit=1)

            try:
                val = int(val)
            except ValueError:
                try:
                    val = float(val)
                except ValueError:
                    pass

            self[key] = val

    def write(self, path):
        with open(path, 'w') as f:
            for key, val in self.items():
                f.write(f"{key} {val}\n")


def template_path():
    home = os.getenv("IBDSEL_HOME")
    return os.path.join(home, "static/configs/config.nominal.txt")

