# GLoSSRt


## Building to publish

```
pip install ninja

pip install --upgrade setuptools wheel twine

python setup.py sdist bdist_wheel

pip install twine build
python -m build

twine upload --config-file .pypirc --repository testpypi dist/*
twine upload --verbose --config-file ./.pypirc --repository testpypi dist/*


```