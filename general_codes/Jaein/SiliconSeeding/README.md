# SiliconSeedAna Framework

**Recent update**: March 30, 2025

This project is designed to simulate and analyze charged particle tracking and calorimeter matching in the sPHENIX environment. The core component, `SiliconSeedAna`, collects information from silicon trackers, EMCal, and HCal, and stores it in a structured ROOT file for further physics analysis.

---

## 📁 Directory Overview

### 1. `PHYTIAmacro/`

- **Purpose**: Contains macros for full simulations based on PYTHIA event generation.
- **Main Macro**: `Fun4All_PHYTIA_Silicon.C`
- **Usage**:
  1. First, compile `SiliconSeedAna` and build the shared library.
  2. Then run this macro to simulate PYTHIA events.
- **Output**: ROOT file containing tracker and calorimeter hit information.

---

### 2. `gunmacro/`

- **Purpose**: Contains macros for single-particle simulations.
- **Main Macro**: `Fun4All_singleParticle_Silicon.C`
- **Usage**:
  1. Compile `SiliconSeedAna` first.
  2. Run the macro to simulate single-particle events with fixed momentum and direction.
- **Use case**: Useful for analyzing magnetic deflection and detector matching at specific kinematics.

---

### 3. `SiliconSeedAna/`

- **Purpose**: Core analyzer that collects and stores track and calorimeter information into ROOT TTrees.
- **Key Files**:
  - `SiliconSeedAna.cc`
  - `SiliconSeedAna.h`
- **Features**:
  - Extracts hits from silicon detectors, EMCal, and HCal.
  - Projects track states to the EMCal radius (e.g., 93.5 cm).
  - Saves detailed information such as:
    - Initial track position and momentum
    - Projected EMCal coordinates
    - Matching-related variables
- **Build Instructions**:
  ```bash
  source setup.sh
  ./autogen.sh
  ./configure
  make -j
