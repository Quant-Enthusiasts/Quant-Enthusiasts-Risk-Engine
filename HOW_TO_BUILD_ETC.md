# The new structure should be the basis for further development

We really need to separate the src from includes, the libs from the apps and the apps from tests.
We also need to separate the interfaces from the rest e.g. pybind here or excel or other.

There is no need to add so many flags at the moment as the code source is not really that complex and the compiler add some basic flags by default either you choose Release/Debug.

I did not touch the essence of the code as there are a lot because I did not want to render the shock bigger and you can develop with familiar code.

To build and install in a local folder

Linux/macOS:

```bash
cd cpp_engine
mkdir -p build
cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .
```

Windows (PowerShell):

```powershell
# From the repository root
cmake -S cpp_engine -B cpp_engine\build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build cpp_engine\build --config Release
cmake --install cpp_engine\build --config Release
```

Notes:
- pybind11 is fetched automatically during configure if it is not installed on your system.
- On Windows, avoid pasting multiline commands with prompts like ">>"; run each line separately or use the explicit -S/-B form shown above.

To run the tests (once built ofc) at the top level:

```bash
# Shell (Linux/macOS)
./run_tests.sh
python3 test_installed_module.py

# Windows PowerShell
./run_tests.sh
python .\test_installed_module.py
```

The Python test script automatically adds `cpp_engine/install/lib` to `PYTHONPATH` and imports the built `quant_risk_engine` module.
