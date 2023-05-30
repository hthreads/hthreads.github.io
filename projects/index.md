---
layout: page
title: Projects
has_children: true
---

# Projects
{:.no_toc}

## Table Of Contents
{:.no_toc .text-delta}

1. TOC
{:toc}

---
# Current Projects
## [PiCaSO](picaso)
Processor-in-Memory (PIM) architectures are highly suitable for data-intensive applications like machine learning.
While there exists several proposals for custom PIM blocks for FPGAs, they are not currently available in the commodity FPGAs.
PiCaSO is a fast and scalable Processor-in-Memory (PIM) overlay architecture.
PiCaSO serves as a viable solution for emulating PIM on existing FPGAs, delivering comparable performance to dedicated custom implementations.
<br>

Check out PiCaSO on <a href="https://github.com/Arafat-Kabir/PiCaSO" target="_blank">Github</a>.

# Old Projects
## Heterogeneous Extensible MultiProcessor Systems (HEMPS)
The goal of the Heterogeneous Extensible Multiprocessor System (HEMPS) is to provide a programming model for heterogeneous platforms containing mixes of processors, processors with statically attached accelerators, and processors with dynamic accelerators capable of online reconfiguration. As a result, HEMPS incorporates a runtime system that profiles and adaptively partitions a high-level application onto available heterogeneous resources.

## Synthesis Free Approach to Domain-Specific Accelerator Design
The world of software development has the notion of just-in-time compilation, run time binary translation, and language interpretation. There are no such equivalences to increase the productivity or portability of creating new hardware components. Creating a new hardware component requires running through a very time consuming synthesis process. Whereas even re-compiling code can be done in seconds, synthesis takes hours. As we move towards chip heterogeneous multiprocessor systems with more customized accelerator components we need a new flow that eliminates the need to synthesize for every change. We need a new flow that allows software as well as hardware developers to quickly assemble custom hardware accelerators using compilation and run time interpretation techniques. The fundamental question this project seeks to answer is: can software compilation or run time interpretation equivalences replace traditional hardware synthesis for creating custom accelerators?

## Self Assembly of MPSoPCs
When we talk about putting the ability to create a complete MPSoPC at your fingertips, we mean the whole ball of wax; a complete integrated hardware architecture and protocol stack! You can easily create SMP and NUMA Multiprocessor Systems on Chip Architectures using a menu-driven web interface in the Cloud. The produced MPSoPC contains Hthreads which is a Pthreads compliant run time system. You can write Pthreads programs using our compiler/linker, download onto you MPCoPC and run !

## Heterogeneous Multi- and Many-Core Systems
Creating heterogeneous SoCs in which each processor has uniform access to operating system services. This project leverages the existing HThreads architecture to allow heterogeneous processors (processors with different ISAs) to communicate and synchronize using a uniform set of OS APIs. Uniform sets of abstractions allow applications to be developed, compiled, and deployed on heterogeneous systems almost as easily as if they were a homogeneous system. Learn more about Heterogeneous_Threads.

## Cache Design for Solid-State Drives
A solid-state drive (SSD) is a data storage device that uses solid-state memory to store persistent data with the intention of providing access in the same manner of a traditional block i/o hard disk drive. The de facto memory used in SSDs currently is flash memory. SSDs are potentially more robust than hard disk drives due to the fact that there is no internal moving parts. Further it provides more data access parallelism since multiple flash packages can be accessed simultaneously.

However, the flash-based SSD has its own disadvantages as well. First, the write performance is much lower than the read performance. Second, each flash cell can sustain a limited number of writes. This project is intended to address these two issues by proposing an on-board cache for SSDs. Various cache algorithms are to be investigated. A new automatic periodic update (APU) feature is proposed. Results show that the on-board cache can significantly reduce the access latency to SSDs and improve the lifetime of the drives as well.

## Programming Models and Runtime Support for GPUs
Graphics Processing Units (GPUs) are ubiquitous in modern computers. Modern GPUs are very efficient at manipulating computer graphics, and their highly parallel structure makes them more effective than general-purpose CPUs for algorithms where processing of large blocks of data is done in parallel.

One of the biggest overhead to use GPUs is the data communication between host CPU and GPU. This overhead can be hidden by overlapping the computation and communication of different GPU contexts. A programming model is proposed to efficiently merge the kernel functions in a multithreaded application into a single context to improve the performance as well as the device occupancy.

## Hybridthreads
Hthreads is a computation platform built on top of standard field-programmable gate arrays (FPGAs). This platform is designed as a hardware/software co-designed operating system along with a set of APIs which provide access to the operating system to both hardware and software components.

The APIs of hthreads are uniform in policy for both software and hardware threads, although the interfaces used by the two type of threads differ in mechanism. The software thread interface (SWTI) is for use by software components. This interface is designed as a lightweight wrapper around the hardware-based Hthreads operating system and so provides very low-overhead operation. The hardware thread interface (HWTI) is for use by hardware components. This interface is designed in VHDL as a black box hardware component which is capable of communicating with the Hthreads operating system.

This design provides seamless integration between hardware components and software components because both the software interface and the hardware interface provide uniform access to the same operating system APIs. This functionality is provided at a very low cost to efficiency because of Hthreads unique hardware based operating system. Learn more about hybridthreads. Learn how to start using hybridthreads.

## FSMLanguage
FSMLanguage is a domain-specific language (DSL) for describing finite-state machines. The language was developed in order to create a way for programmers to develop re-targetable representations of FSMs. The FSMLanguage compiler is capable of producing both software- and hardware-implementations of FSMLanguage programs. Both implementation types remain compatible with one another as the communication abstractions that are built in to FSMLanguage are able to cross the hardware/software boundary. Learn more about FSMLanguage.

## Energy Efficiency of MPSoPCs
Designing new architecture for Hhthreads to improve the energy efficiency but not compromise any performance. To investigate this topic, the research on dynamic power based on Hthreads OS is start. Lean more about the Energy Efficiency of MPSoPCs .

## Hybridthread Compiler
The HybridThread compiler is designed to allow programmers to develop hardware/software co-designed systems from a single, high-level source language. Learn more about the hybridthread compiler.
Hog ARray Processor

We are interested in developing a co-processor that can handle SIMD operations without the need to develop a custom compiler. To achieve this function the SIMD instructions for the co-processor are a mix of MicroBlaze and custom instructions that are all generated by the standard MicroBlaze compiler in XST with some custom libraries.

## Application-Specific Embedded Manycore System Generation using OpenCL Using OpenCL
The capacity of Field Programmable Gate-Arrays (FPGAs) is approaching 1 million LUTs. With this capacity, it is possible to create a complete multiprocessor system on a single FPGA device. The traditional approach to designing a system-on-chip is to use the specific tools from the chip vendors. However, this approach generally requires hardware design expertise and lacks the portability. An alternative approach is to generate systems directly from applications written in high-level languages, such as OpenCL.

OpenCL is a framework to design parallel applications on multicore/manycore platforms. Both the data parallelism and the architectural hierarchy are explicitly exposed in an OpenCL application. This project focuses on (1) developing a complete flow to generate a multiprocessor system given an OpenCL application, and (2) investigating the high-level synthesis techniques to realize the optimal processor architecture and interconnect infrastructure of the system. 
