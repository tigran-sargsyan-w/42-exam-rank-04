# 42 – Exam Rank 04 Practice 🧪

> Practice solutions for **42 Exam Rank 04** in C (`ft_popen`, `picoshell`, `sandbox`, `argo`, `vbc`).
> Personal study repo, **not for cheating during the exam**.

---

## ⚠️ Disclaimer

This repository is **only for personal study and revision**:

* It is **not** an official 42 resource.
* It is **not** meant to be used during the actual exam.
* Please respect your campus rules and the spirit of the project.

Use it to **understand concepts**, re-implement exercises on your own, and build your muscle memory in C.

---

## 📂 Repository Structure

The repository is organized by exam level and assignment:

```text
exam-rank-04/
├── level-1
│   ├── ft_popen/
│   ├── picoshell/
│   └── sandbox/
└── level-2
    ├── argo/
    └── vbc/
```

Each directory contains my practice implementation and simple test code.

---

## 🧩 Exercises Overview

### level-1 / `ft_popen`

A minimal version of `popen`:

* Spawns a child process with `fork` + `execve` / `execvp`.
* Connects the child’s stdin/stdout to a pipe.
* Returns a file descriptor to read from or write to.
* Teaches pipes, file descriptors, and basic process control.

---

### level-1 / `picoshell`

A tiny shell pipeline executor:

* Executes a sequence of commands connected by pipes.
* Uses `pipe`, `fork`, `dup2`, `execvp`, `wait`.
* Goal: reproduce `cmd1 | cmd2 | ... | cmdN` behaviour in a minimal way.

---

### level-1 / `sandbox`

A small “sandbox” runner:

* Runs a user function / command with a time limit.
* Handles signals and timeouts.
* Practice for `fork`, `alarm` / timers, `waitpid`, and exit statuses.

---

### level-2 / `argo`

A lightweight JSON parser into a custom structure:

* Parses a limited subset of JSON (objects, arrays, strings, numbers, etc.).
* Builds a simple in-memory representation (AST / tree).
* Focuses on parsing, validation, and error handling.

---

### level-2 / `vbc`

A minimal expression evaluator:

* Parses and evaluates arithmetic expressions like:
  `1 + 2 * (3 + 4)`
* Handles operator precedence and parentheses.
* Good exercise for recursive descent parsing and integer arithmetic.

---

## 🛠️ Build & Run

All projects are built and run via the **root Makefile**.

From the repository root:

```bash
# Build all binaries (ft_popen, picoshell, sandbox, argo, vbc)
make

# Build a single project
make ft_popen
make picoshell
make sandbox
make argo
make vbc

# Build and run a single project
make run_ft_popen
make run_picoshell
make run_sandbox
make run_argo
make run_vbc

# Clean object files and binaries
make clean   # remove all .o files
make fclean  # remove .o files and all binaries
make re      # full rebuild (fclean + all)
```

> All commands above must be run **from the root directory** of the repository.

---

## 📖 How I Use This Repo

* Re-implement functions **from scratch** without copy-paste.
* Add small **test mains** for each exercise.
* Try to solve with **limited functions**, just like in the exam.
* Refactor until the code is **clean, readable and robust**.

If you are a 42 student:
Treat this repo as a **reference and practice ground**, not as something to memorize. The real skill for the exam is being able to write these functions again under pressure.

---
