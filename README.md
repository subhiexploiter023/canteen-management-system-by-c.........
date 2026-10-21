# 🍔 Advanced Canteen Management System (CMS)

> An enterprise-grade, console-based Canteen Management System engineered in **Standard C (C99)**. This project bridges foundational **Data Structures and Algorithms (DSA)** with persistent binary file management, robust error handling, and clean modular architecture.

![C Language](https://img.shields.io/badge/Language-C99-blue.svg?style=flat-square&logo=c)
![DSA Core](https://img.shields.io/badge/DSA-Linked%20List%20|%20Queue%20|%20Stack-success.svg?style=flat-square)
![Persistence](https://img.shields.io/badge/Storage-Binary%20Files-orange.svg?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-purple.svg?style=flat-square)

---

## 🏗️ System Architecture & Workflow

The application follows a layered modular design pattern, cleanly separating the user interface, core data structure operations, and storage persistence layers.

```text
 ┌────────────────────────────────────────────────────────┐
 │                      Presentation Layer                │
 │               (Customer Menu & Admin Panel)            │
 └───────────────────────────┬────────────────────────────┘
                             │
 ┌───────────────────────────▼────────────────────────────┐
 │                  Core DSA Logic Layer                  │
 │   ┌───────────────┬───────────────┬────────────────┐   │
 │   │ Singly Linked │     Queue     │     Stack      │   │
 │   │  List (Menu)  │ (FIFO Orders) │ (LIFO Cancels) │   │
 │   └───────┬───────┴───────┬───────┴────────┬───────┘   │
 └───────────┼───────────────┼────────────────┼───────────┘
             │               │                │
 ┌───────────▼───────────────▼────────────────▼───────────┐
 │                Persistence Layer (File I/O)            │
 │             (`menu.dat` & `orders.dat`)                │
 └────────────────────────────────────────────────────────┘
