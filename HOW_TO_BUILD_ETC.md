# The new structure should be the basis for further development

We really need to separate the src from includes, the libs from the apps and the apps from tests.
We also need to separate the interfaces from the rest e.g. pybind here or excel or other.

There is no need to add so many flags at the moment as the code source is not really that complex and the compiler add some basic flags by default either you choose Release/Debug.

I did not touch the essence of the code as there are a lot because I did not want to render the shock bigger and you can develop with familiar code.

To build and install in a local folder

```bash
cd cpp_engine
mkdir build
cd build
cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
cmake --build .
cmake --install .
```

To run the tests (once built ofc) on the top level:

```bash
./run_test.sh
python3 test_installed_module.py
```
