from setuptools import setup, Extension
import pybind11

cpp_args = ['-std=c++17', '-Wall', '-pedantic']

ext_modules = [
    Extension(
        'quant_risk_engine',
        sources=[
            '../cpp_engine/apps/main.cpp',
            '../cpp_engine/libraries/python_interface/src/pybind_wrapper.cpp',
            '../cpp_engine/libraries/qe_risk_engine/src/Portfolio.cpp',
            '../cpp_engine/libraries/qe_risk_engine/src/RiskEngine.cpp',
            '../cpp_engine/libraries/qe_risk_engine/src/BlackScholes.cpp',
            '../cpp_engine/libraries/qe_risk_engine/src/BinomialTree.cpp',
            '../cpp_engine/libraries/qe_risk_engine/src/JumpDiffusion.cpp',
            '../cpp_engine/libraries/qe_risk_engine/src/ImpliedVolatilitySurface.cpp',
            '../cpp_engine/libraries/qe_risk_engine/src/MarketData.cpp',
            "../cpp_engine/libraries/qe_risk_engine/src/Instrument.cpp"
        ],
        include_dirs=[
            pybind11.get_include(),
            '../cpp_engine/src',
            '../cpp_engine/src/utils',
            '../cpp_engine/libraries/qe_risk_engine/src',
            '../cpp_engine/libraries/qe_risk_engine/src/utils',
            '../cpp_engine/libraries/qe_risk_engine/includes'
        ],
        language='c++',
        extra_compile_args=cpp_args,
    ),
]

setup(
    name='quant_risk_engine',
    version='3.0',
    description='Python bindings for the Quant Enthusiasts Risk Engine',
    author='Quant Enthusiasts',
    ext_modules=ext_modules,
    install_requires=[
        'pybind11>=2.6.0',
    ],
    zip_safe=False,
)