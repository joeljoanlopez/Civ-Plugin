---
name: Code style — declarations vs implementations
description: Where to put method declarations and implementations in this C++ project
type: feedback
---

Header files should only have declarations. Implementation always goes in the corresponding .cpp file.

**Why:** User's explicit preference for clean separation between interface and implementation.

**How to apply:** Even for short methods, put the body in the .cpp file. For template methods that must be in the header, use explicit instantiation in the .cpp.
