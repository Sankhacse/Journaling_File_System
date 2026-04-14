# 🧾 Journaling File System (FUSE-Based)

A high-level systems engineering project that implements a **Journaling File System** for Linux using the **FUSE (Filesystem in Userspace)** framework.

This project demonstrates how modern file systems ensure **consistency, atomicity, and crash recovery** using **Write-Ahead Logging (WAL)**.

---

## 🚨 Problem Addressed: Write Hole

During a system crash or power failure, partial writes can leave the filesystem in an **inconsistent state** — this is known as the **Write Hole problem**.

### ✅ Solution Implemented

This filesystem uses **Metadata Journaling**, ensuring that:

* Critical metadata (Inodes, Bitmaps) is always recoverable
* Incomplete operations are safely replayed
* Filesystem integrity is preserved after crashes

---

## ⚙️ Core Concept: Write-Ahead Logging (WAL)

Instead of writing directly to disk:

1. 📝 **Log the intent** in a Journal Block
2. 💾 Perform actual metadata updates
3. 🔁 On crash → replay journal

This guarantees:

* **Atomicity** (all or nothing updates)
* **Durability** (data survives crashes)

---

## 💽 Disk Architecture

The filesystem emulates a **10MB disk (`disk.img`)** divided into **4KB blocks**.

[
10 \text{ MB} = 10 \times 1024 \times 1024 = 10,485,760 \text{ bytes}
]

Total blocks = **2560**

### 📊 Layout

| Zone        | Block Range | Description                        |
| ----------- | ----------- | ---------------------------------- |
| Superblock  | 0           | Filesystem metadata & magic number |
| Bitmaps     | 1–2         | Tracks free/used blocks & inodes   |
| Journal     | 3           | Write-ahead log for atomic updates |
| Inode Table | 4–19        | File metadata (size, pointers)     |
| Data Blocks | 20+         | Actual file content                |

---

## 🧩 Project Components

| File              | Description                                               |
| ----------------- | --------------------------------------------------------- |
| `mkfs.c`          | Initializes disk image with superblock & empty structures |
| `simple_fs.c`     | Main FUSE filesystem driver                               |
| `recovery.c`      | Replays journal after crash                               |
| `inspect.c`       | Debugging tool to inspect disk                            |
| `fs_structures.h` | Defines core filesystem structures                        |

---

## 🛠️ Build & Run Instructions

### 🔹 Phase 1: Compilation

```bash
gcc mkfs.c -o mkfs
gcc simple_fs.c -o simple_fs -D_FILE_OFFSET_BITS=64 `pkg-config fuse3 --cflags --libs`
gcc recovery.c -o recovery
gcc inspect.c -o inspect
```

---

### 🔹 Phase 2: Initialize & Mount

```bash
./mkfs
mkdir -p mnt
./simple_fs -f mnt/
```

---

### 🔹 Phase 3: Test + Crash Simulation

Open another terminal:

```bash
ls -l mnt/

echo "Success test" > mnt/hello.txt
cat mnt/hello.txt

# Trigger crash
echo "CRASH" > mnt/crash_test.txt
```

💥 This simulates a failure **after journaling but before metadata commit**

---

### 🔹 Phase 4: Recovery

```bash
fusermount3 -u mnt
./recovery
./simple_fs -f mnt/
```

---

### 🔹 Phase 5: Verification

```bash
ls -l mnt/
cat mnt/crash_test.txt
```

✅ If file exists → journaling worked successfully

---

## 🔬 Key Features

* ✔️ Custom Linux filesystem using FUSE
* ✔️ Metadata Journaling (WAL)
* ✔️ Crash simulation & recovery
* ✔️ Block-level disk emulation
* ✔️ Atomic transaction handling

---

## 🧠 Learning Outcomes

This project demonstrates:

* Filesystem internals (Inodes, Bitmaps, Blocks)
* Journaling techniques used in modern FS (ext4, NTFS)
* Crash consistency mechanisms
* Systems-level debugging and design

---

## 🚀 Future Improvements

* Data journaling (not just metadata)
* Multi-block transactions
* Performance optimizations
* GUI-based disk visualizer

---

## 📌 Conclusion

This project provides a complete, hands-on demonstration of:

👉 **Atomicity + Durability in storage systems**

It bridges theoretical OS concepts with real-world filesystem design using Linux and FUSE.

---

