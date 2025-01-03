# FAST MQ Project - Secondary Development Environment Setup

## 1. System Requirements

### 1.1 Operating System
- **Linux** (Recommended: Ubuntu or CentOS)
- Supports x64-linux architecture.

### 1.2 Software Dependencies
- This project does not depend on any additional software.
- Supports server-side cluster deployment and load balancing for high-performance environments.

## 2. High Performance Requirements

### 2.1 High Concurrency and Performance Processing
- Each forwarder node can handle millions of scheduling tasks, offering high concurrency and performance.

### 2.2 Cluster Support
- Supports peer-to-peer cluster deployment, with theoretically unlimited scheduling capacity.
- Ideal for large-scale data flows and distributed environments.

### 2.3 Single-node Scheduling Capability
- A single forwarder node is capable of handling millions of scheduling tasks.

### 2.4 Load Balancing
- Supports server-side cluster and load balancing deployment for scalable systems.

### 2.5 I/O Performance (New in v5.1)
- Support for **io_uring**: From version 5.1 onward, io_uring is supported for high-performance I/O operations on Linux.
- This significantly improves asynchronous I/O performance, especially under heavy workloads.

## 3. Deployment and Configuration

### 3.1 Set Environment Variables
In your development or production environment, set the following environment variables:

```bash
export WORK_ROOT=/path/to/work
export WORK_DATA_ROOT=/path/to/data
