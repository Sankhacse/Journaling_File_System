

```markdown
# 🛡️ J-FS: Crash-Consistent Journaling File System

![C](https://img.shields.io/badge/Language-C-blue.svg)
![FUSE](https://img.shields.io/badge/Tech-FUSE_3-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)
![Status](https://img.shields.io/badge/Status-Stable-brightgreen.svg)

**J-FS** is a custom, user-space virtual file system built using **FUSE (Filesystem in Userspace)**. It tackles one of the most fundamental challenges in Operating Systems: the **"Write Hole" problem**. By implementing a **Write-Ahead Logging (WAL)** protocol via Metadata Journaling, J-FS guarantees structural integrity and data consistency even in the event of an asynchronous power failure or system crash.

This project was developed as a comprehensive systems engineering demonstration, bridging the gap between theoretical OS concepts and practical low-level C programming.

---

## ✨ Key Features

* **Metadata-Only Journaling:** Achieves a high-performance balance by logging critical structural changes (Inodes, Bitmaps) without the massive I/O overhead of double-writing full data blocks.
* **FUSE Integration:** Fully POSIX-compliant. Standard Linux commands (`ls`, `cat`, `echo`, `mkdir`) work seamlessly out of the box.
* **Deterministic Fault Injection:** Built-in crash simulation allows users to trigger a hard system fault mid-transaction to prove the necessity of the journal.
* **Idempotent Recovery Engine:** An offline recovery tool that detects "hanging" transactions in the journal and safely replays them to restore the disk to a consistent state.
* **Custom Disk Geometry:** Operates on a virtual 10MB `disk.img` divided into distinct zones (Superblock, Bitmaps, Journal, Inodes, Data Blocks).

---

## 🏗️ Disk Layout Architecture

The 10MB virtual disk is partitioned into 4KB blocks:
1. **Block 0 (Superblock):** Magic number and geometry metadata.
2. **Blocks 1-2 (Bitmaps):** O(1) allocation tracking for Inodes and Data Blocks.
3. **Block 3 (The Journal):** The transactional safety zone.
4. **Blocks 4-19 (Inode Table):** Stores file metadata and pointers.
5. **Blocks 20+ (Data Blocks):** Actual payload storage.

---

## ⚙️ Prerequisites

You will need a Linux environment (Native or VM) with the FUSE 3 development headers installed.

```bash
# Install FUSE3 and GCC (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install gcc make libfuse3-dev pkg-config python3
```

---

## 🚀 Installation & Build

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/Sankhacse/Journaling_File_System.git](https://github.com/Sankhacse/Journaling_File_System.git)
   cd Journaling_File_System
   ```

2. **Compile the suite:**
   *(Assuming you have the Makefile set up. If not, compile manually below)*
   ```bash
   gcc mkfs.c -o mkfs
   gcc simple_fs.c -o simple_fs -D_FILE_OFFSET_BITS=64 `pkg-config fuse3 --cflags --libs`
   gcc recovery.c -o recovery
   ```

---

## 💻 Live Demo: The Crash & Recover Workflow

To truly see the journaling in action, you need **two terminal windows**.

### Step 1: Format & Mount (Terminal 1)
Format the virtual disk and start the FUSE driver in the foreground so you can watch the logs.

```bash
# Create the 10MB disk.img and write the Superblock
./mkfs

# Create a mount point
mkdir -p mnt

# Mount the filesystem (runs in foreground)
./simple_fs -f mnt/
```

### Step 2: Interact & Simulate Crash (Terminal 2)
Open a second terminal window, navigate to the project folder, and act as the user.

```bash
# Write a standard file
echo "Hello World, this is J-FS!" > mnt/hello.txt
cat mnt/hello.txt

# TRIGGER THE CRASH (Watch Terminal 1 instantly die)
echo "CRASH" > mnt/test.txt
```
*Note: Because of the crash, the journal wrote the intent, but the Inode table was never permanently updated!*

### Step 3: The Recovery (Terminal 1)
The file system is now technically "corrupted". Let's fix it.

```bash
# First, force-unmount the broken folder
fusermount3 -u mnt

# Run the Idempotent Recovery Engine
./recovery

# Remount the filesystem to verify
./simple_fs -f mnt/
```

### Step 4: Verify Success (Terminal 2)
```bash
# The system successfully rebuilt the Inode from the Journal!
cat mnt/test.txt
```

---

## 📊 Performance Evaluation

Journaling introduces a "performance tax" because of the extra I/O write to the journal block. However, by using **Metadata Journaling**, this latency is kept to a statistical minimum while guaranteeing 100% structural integrity.

---

## 🔮 Future Scope
* Implementation of **Indirect Pointers** to support file sizes larger than direct block limits.
* Addition of a **Full Data Journaling** mode toggle for mission-critical payloads.
* Journal circular buffering for sustained heavy I/O workloads.

---

## 👨‍💻 Author
**Sankha (Sankhacse)** *Computer Science & Engineering* | *IIT BHU* *Designed for educational purposes and systems programming exploration.*
```
