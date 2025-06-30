from setuptools import setup, Extension
import pybind11

ext_modules = [
    Extension(
        "comunicacion",
        ["bindings.cpp", "tcp_client.cpp"],
        include_dirs=[pybind11.get_include()],
        language="c++"
    )
]

setup(
    name="comunicacion",
    ext_modules=ext_modules,
    python_requires=">=3.6",
)