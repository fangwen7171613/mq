# FAST MQ - High-Performance Lightweight Message Queue System

## Core Features

### Lightweight Design:
- The core program is only a few megabytes in size, with minimal memory usage, making it ideal for resource-constrained environments.

### Outstanding Performance:
- Tested on standard PC hardware (VMware + Arch Linux), achieving over 8.6 million TPS (transactions per second) (each request-response pair counts as one message). It excels in high-concurrency and high-throughput scenarios.

### Ultra-Low Latency:
- Designed for real-time scheduling, with very low latency, making it suitable for applications sensitive to delay, such as online gaming, financial transactions, and real-time communication.

### Peer-to-Peer Cluster & Scalability:
- Supports a peer-to-peer cluster architecture, with strong horizontal scalability and theoretically unlimited scheduling capacity. Built-in load balancing efficiently utilizes resources.

### Simple Deployment:
- Requires only the installation of `io_uring` (Linux Kernel v5.1+), with no need for any third-party dependencies. Quick configuration and ready to use right out of the box.

### Efficient I/O:
- `io_uring` significantly enhances asynchronous I/O performance, especially in high-load scenarios, delivering exceptional efficiency.

### Cost-Effective:
- Achieves enterprise-level performance on standard hardware. Testing environments show fast, stable message scheduling capabilities.

## Use Cases

- **Financial Systems:** High-throughput transaction processing, supports real-time settlement.
- **Big Data Pipelines:** Efficiently manages distributed data flows.
- **IoT and Edge Computing:** Lightweight design ideal for edge devices and IoT.
- **E-commerce and Logistics:** Reliable order processing and inventory management.
- **Online Gaming:** Ultra-low latency supports real-time player interactions, match services, and game synchronization.

## Why Choose FAST MQ?

- **Lightweight:** Low memory usage, high efficiency.
- **High Performance:** Achieves over 8.6 million TPS in a standard hardware environment.
- **Ultra-Low Latency:** Real-time scheduling ensures timely delivery.
- **Strong Scalability:** Supports peer-to-peer clusters and horizontal scaling.
- **Simple Deployment:** Requires only `io_uring` with no additional dependencies.

## Application Case: Online Gaming

FAST MQ performs exceptionally well in online gaming by supporting:
- **Real-Time Response:** Ensures smooth gameplay experience with quick response times.
- **High Concurrency:** Can handle thousands of players simultaneously.
- **Flexible Scalability:** Easily adapts to the rapid growth demands of the gaming ecosystem.

## 3. Deployment and Configuration

### 3.1 Set Environment Variables
In your development or production environment, set the following environment variables:

```bash
export WORK_ROOT=/path/to/work
export WORK_DATA_ROOT=/path/to/data

gmail: fangw717161@gmail.com

====================================================================================================================
FAST MQ - 高性能轻量级消息队列系统
核心特点
轻量级设计：
核心程序仅数兆字节，运行内存占用极小，非常适合资源有限的环境。
卓越性能：
在普通PC硬件（VMware + Arch Linux）上测试，TPS高达860万+（每次请求-应答算作一条消息）。
在高并发、高吞吐场景中表现出色。
超低时延：
专为实时调度设计，时延极低，适用于网络游戏、金融交易、实时通信等对延迟敏感的应用。
对等集群与扩展性：
支持对等集群架构，横向扩展能力强，调度能力理论上无限制。
内置负载均衡，高效利用资源。
简单部署：
仅需安装io_uring（Linux Kernel v5.1+），无需预装任何第三方依赖。
快速配置，即装即用。
高效I/O：
io_uring 显著提升异步I/O性能，特别是在高负载场景下表现突出。
经济实用：
在普通硬件上即可实现企业级性能，测试环境显示快速稳定的消息调度能力。
应用场景
金融系统：高吞吐量事务处理，支持实时结算。
大数据管道：高效管理分布式数据流。
物联网与边缘计算：轻量级设计，适用于边缘设备和物联网。
电商与物流：可靠的订单处理与库存管理。
网络游戏：超低时延满足玩家实时操作、匹配服务和游戏同步需求。
为什么选择 FAST MQ？
轻量级：内存占用低，运行高效。
高性能：普通硬件环境下实现 860万+ TPS。
超低时延：实时调度确保时效性。
强扩展性：支持对等集群和横向扩展。
简单部署：仅需 io_uring，无其他依赖。
应用案例：网络游戏
FAST MQ 在网络游戏中表现优异，通过超低时延和高吞吐量支持：
实时操作响应：提供流畅的游戏体验。
高并发处理：同时服务数千玩家。
灵活扩展：轻松满足游戏生态的快速增长需求。
gmail账号: fangw717161@gmail.com
微信号: fangwen596399

