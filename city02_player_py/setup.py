from setuptools import find_packages, setup

package_name = 'city02_player_py'

setup(
    name=package_name,
    version='0.1.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', ['launch/city02_player.launch.py']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='MA_LIO Migration',
    maintainer_email='dev@example.com',
    description='ROS2 City02 dataset player for MA_LIO migration.',
    license='BSD-3-Clause',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'city02_player_node = city02_player_py.city02_player_node:main',
        ],
    },
)
