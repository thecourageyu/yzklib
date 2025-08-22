1. install cmake
   sudo apt update
   sudo apt install cmake -y (Ubuntu)
2. install vcpkg 
   - git clone https://github.com/microsoft/vcpkg.git
   - cd vcpkg && ./bootstrap-vcpkg.sh (bash)
   - cd vcpkg; .\bootstrap-vcpkg.bat (powershell)
   - cd to/your/project (maybe you should add vcpkg into environment variables)
   - vcpkg new --application (create vcpkg.json)
   - vcpkg add port fmt nlohmann-json (update vcpkg.json)
   - vcpkg install (install packages in vcpkg.json)