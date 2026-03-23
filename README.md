# 征途 (ZhengTu Online)

<p align="center">
  <img src="Supporting official website/images/logo.png" alt="征途 Logo" width="220"/>
</p>

<p align="center">
  <img src="Supporting official website/images/banner.jpg" alt="征途 Banner"/>
</p>

---

## 🎮 About The Game

**征途 (ZhengTu Online)** is a classic **2D MMORPG** developed by Giant Network.  
Originally released in the mid-2000s, it became one of China’s most successful online games, featuring massive PvP systems and large-scale nation warfare. :contentReference[oaicite:0]{index=0}  

---

## 🌐 Official Website

👉 https://zt.ztgame.com/

---

## 🖼️ In-Game Preview

<p align="center">
  <img src="Supporting official website/images/dd.jpg" alt="In-game Screenshot" width="700"/>
</p>

---

## ⚔️ Core Features

- 🏹 **Nation Warfare** – Large-scale PvP between entire nations  
- 🧙 **Character Progression** – Skills, leveling, gear upgrades  
- 💰 **Player Economy** – Trading, markets, and item systems  
- 🎯 **Quests & Events** – Structured progression and activities  
- 🎭 **Cosmetics & Systems** – Fashion, collections, enhancements  

---

## 📂 Project Structure (Placeholder)



---

## 🚀 Getting Started

### I. Directory Structure and File Description

#### 🧩 Core Servers

- **Base Server Core**  
  Contains shared logic, utility functions, database wrappers, and network abstractions.

- **BillServer**  
  Handles communication with platform services, including payment and currency conversion.

- **GatewayServer**  
  Routes player messages between client and backend services.

- **RecordServer**  
  File server responsible for persistent storage (no caching layer).

- **ScenesServer**  
  Core gameplay server handling:
  - Maps
  - Quests
  - Trading
  - PvP
  - Skills
  - NPCs  

- **SessionServer**  
  Maintains global player state:
  - Player sessions
  - Social systems (guilds, friends, family, mentor)
  - Battle & competition systems  

- **MiniServer**  
  Handles mini-games (e.g. card games, board games).

- **SuperServer**  
  Server manager / coordinator.

---

#### 📁 Data & Scripts

- **Map Configuration**  
  XML-based map configuration files (undocumented format).

- **New Quest System**  
  LUA-based scripting system.

- **Legacy Quest System**  
  XML-based task system (limited documentation).

- **Database Scripts**  
  SQL files for:
  - Schema creation  
  - Initial data setup  

---

#### 🛠️ Additional Components

- **Release Output Directory**  
  Generated build output (`make dist`).

- **Test Code**  
  Unit testing and experimental code.

- **Tools & Bots**  
  Internal tools, automation, and test utilities.

---

### II. Documents & Utilities

- **ChangeLog**  
  Generate using:
  ```bash
  make ChangeLog