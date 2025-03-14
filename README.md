# Rift – Navigate the Flow. Control the Change.

Rift is a fast and flexible version control system that makes branching, tracking, and collaboration effortless. Stay in control of your code, experiment freely, and never lose progress.


Rift is a lightweight version control system inspired by Git. This repository contains the implementation of Rift, allowing users to track changes in their files.

## 📥 Cloning the Repository

To get started, clone this repository using the following command:

```sh
git clone https://github.com/DarkPhoenix616/Rift
```

After cloning, navigate into the project directory:

```sh
cd rift
```

## ⚙️ Building the Project

This project uses a `Makefile` to compile the source code. To build the executable, run:

```sh
make
```

This will generate the `Rift` executable in the project directory.

## 🚀 Running Rift

Once compiled, you can use Rift without the `./` prefix by moving it to `/usr/local/bin`:

```sh
sudo mv Rift /usr/local/bin/
```

Now you can run Rift from anywhere in the terminal!

## 📌 Initializing a Rift Repository

To start using Rift, initialize a repository in your project directory:

```sh
Rift init
```

## 📄 Available Commands

- `Rift init` - Initializes a new Rift repository.
- `Rift status` - Detects changes in the files and shows the status of your directory

## 🛠 Cleaning Up

To remove the compiled executable:

```sh
make clean
```

## 🤝 Contributing

Feel free to contribute by submitting issues or pull requests.

---


