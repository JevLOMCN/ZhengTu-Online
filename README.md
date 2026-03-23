# 征途 (ZhengTu Online)

<p align="center">
  <img src="Supporting official website/images/logo.png" alt="征途 Logo" width="220"/>
</p>

<p align="center">
  <img src="Supporting official website/images/banner.jpg" alt="征途 Banner"/>
</p>

---

## 🎮 About The Game

征途 (ZhengTu Online) is a classic 2D MMORPG developed by Giant Network.  
Released in the mid-2000s, it became one of China’s most successful online games, known for its massive PvP systems and nation-based warfare.

At its peak, the game reached millions of players and supported extremely large concurrent battles.

---

## 🌐 Official Website

https://zt.ztgame.com/

---

## 🖼️ In-Game Preview

<p align="center">
  <img src="Supporting official website/images/dd.jpg" alt="In-game Screenshot" width="700"/>
</p>

---

## ⚔️ Core Features

- Nation Warfare – Large-scale PvP between entire nations  
- Character Progression – Skills, leveling, gear upgrades  
- Player Economy – Trading, markets, and item systems  
- Quests & Events – Structured progression and activities  
- Cosmetics & Systems – Fashion, collections, enhancements  

---

## 📂 Project Structure (Placeholder)

/Assets        # Placeholder - game assets  
/Code          # Placeholder - client/server source  
/Tools         # Placeholder - utilities and tools  

---

## 🚀 Getting Started

### I. Directory Structure and File Description

Core Servers:

- Base Server Core  
  Contains shared logic, utility functions, database wrappers, and network abstractions.

- BillServer  
  Handles communication with platform systems, including point card and currency conversion.

- GatewayServer  
  Routes player messages between client and backend services.

- RecordServer  
  File server responsible for persistent storage (no caching layer).

- ScenesServer  
  Handles map management, quest system, trading, PvP, skills, and NPC logic.

- SessionServer  
  Maintains global player state including social systems (guild, friends, mentor, family), battle systems, and competition systems.

- MiniServer  
  Provides mini-games such as card and board games.

- SuperServer  
  Server manager / coordinator.

---

Data & Scripts:

- Map Configuration  
  XML-based map configuration (undocumented).

- New Quest System  
  LUA-based scripting.

- Legacy Quest System  
  XML-based scripts.

- Database Scripts  
  SQL files for database creation and initialization.

---

Additional Components:

- Release Output  
  Generated via: make dist

- Test Code  
  Unit tests and experimental code.

- Tools / Bots  
  Internal tools and automation.

---

### II. Documents & Utilities

ChangeLog:
make ChangeLog

Doxyfile  
Documentation generator configuration.

Makefile  
Build script.

config.xml.example  
Must be copied to config.xml and configured.

Scripts:
- findexp (search script)  
- forbidWords (word filter)  
- go (server startup script)  
- gnuify-changelog.pl (ChangeLog generator)  
- Hellow.lua (example LUA script)  

Tags (ctags):
make ctags

---

### III. Setup & Run

1. Database Setup  
- Create database manually  
- Execute SQL scripts  
- No automated installer provided  

2. Configuration  

cp config.xml.example config.xml  

- Edit database connection settings inside config.xml  

3. Build & Run  

make  
./go  

---

### IV. Required Third-Party Libraries

gd  
http://www.boutell.com/gd  

---

## 📌 Notes

- Based on the original 征途 MMORPG  
- Intended for development, learning, and preservation  
- Some systems and formats remain undocumented  

---

## 📜 License

This project is provided for educational purposes only.  
All original assets, trademarks, and content belong to their respective owners.

---

## ❤️ Credits

- Giant Network  
- Official Website Assets  
- Whoever leaked this
