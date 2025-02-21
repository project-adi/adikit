# adikit

ADIKit is a toolkit for creating driver based on the [Adaptive Driver Interface (ADI)](https://github.com/project-adi). ADIKit is there to create driver projects and build them. Below are instructions on how to install and use ADIKit.

## How to use ADIKit

### Installation

To Install ADIKit you need the following dependencies:

- A C compiler + linker
- `libelf-dev`

---

To get started first download the project from Github:

```bash
git clone --recursive https://github.com/project-adi/adikit
```

After that to build and install it run

```bash
sudo make install
```

You can verify the installation by running `adikit --version` you should see something simmilar to this:

```text
adikit (0.1.1 Linux 5.15.167.4-microsoft-standard-WSL2-x86_64) 0.1.1
Copyright (C) 2025 Adaptive Driver Interface Project.
This work has been released under CC0 1.0 Universal (Public Domain Dedication). You may copy, modify, and distribute it without any restrictions.

This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

Now you successfully installed ADIKit!

### Usage

For general help you can always run `adikit --help`

#### Create a new driver project

To create a new driver project run

```bash
adikit create
```

Now fill out the questions and youre done!

#### Build a driver

To build a driver project run the following where `<driver directory>` is the direcory to the driver project

```bash
adikit build <driver directory>
```

Now in the specified directory there should be an `output.adi` that's your driver!
