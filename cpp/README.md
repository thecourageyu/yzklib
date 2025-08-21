1. install cmake
2. install vcpkg 
   - git clone https://github.com/microsoft/vcpkg.git
   - cd to/vcpkg && ./bootstrap-vcpkg.sh (bash)
   - cd to\vcpkg; .\bootstrap-vcpkg.bat (powershell)
   - cd to/your/project
   - to/vcpkg/vcpkg new --application (vcpkg.json)
   - to/vcpkg/vcpkg add port fmt nlohmann-json