/*
 * Copyright (C) 2022 2666680 Ontario Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*! \mainpage GPU Virtual Machine user suite (GVM-user)

  This page makes use of terminology from the
  <a href="https://openmdev.io/index.php/Glossary">OpenMdev Glossary</a>.
  If you're unsure what a specific term means you can read this for more context.

  \tableofcontents

  \section intro Introduction

  GVM is a GPU Virtual Machine for IOMMU-capable computers such as x86 and ARM.
  
  GVM-user is the latest release made public under the
  <a href="https://docs.linux-gvm.org/">GVM Project</a> developed by
  the <a href="https://openmdev.io/index.php/Main_Page">OpenMdev Open-Source software community</a>.
  
  GVM-user tools are designed to create and manage mediated <a href="https://openmdev.io/index.php/Virtual_IO_Internals">Virtual GPUs</a> as well as provide control of the
  mediated device to a VM running under a GVM-compatible platform hypervisor such
  as <a href="https://openmdev.io/index.php/Hypervisor_Support">KVM or Xen*</a>.

  This software is based on the C programming language and is released with Long Term
  Support under the <a href="https://opensource.org/licenses/gpl-2.0.php">GPLv2 License</a>.
  
  \section contrib Contributing

  Ongoing development occurs through collaboration by members of the OpenMdev Community.

  If you would like to contribute to GPU Virtual Machine development you can participate by submitting pull requests on GitHub to help us reach our <a href="https://github.com/orgs/OpenMdev/projects/1/views/1">GVM Development Targets</a> and participate in related <a href="https://openmdev.io/index.php/Articles#Communities_&_Mailing_Lists">Communities & Mailing Lists</a> such as the <a href="https://discord.gg/Rb9K9DYxKK">OpenMdev Community Discord</a>.

  If you would like to help suggest GVM Development Targets you can join the <a href="https://discord.gg/Rb9K9DYxKK">#Community-Call Room</a> to stay informed about when community meetings will occur.
  
  \section source-code Open Sources

  Open source code for GVM-user can be found on <a href="https://github.com/OpenMdev/GVM-user">GitHub</a>.

  \section code-standards Coding Standards

  GVM's coding standards are the same as those in the Linux Kernel.

  You can read relevant documentation about how software contributions to GVM should be structured from <a href="https://www.kernel.org/doc/html/v4.10/process/coding-style.html">this page on kernel.org</a>.
  
  \section doc-standards Documentation Standards

  All functions of the program must be documented using the <a href="https://doxygen.nl/index.html">Doxygen tool</a>.

  Appropriate descriptions, import graphs, parameters, returns, restrictions, lists of side effects, and lists of possible failures are required for each function of the program (<a href="https://docs.linux-gvm.org/gvm-user/d9/ddc/utils_2device_8h.html">example</a>).
  
  \section gpu-support Microarchitecture Support

  To see a full list of supported microarchitectures you can read the <a href="https://openmdev.io/index.php/GPU_Support">GPU Support Feature Matrix</a> on OpenMdev.

  
  \section onboarding Onboarding Device Support

  Expanding device support through additional vendors and architectures should be accomplished in several steps.

  These steps are as follows:

  <div class="itemizedlist">
  <ul type="disc">
  <li>
	  <p>Thorough documentation of functions related to I/O Mediation in a vendor driver-specific <a href="https://openmdev.io/index.php/Articles#GVM_Integration_Documents">GVM Integration Document</a>.</p>
  </li>
  <li>
	  <p>Formal verification (<a href="https://en.wikipedia.org/wiki/TLA%2B">TLA+</a>) of GVM interactions with driver logic.</p>
  </li>
  <li>
	  <p>GVM-driver integration via <a href="https://docs.linux-gvm.org/mdev-gpu/mdev-gpu-0.1.0.0/Common-Types.html">common types</a> (<a href="https://docs.linux-gvm.org/gvm-user/d8/d41/struct_m_dev_request.html">C example</a>) and <a href="https://docs.linux-gvm.org/mdev-gpu/">vendor driver-specific modules</a> (<a href="https://docs.linux-gvm.org/gvm-user/dir_4e562d255e853d122276fc2c4748b535.html">vendor driver directory</a>).</p>
  </li>
  <li>
	  <p>Unit testing and validation.</p>
  </li>
  <li>
	  <p>Deployment of support in packaged GVM utilities.</p>
  </li>
  </div>
  
  \section planned-support Planned Support Improvements

  Planned support improvements include:
  
  <div class="itemizedlist">
  <ul type="disc">
  <li>
	  <p>Support for Intel GPUs.</p>
  </li>
  <li>
	  <p>Support for TensTorrent TPUs.</p>
  </li>
  <li>
	  <p>Support for AMD GPUs.</p>
  </li>
  </div>

  \section side-effect Side Effects

  As this suite deals with low level components, we have taken to providing a detailed list of major side effects that occur in the functions we are dealing with to provide us a better understanding of where bugs can occur. This list can be found here: \ref side-effects.

  \section restrict Restrictions

  Several functions have a reduced domain, that cannot be properly modeled in C/C++.
  As a result we have the following additional restrictions on several of the functions:
  \ref restrictions.
  
  \section fail Failures
  
  Virtualization of the GPU incorporates many functions which may experience software failures. As a result we have taken to documenting all known possible failures: \ref failures.

  \section tests Tests

  To design a more professional codebase, we have started to use unit tests
  for different regions of our codebase. Currently we only have 2 major
  components, the user interaction system, and the GVM control system.

  -# GVM Control System Tests
    -# GPU Specific Tests.
      -# \subpage dev-creation-test
      -# \subpage nvidia-api-test
      -# \subpage nvidia-mgr-test
*/
